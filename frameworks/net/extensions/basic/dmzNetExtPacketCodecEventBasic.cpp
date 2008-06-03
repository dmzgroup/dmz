#include "dmzNetExtPacketCodecEventBasic.h"
#include <dmzNetModuleAttributeMap.h>
#include <dmzNetModuleIdentityMap.h>
#include <dmzEventConsts.h>
#include <dmzEventModule.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzSystemMarshal.h>
#include <dmzSystemUnmarshal.h>
#include <dmzTypesMask.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

dmz::NetExtPacketCodecEventBasic::NetExtPacketCodecEventBasic (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      NetExtPacketCodecEvent (Info),
      _log (Info),
      _time (Info.get_context ()),
      _defaultHandle (0),
      _sourceHandle (0),
      _targetHandle (0),
      _munitionHandle (0),
      _fireHandle (1),
      _detonateHandle (2),
      _collisionHandle (3),
      _eventMod (0),
      _attrMod (0),
      _idMod (0) {

   _init (local);
}


dmz::NetExtPacketCodecEventBasic::~NetExtPacketCodecEventBasic () {

}


// Plugin Interface
void
dmz::NetExtPacketCodecEventBasic::discover_plugin (const Plugin *PluginPtr) {

   if (!_eventMod) {

      _eventMod = EventModule::cast (PluginPtr);
   }

   if (!_attrMod) { _attrMod = NetModuleAttributeMap::cast (PluginPtr); }
   if (!_idMod) { _idMod = NetModuleIdentityMap::cast (PluginPtr); }
}


void
dmz::NetExtPacketCodecEventBasic::start_plugin () {

}


void
dmz::NetExtPacketCodecEventBasic::stop_plugin () {

}


void
dmz::NetExtPacketCodecEventBasic::shutdown_plugin () {

}


void
dmz::NetExtPacketCodecEventBasic::remove_plugin (const Plugin *PluginPtr) {

   if (_eventMod && (_eventMod = EventModule::cast (PluginPtr))) {

      _defaultHandle = 0;
      _sourceHandle = 0;
      _targetHandle = 0;
      _munitionHandle = 0;
      _eventMod = 0;
   }

   if (_attrMod && (_attrMod == NetModuleAttributeMap::cast (PluginPtr))) {

      _attrMod = 0;
   }

   if (_idMod && (_idMod == NetModuleIdentityMap::cast (PluginPtr))) { _idMod = 0; }
}


// NetExtPacketCodecEvent Interface
dmz::Boolean
dmz::NetExtPacketCodecEventBasic::decode (Unmarshal &data, Boolean &isLoopback) {

   Boolean result (False);

   if (_idMod && _attrMod && _eventMod) {

      const UInt32 TypeEnum (data.get_next_uint32 ());
      const UInt32 SourceSite (data.get_next_uint32 ());
      const UInt32 SourceHost (data.get_next_uint32 ());
      const UInt32 SourceEntity (data.get_next_uint32 ());

      if ((_idMod->get_site_handle () == SourceSite) &&
            (_idMod->get_host_handle () == SourceHost)) {

         isLoopback = True;
      }
      else {

         Handle sourceHandle (0);
         Handle targetHandle (0);
         Handle munitionHandle (0);

         _idMod->lookup_handle_from_site_host_entity (
            SourceSite,
            SourceHost,
            SourceEntity,
            sourceHandle);

         EventType type;
         ObjectType munitionType;

         if (TypeEnum == _fireHandle) { type = _fireType; }
         else if (TypeEnum == _detonateHandle) { type = _detonateType; }
         else if (TypeEnum == _collisionHandle) { type = _collisionType; }

         const UInt32 TargetSite (data.get_next_uint32 ());
         const UInt32 TargetHost (data.get_next_uint32 ());
         const UInt32 TargetEntity (data.get_next_uint32 ());

         _idMod->lookup_handle_from_site_host_entity (
            TargetSite,
            TargetHost,
            TargetEntity,
            targetHandle);

         if ((TypeEnum == _fireHandle) || (TypeEnum == _detonateHandle)) {

            const UInt32 MunitionSite (data.get_next_uint32 ());
            const UInt32 MunitionHost (data.get_next_uint32 ());
            const UInt32 MunitionEntity (data.get_next_uint32 ());

            _idMod->lookup_handle_from_site_host_entity (
               MunitionSite,
               MunitionHost,
               MunitionEntity,
               munitionHandle);

            ArrayUInt32 typeArray;

            typeArray.set (0, data.get_next_uint32 ());
            typeArray.set (1, data.get_next_uint32 ());
            typeArray.set (2, data.get_next_uint32 ());

            _attrMod->to_internal_type (typeArray, munitionType);
         }

         Vector pos;
         Vector offset;

         data.get_next_vector (pos);
         data.get_next_vector (offset);

         const Handle EventHandle (_eventMod->start_event (type, EventRemote));

         if (EventHandle) {

            _eventMod->store_object_handle (EventHandle, _sourceHandle, sourceHandle);
            _eventMod->store_object_handle (EventHandle, _targetHandle, targetHandle);

            if ((TypeEnum == _fireHandle) || (TypeEnum == _detonateHandle)) {

               _eventMod->store_object_handle (
                  EventHandle,
                  _munitionHandle,
                  munitionHandle);

               _eventMod->store_object_type (EventHandle, _munitionHandle, munitionType);
            }

            _eventMod->store_position (EventHandle, _defaultHandle, pos);
            _eventMod->store_vector (EventHandle, _targetHandle, offset);

            result = _eventMod->end_event (EventHandle);
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::NetExtPacketCodecEventBasic::encode_event (
      const Handle EventHandle,
      Marshal &data) {

   Boolean result (False);

   if (_idMod && _attrMod && _eventMod) {

      UInt32 typeEnum (0);
      EventType type;
      _eventMod->lookup_event_type (EventHandle, type);

      if (type.is_of_type (_detonateType)) { typeEnum = _detonateHandle; }
      else if (type.is_of_type (_fireType)) { typeEnum = _fireHandle; }
      else if (type.is_of_type (_collisionType)) { typeEnum = _collisionHandle; }

       data.set_next_uint32 (typeEnum);

      Handle sourceHandle (0);

      UInt32 sourceSite (0);
      UInt32 sourceHost (0);
      UInt32 sourceEntity (0);

      if (_eventMod->lookup_object_handle (EventHandle, _sourceHandle, sourceHandle)) {

         _idMod->lookup_site_host_entity (
            sourceHandle,
            sourceSite,
            sourceHost,
            sourceEntity);
      }

      data.set_next_uint32 (sourceSite);
      data.set_next_uint32 (sourceHost);
      data.set_next_uint32 (sourceEntity);

      Handle targetHandle (0);

      UInt32 targetSite (0);
      UInt32 targetHost (0);
      UInt32 targetEntity (0);

      if (_eventMod->lookup_object_handle (EventHandle, _targetHandle, targetHandle)) {

         _idMod->lookup_site_host_entity (
            targetHandle,
            targetSite,
            targetHost,
            targetEntity);
      }

      data.set_next_uint32 (targetSite);
      data.set_next_uint32 (targetHost);
      data.set_next_uint32 (targetEntity);

      if ((typeEnum == _detonateHandle) || (typeEnum == _fireHandle)) {

         Handle munitionHandle (0);

         UInt32 munitionSite (0);
         UInt32 munitionHost (0);
         UInt32 munitionEntity (0);

         if (_eventMod->lookup_object_handle (
               EventHandle,
               _munitionHandle,
               munitionHandle)) {

            _idMod->lookup_site_host_entity (
               munitionHandle,
               munitionSite,
               munitionHost,
               munitionEntity);
         }

         data.set_next_uint32 (munitionSite);
         data.set_next_uint32 (munitionHost);
         data.set_next_uint32 (munitionEntity);

         ObjectType munitionType;
         ArrayUInt32 array;

         if (_eventMod->lookup_object_type (EventHandle, _munitionHandle, munitionType)) {

            _attrMod->to_net_type (munitionType, array);
         }

         data.set_next_uint32 (array.get (0));
         data.set_next_uint32 (array.get (1));
         data.set_next_uint32 (array.get (2));
      }

      Vector pos;

      _eventMod->lookup_position (EventHandle, _defaultHandle, pos);

      data.set_next_vector (pos);

      Vector offset;

      _eventMod->lookup_vector (EventHandle, _targetHandle, pos);

      data.set_next_vector (offset);

      result = True;
   }

   return result;
}


void
dmz::NetExtPacketCodecEventBasic::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   Definitions defs (context, &_log);

   _defaultHandle = defs.create_named_handle (EventAttributeDefaultName);
   _sourceHandle = defs.create_named_handle (EventAttributeSourceName);
   _targetHandle = defs.create_named_handle (EventAttributeTargetName);
   _munitionHandle = defs.create_named_handle (EventAttributeMunitionName);

   _fireType.set_type (
      config_to_string ("events.fire.name", local, "Fire_Event"),
      context);

   _detonateType.set_type (
      config_to_string ("events.detonate.name", local, "Detonation_Event"),
      context);

   _collisionType.set_type (
      config_to_string ("events.collision.name", local, "Collision_Event"),
      context);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzNetExtPacketCodecEventBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::NetExtPacketCodecEventBasic (Info, local);
}

};
