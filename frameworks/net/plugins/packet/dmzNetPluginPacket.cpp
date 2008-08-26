#include <dmzEventCallbackMasks.h>
#include <dmzEventModule.h>
#include "dmzNetPluginPacket.h"
#include <dmzObjectAttributeMasks.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesUUID.h>

dmz::NetPluginPacket::NetPluginPacket (
      const PluginInfo &Info,
      const ByteOrderEnum Endian,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      NetPacketObserver (Info),
      EventObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _drMod (0),
      _codecMod (0),
      _ioMod (0),
      _outData (Endian),
      _inData (Endian) {

   _init (local);

   activate_default_object_attribute (
      ObjectCreateMask | ObjectDestroyMask | ObjectLocalityMask | ObjectTypeMask);
}


dmz::NetPluginPacket::~NetPluginPacket () {

   _objTable.empty ();
   _preRegObjTable.empty ();
}


// Plugin Interface
void
dmz::NetPluginPacket::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if ((State == PluginStateStart) && _codecMod && _ioMod) {

      HashTableHandleIterator it;

      ObjStruct *ptr (_preRegObjTable.get_first (it));

      while (ptr) {

         _outData.reset ();

         if (_codecMod->register_object (ptr->ObjectHandle, ptr->type, _outData)) {

            if (_objTable.store (ptr->ObjectHandle, ptr)) {

               _ioMod->write_packet (_outData.get_length (), _outData.get_buffer ());
            }
            else {

               _outData.reset ();
               _codecMod->release_object (ptr->ObjectHandle, _outData);
               delete ptr; ptr = 0;
            }
         }

         ptr = _preRegObjTable.get_next (it);
      }

      _preRegObjTable.clear ();
   }
   else if (State == PluginStateShutdown) {

      // Sends deactivation packet for all object that haven't been destroyed yet.
      HashTableHandleIterator it;
      UUID empty;
      ObjStruct *ptr (_objTable.get_first (it));

      while (ptr) {

         destroy_object (empty, ptr->ObjectHandle);
         ptr = _objTable.get_next (it);
      }
   }
}


void
dmz::NetPluginPacket::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_drMod) { _drMod = NetModuleLocalDR::cast (PluginPtr); }

      if (!_codecMod) {

         _codecMod = NetModulePacketCodec::cast (PluginPtr);

         if (_codecMod) {

            EventTypeSet events;
            _codecMod->get_supported_events (events);

            EventTypeIterator it;
            EventType type;

            while (events.get_next (it, type)) {

               activate_event_callback (type, EventCloseMask);
            }
         }
      }

      if (!_ioMod) {

         _ioMod = NetModulePacketIO::cast (PluginPtr);

         if (_ioMod) { _ioMod->register_packet_observer (*this); }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_drMod && (_drMod = NetModuleLocalDR::cast (PluginPtr))) { _drMod = 0; }

      if (_codecMod && (_codecMod == NetModulePacketCodec::cast (PluginPtr))) {

         HashTableHandleIterator it;

         ObjStruct *os (_objTable.get_first (it));

         while (os) {

            if (!_preRegObjTable.store (os->ObjectHandle, os)) { delete os; os = 0; }

            os = _objTable.get_next (it);
         }

         _objTable.clear ();

         EventTypeSet events;
         _codecMod->get_supported_events (events);

         EventTypeIterator eventIt;
         EventType type;

         while (events.get_next (eventIt, type)) {

            deactivate_event_callback (type, EventCloseMask);
         }

         _codecMod = 0;
      }

      if (_ioMod && (_ioMod == NetModulePacketIO::cast (PluginPtr))) {

         _ioMod->release_packet_observer (*this);
         _ioMod = 0;
      }
   }
}


// TimeSlice Interface
void
dmz::NetPluginPacket::update_time_slice (const Float64 TimeDelta) {

   if (_codecMod && _ioMod) {

      HashTableHandleIterator it;

      ObjStruct *os (_objTable.get_first (it));

      while (os) {

         _outData.reset ();

         Boolean update (_drMod ? _drMod->update_object (os->ObjectHandle) : True);

         if (update && _codecMod->encode_object (os->ObjectHandle, _outData)) {

            _ioMod->write_packet (_outData.get_length (), _outData.get_buffer ());
         }

         os = _objTable.get_next (it);
      }
   }
}


// NetPacketObserver Interface
void
dmz::NetPluginPacket::read_packet (const Int32 Size, char *buffer) {

   if (buffer && Size && _codecMod) {

      _inData.set_buffer (Size, buffer);
      Boolean isLoopback (False);
      _codecMod->decode (_inData, isLoopback);
   }
}


// EventObserverUtil Interface
void
dmz::NetPluginPacket::close_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

   if (Locality == EventLocal) {

      if (_codecMod && _ioMod) {

         _outData.reset ();

         if (_codecMod->encode_event (Type, EventHandle, _outData)) {

            _ioMod->write_packet (_outData.get_length (), _outData.get_buffer ());
         }
      }
   }
}


// ObjectObserverUtil Interface
void
dmz::NetPluginPacket::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (Locality == ObjectLocal) {

      if (_codecMod && _ioMod) {

         _outData.reset ();

         if (_codecMod->register_object (ObjectHandle, Type, _outData)) {

            ObjStruct *ptr (new ObjStruct (ObjectHandle, Type));

            if (ptr && !_objTable.store (ObjectHandle, ptr)) { delete ptr; ptr = 0; }
            else if (ptr) {

               _ioMod->write_packet (_outData.get_length (), _outData.get_buffer ());
            }
         }
      }
      else {

         ObjStruct *ptr (new ObjStruct (ObjectHandle, Type));

         if (ptr && !_preRegObjTable.store (ObjectHandle, ptr)) { delete ptr; ptr = 0; }
      }
   }
}


void
dmz::NetPluginPacket::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   ObjStruct *ptr (_objTable.remove (ObjectHandle));

   if (ptr) {

      _outData.reset ();

      if (_codecMod && _ioMod && _codecMod->release_object (ObjectHandle, _outData)) {

         _ioMod->write_packet (_outData.get_length (), _outData.get_buffer ());
      }

      delete ptr; ptr = 0;
   }
   else {

      ptr = _preRegObjTable.remove (ObjectHandle);

      if (ptr) { delete ptr; ptr = 0; }
   }
}


void
dmz::NetPluginPacket::update_object_locality (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectLocalityEnum Locality,
      const ObjectLocalityEnum PrevLocality) {

}


void
dmz::NetPluginPacket::update_object_type (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const ObjectType &Value,
      const ObjectType *PreviousValue) {

}


// Internal Interface
void
dmz::NetPluginPacket::_init (Config &local) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzNetPluginPacket (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   dmz::ByteOrderEnum endian (dmz::ByteOrderBigEndian);

   const dmz::String ByteOrderName (
      dmz::config_to_string ("endian", local).to_lower ());

   if (ByteOrderName == "little") { endian = dmz::ByteOrderLittleEndian; }
   else if (ByteOrderName == "big") { endian = dmz::ByteOrderBigEndian; }
   else if (ByteOrderName) {

      dmz::Log log (Info);

      log.error << "Unknown byte order: " << ByteOrderName << " using Big Endian"
         << dmz::endl;
   }

   return new dmz::NetPluginPacket (Info, endian, local);
}

};
