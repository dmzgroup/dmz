#include "dmzNetModulePacketCodecBasic.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeLoadPlugins.h>
#include <dmzSystemMarshal.h>
#include <dmzSystemUnmarshal.h>

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
      const Handle PacketHandle,
      Marshal &data) {

   Boolean result (True);

   if (write_element (PacketHandle, data)) {

      HeaderElement *current (next);

      while (current) {

         result = current->write_element (PacketHandle, data);

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

   if (Mode == PluginDiscoverAdd) { _extensions.discover_external_plugin (PluginPtr); }
   else if (Mode == PluginDiscoverAdd) { _extensions.remove_external_plugin (PluginPtr); }
}


// NetModulePacketCodec Interface
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

   return _objTable.remove (ObjectHandle) != 0;
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
      if (_headerCodec->write_header (ees->PacketHandle, outData)) {

         if (ees->codec.encode_event (EventHandle, outData)) {

            outData.set_place (Place);
            result = _headerCodec->write_header (ees->PacketHandle, outData);
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
      if (_headerCodec->write_header (eos->PacketHandle, outData)) {

         if (eos->codec.encode_object (ObjectHandle, Mode, outData)) {

            outData.set_place (Place);
            result = _headerCodec->write_header (eos->PacketHandle, outData);
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
         if (objCodec) { ds = new DecodeStruct (hs->PacketHandle, *objCodec); }
         else if (eventCodec) { ds = new DecodeStruct (hs->PacketHandle, *eventCodec); }

         if (ds && _decodeTable.store (hs->PacketHandle, ds)) {

            if (objCodec && hs->objects.get_count ()) {

               EncodeObjectStruct *eos (
                  new EncodeObjectStruct (hs->PacketHandle, *objCodec));

               if (eos && _objEncodeTable.store (hs->PacketHandle, eos)) {

                  ObjectTypeIterator it;

                  ObjectType type;

                  Boolean found (hs->objects.get_first (it, type));

                  while (found) {

                     _objTypeTable.store (type.get_handle (), eos);
                     found = hs->objects.get_next (it, type);
                  }
               }
               else if (eos) { delete eos; eos = 0; }
            }
            else if (eventCodec && hs->events.get_count ()) {

               EncodeEventStruct *ees (
                  new EncodeEventStruct (hs->PacketHandle, *eventCodec));

               if (ees && _eventEncodeTable.store (hs->PacketHandle, ees)) {

                  EventTypeIterator it;

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
dmz::NetModulePacketCodecBasic::_init (Config &local, Config &global) {

   Config packetList;

   if (local.lookup_all_config ("packet", packetList)) {

      ConfigIterator it;

      Config packet;

      Boolean found (packetList.get_first_config (it, packet));

      while (found) {

         const String Name (config_to_string ("name", packet));
         const Handle PacketHandle (config_to_uint32 ("handle", packet));

         if (Name && PacketHandle) {

            PacketStruct *ps (new PacketStruct (Name, PacketHandle));

            if (ps && _packetTable.store (Name, ps)) {

               Config objectList;

               packet.lookup_all_config ("object", objectList);

               ConfigIterator objectIt;

               Config object;

               Boolean objectFound (objectList.get_first_config (objectIt, object));

               while (objectFound) {

                  const String TypeName (config_to_string ("type", object));

                  if (!ps->objects.add_object_type (
                        TypeName,
                        get_plugin_runtime_context ())) {

                     _log.error << "Unknown object type: " << TypeName
                        << ". Unable to bind to codec: " << Name
                        << " with handle: " << PacketHandle << endl;
                  }

                  objectFound = objectList.get_next_config (objectIt, object);
               }

               Config eventList;

               packet.lookup_all_config ("event", eventList);

               ConfigIterator eventIt;

               Config event;

               Boolean eventFound (eventList.get_first_config (eventIt, event));

               while (eventFound) {

                  const String TypeName (config_to_string ("type", event));

                  if (!ps->events.add_event_type (
                        TypeName,
                        get_plugin_runtime_context ())) {

                     _log.error << "Unknown event type: " << TypeName
                        << ". Unable to bind to codec: " << Name
                        << " with handle: " << PacketHandle << endl;
                  }

                  eventFound = eventList.get_next_config (eventIt, event);
               }
            }
            else if (ps) {

               delete ps; ps = 0;
            }
         }

         found = packetList.get_next_config (it, packet);
      }

      _build_header_codec (local);
   }

   Config pluginList;

   if (local.lookup_all_config ("plugins.plugin", pluginList)) {

      RuntimeContext *context (get_plugin_runtime_context ());

      if (dmz::load_plugins (context, pluginList, local, global, _extensions, &_log)) {

         _extensions.discover_plugins ();

         PluginIterator it;

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


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzNetModulePacketCodecBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::NetModulePacketCodecBasic (Info, local, global);
}

};
