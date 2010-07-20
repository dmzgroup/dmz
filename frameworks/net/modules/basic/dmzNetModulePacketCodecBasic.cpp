#include "dmzNetModulePacketCodecBasic.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeLoadPlugins.h>
#include <dmzSystemMarshal.h>
#include <dmzSystemUnmarshal.h>

/*!

\class dmz::NetModulePacketCodecBasic
\ingroup Net
\brief Basic Packet Codec Module.
\details The Basic Codec Module parses the network packet's header and decodes the
packet with the correct codec extension. It will also write the packet header before using
the appropriate codec to encode the packet.
\code
<local-scope>
   <header>
      <element type="Element Type" base="Element Base"/>
      <element type="const" base="Element Base" value="Const Value"/>
      ...
   </header>
   <packet id="Packet ID" name="Codec Name">
      <object-type name="Object Type Name"/>
      ...
   </packet>
   <packet id="Packet ID" name="Codec Name">
      <event-type name="Event Type Name"/>
      ...
   </packet>
   <plugin-list>
      <plugin name="Net Extention Name"/>
   </plugin-list>
</local-scope>
\endcode
Possible type attribute values: const, id, and size.\n
Possible base attribute values: uint8, uint16, uint32, uint64,
int8, int16, int32, and int64.\n
The const type may have a value attribute where the const value is specified.

*/

//! \cond
dmz::Boolean
dmz::NetModulePacketCodecBasic::HeaderElement::read_header (
      Unmarshal &data,
      Handle &packetHandle) {

   Boolean result (True);

   if (read_element (data, packetHandle)) {

      HeaderElement *current (next);

      while (current) {

         result = current->read_element (data, packetHandle);

         if (!result) { current = 0; }
         else { current = current->next; }
      }
   }
   else { result = False; }

   return result;
}


dmz::Boolean
dmz::NetModulePacketCodecBasic::HeaderElement::write_header (
      const Handle PacketID,
      Marshal &data) {

   Boolean result (True);

   if (write_element (PacketID, data)) {

      HeaderElement *current (next);

      while (current) {

         result = current->write_element (PacketID, data);

         if (!result) { current = 0; }
         else { current = current->next; }
      }
   }
   else { result = False; }

   return result;
}


dmz::NetModulePacketCodecBasic::NetModulePacketCodecBasic (
      const PluginInfo &Info,
      Config &local,
      Config &global) :
      Plugin (Info),
      NetModulePacketCodec (Info),
      _log (Info),
      _extensions (Info.get_context (), &_log),
      _headerCodec (0) {

   _init (local, global);
}


dmz::NetModulePacketCodecBasic::~NetModulePacketCodecBasic () {

   _objTypeTable.clear ();
   _objTable.clear ();
   _objEncodeTable.empty ();
   _eventTypeTable.clear ();
   _eventEncodeTable.empty ();
   _decodeTable.empty ();
   _packetTable.empty ();

   if (_headerCodec) { delete _headerCodec; _headerCodec = 0; }
}


// Plugin Interface
void
dmz::NetModulePacketCodecBasic::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateStart) { _extensions.start_plugins (); }
   else if (State == PluginStateStop) { _extensions.stop_plugins (); }
   else if (State == PluginStateShutdown) { _extensions.shutdown_plugins (); }
}


void
dmz::NetModulePacketCodecBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      _discover_codec (PluginPtr);
      _extensions.discover_external_plugin (PluginPtr);
   }
   else if (Mode == PluginDiscoverAdd) {

      _remove_codec (PluginPtr);
      _extensions.remove_external_plugin (PluginPtr);
   }
}


// NetModulePacketCodec Interface
void
dmz::NetModulePacketCodecBasic::get_supported_objects (ObjectTypeSet &objects) {

   HashTableStringIterator it;
   PacketStruct *ps (_packetTable.get_first (it));

   while (ps) {

      objects.add_object_set (ps->objects);
      ps = _packetTable.get_next (it);
   }
}


void
dmz::NetModulePacketCodecBasic::get_supported_events (EventTypeSet &events) {

   HashTableStringIterator it;
   PacketStruct *ps (_packetTable.get_first (it));

   while (ps) {

      events.add_event_set (ps->events);
      ps = _packetTable.get_next (it);
   }
}


dmz::Boolean
dmz::NetModulePacketCodecBasic::decode (Unmarshal &inData, Boolean &isLoopback) {

   Boolean result (False);

   if (_headerCodec) {

      Handle handle (0);

      if (_headerCodec->read_header (inData, handle)) {

         DecodeStruct *ds (_decodeTable.lookup (handle));

         if (ds) { result = ds->decoder.decode (inData, isLoopback); }
      }
   }

   return result;
}


dmz::Boolean
dmz::NetModulePacketCodecBasic::register_object (
      const Handle ObjectHandle,
      const ObjectType &Type,
      Marshal &outData) {

   Boolean result (False);

   _objTable.remove (ObjectHandle);

   ObjectType current (Type);

   EncodeObjectStruct *eos (0);

   do {

      eos = _objTypeTable.lookup (current.get_handle ());
      current.become_parent ();

   } while (current && !eos);

   if (ObjectHandle && eos && _objTable.store (ObjectHandle, eos)) {

      eos->objects.add_handle (ObjectHandle);
      result = _write_object (ObjectHandle, NetObjectActivate, outData);
   }

   return result;
}


dmz::Boolean
dmz::NetModulePacketCodecBasic::encode_object (
      const Handle ObjectHandle,
      Marshal &outData) {

   return _write_object (ObjectHandle, NetObjectUpdate, outData);
}


dmz::Boolean
dmz::NetModulePacketCodecBasic::release_object (
      const Handle ObjectHandle,
      Marshal &outData) {

   _write_object (ObjectHandle, NetObjectDeactivate, outData);

   EncodeObjectStruct *eos = _objTable.remove (ObjectHandle);

   if (eos) { eos->objects.remove_handle (ObjectHandle); }

   return eos != 0;
}


dmz::Boolean
dmz::NetModulePacketCodecBasic::encode_event (
      const EventType &Type,
      const Handle EventHandle,
      Marshal &outData) {

   Boolean result (False);

   EncodeEventStruct *ees (_eventTypeTable.lookup (Type.get_handle ()));

   if (!ees) {

      EventType current (Type); current.become_parent ();

      while (!ees && current) {

         ees = _eventTypeTable.lookup (current.get_handle ());
         current.become_parent ();
      }
   }

   if (ees && _headerCodec) {

      const Int32 Place (outData.get_place ());
      if (_headerCodec->write_header (ees->PacketID, outData)) {

         if (ees->codec.encode_event (EventHandle, outData)) {

            outData.set_place (Place);
            result = _headerCodec->write_header (ees->PacketID, outData);
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::NetModulePacketCodecBasic::_write_object (
      const Handle ObjectHandle,
      const NetObjectEncodeEnum Mode,
      Marshal &outData) {

   Boolean result (False);

   EncodeObjectStruct *eos (_objTable.lookup (ObjectHandle));

   if (eos && _headerCodec) {

      const Int32 Place (outData.get_place ());
      if (_headerCodec->write_header (eos->PacketID, outData)) {

         if (eos->codec.encode_object (ObjectHandle, Mode, outData)) {

            outData.set_place (Place);
            result = _headerCodec->write_header (eos->PacketID, outData);
         }
      }
   }

   return result;
}


void
dmz::NetModulePacketCodecBasic::_discover_codec (const Plugin *PluginPtr) {

   const String Name (PluginPtr ? PluginPtr->get_plugin_name () : "");

   NetExtPacketCodecObject *objCodec (NetExtPacketCodecObject::cast (PluginPtr));
   NetExtPacketCodecEvent *eventCodec (NetExtPacketCodecEvent::cast (PluginPtr));

   if (Name && (objCodec || eventCodec)) {

      PacketStruct *hs (_packetTable.lookup (Name));

      if (hs) {

         DecodeStruct *ds (0);
         if (objCodec) { ds = new DecodeStruct (hs->PacketID, *objCodec); }
         else if (eventCodec) { ds = new DecodeStruct (hs->PacketID, *eventCodec); }

         if (ds && _decodeTable.store (hs->PacketID, ds)) {

            if (objCodec && hs->objects.get_count ()) {

               EncodeObjectStruct *eos (
                  new EncodeObjectStruct (hs->PacketID, *objCodec));

               if (eos && _objEncodeTable.store (hs->PacketID, eos)) {

                  RuntimeIterator it;
                  ObjectType type;

                  while (hs->objects.get_next (it, type)) {

                     _objTypeTable.store (type.get_handle (), eos);
                  }
               }
               else if (eos) { delete eos; eos = 0; }
            }
            else if (eventCodec && hs->events.get_count ()) {

               EncodeEventStruct *ees (
                  new EncodeEventStruct (hs->PacketID, *eventCodec));

               if (ees && _eventEncodeTable.store (hs->PacketID, ees)) {

                  RuntimeIterator it;

                  EventType type;

                  Boolean found (hs->events.get_first (it, type));

                  while (found) {

                     _eventTypeTable.store (type.get_handle (), ees);
                     found = hs->events.get_next (it, type);
                  }
               }
               else if (ees) { delete ees; ees = 0; }
            }
         }
         else if (ds) {

            delete ds; ds = 0;
         }
      }
   }
}


void
dmz::NetModulePacketCodecBasic::_remove_codec (const Plugin *PluginPtr) {

   const String Name (PluginPtr ? PluginPtr->get_plugin_name () : "");

   NetExtPacketCodecObject *objCodec (NetExtPacketCodecObject::cast (PluginPtr));
   NetExtPacketCodecEvent *eventCodec (NetExtPacketCodecEvent::cast (PluginPtr));

   if (Name && (objCodec || eventCodec)) {

      PacketStruct *hs (_packetTable.lookup (Name));

      if (hs) {

         DecodeStruct *ds = _decodeTable.remove (hs->PacketID);

         if (ds) { delete ds; ds = 0; }

         if (objCodec && hs->objects.get_count ()) {

            RuntimeIterator it;

            ObjectType type;

            while (hs->objects.get_next (it, type)) {

               _objTypeTable.remove (type.get_handle ());
            }

            EncodeObjectStruct *eos = _objEncodeTable.remove (hs->PacketID);

            if (eos) {

               Handle obj = eos->objects.get_first ();

               while (obj) {

                  _objTable.remove (obj);

                  obj = eos->objects.get_next ();
               }

               delete eos; eos = 0;
            }
         }
         else if (eventCodec && hs->events.get_count ()) {

            RuntimeIterator it;

            EventType type;

            while (hs->events.get_next (it, type)) {

               _eventTypeTable.remove (type.get_handle ());
            }

            EncodeEventStruct *ees = _eventEncodeTable.remove (hs->PacketID);

            if (ees) { delete ees; ees = 0; }
         }
      }
   }
}


void
dmz::NetModulePacketCodecBasic::_init (Config &local, Config &global) {

   Config packetList;

   if (local.lookup_all_config ("packet", packetList)) {

      ConfigIterator it;
      Config packet;

      while (packetList.get_next_config (it, packet)) {

         const String Name (config_to_string ("name", packet));
         const Handle PacketID (config_to_uint32 ("id", packet));

         if (Name && PacketID) {

            PacketStruct *ps (new PacketStruct (Name, PacketID));

            if (ps && _packetTable.store (Name, ps)) {

               Config objectList;

               packet.lookup_all_config ("object-type", objectList);

               ConfigIterator objectIt;

               Config object;

               while (objectList.get_next_config (objectIt, object)) {

                  const String TypeName (config_to_string ("name", object));

                  if (!ps->objects.add_object_type (
                        TypeName,
                        get_plugin_runtime_context ())) {

                     _log.error << "Unknown object type: " << TypeName
                        << ". Unable to bind to codec: " << Name
                        << " with id: " << PacketID << endl;
                  }
               }

               Config eventList;

               packet.lookup_all_config ("event-type", eventList);

               ConfigIterator eventIt;

               Config event;

               while (eventList.get_next_config (eventIt, event)) {

                  const String TypeName (config_to_string ("name", event));

                  if (!ps->events.add_event_type (
                        TypeName,
                        get_plugin_runtime_context ())) {

                     _log.error << "Unknown event type: " << TypeName
                        << ". Unable to bind to codec: " << Name
                        << " with id: " << PacketID << endl;
                  }
               }
            }
            else if (ps) {

               delete ps; ps = 0;
            }
         }
      }

      _build_header_codec (local);
   }

   Config pluginList;

   if (local.lookup_all_config ("plugin-list.plugin", pluginList)) {

      RuntimeContext *context (get_plugin_runtime_context ());

      Config init;
      global.lookup_all_config_merged ("dmz", init);

      if (dmz::load_plugins (context, pluginList, init, global, _extensions, &_log)) {

         _extensions.discover_plugins ();

         RuntimeIterator it;

         Plugin *ptr (_extensions.get_first (it));

         while (ptr) {

            _discover_codec (ptr);
            ptr = _extensions.get_next (it);
         }
      }
   }
   else {

   }
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzNetModulePacketCodecBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::NetModulePacketCodecBasic (Info, local, global);
}

};
