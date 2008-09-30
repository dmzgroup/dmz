#include <dmzEventConsts.h>
#include <dmzEventModule.h>
#include "dmzNetExtPacketCodecEventBasic.h"
#include <dmzNetModuleAttributeMap.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeUUID.h>
#include <dmzSystemMarshal.h>
#include <dmzSystemUnmarshal.h>
#include <dmzTypesMask.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesUUID.h>
#include <dmzTypesVector.h>

/*!

\class dmz::NetExtPacketCodecEventBasic
\ingroup Net
\brief Basic event network codec.

*/

//! \cond
dmz::NetExtPacketCodecEventBasic::NetExtPacketCodecEventBasic (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      NetExtPacketCodecEvent (Info),
      _SysID (get_runtime_uuid (Info)),
      _log (Info),
      _time (Info.get_context ()),
      _defaultHandle (0),
      _sourceHandle (0),
      _targetHandle (0),
      _munitionsHandle (0),
      _launchHandle (1),
      _detonateHandle (2),
      _collisionHandle (3),
      _eventMod (0),
      _attrMod (0),
      _objMod (0) {

   _init (local);
}


dmz::NetExtPacketCodecEventBasic::~NetExtPacketCodecEventBasic () {

}


// Plugin Interface
void
dmz::NetExtPacketCodecEventBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_eventMod) {

         _eventMod = EventModule::cast (PluginPtr);
      }

      if (!_attrMod) { _attrMod = NetModuleAttributeMap::cast (PluginPtr); }
      if (!_objMod) { _objMod = ObjectModule::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_eventMod && (_eventMod = EventModule::cast (PluginPtr))) {

         _eventMod = 0;
      }

      if (_attrMod && (_attrMod == NetModuleAttributeMap::cast (PluginPtr))) {

         _attrMod = 0;
      }

      if (_objMod && (_objMod == ObjectModule::cast (PluginPtr))) { _objMod = 0; }
   }
}


// NetExtPacketCodecEvent Interface
dmz::Boolean
dmz::NetExtPacketCodecEventBasic::decode (Unmarshal &data, Boolean &isLoopback) {

   Boolean result (False);

   if (_objMod && _attrMod && _eventMod) {

      UUID sysID;
      data.get_next_uuid (sysID);

      if (_SysID == sysID) {

         isLoopback = True;
      }
      else {

         const UInt32 TypeEnum (data.get_next_uint32 ());

         UUID sourceID;
         data.get_next_uuid (sourceID);

         EventType type;
         ObjectType munitionType;
         Handle sourceHandle (0), targetHandle (0), munitionsHandle (0);

         sourceHandle = _objMod->lookup_handle_from_uuid (sourceID);

         if (TypeEnum == _launchHandle) { type = _launchType; }
         else if (TypeEnum == _detonateHandle) { type = _detonateType; }
         else if (TypeEnum == _collisionHandle) { type = _collisionType; }

         UUID targetID;
         data.get_next_uuid (targetID);
         targetHandle = _objMod->lookup_handle_from_uuid (targetID);

         if ((TypeEnum == _launchHandle) || (TypeEnum == _detonateHandle)) {

            UUID munitionsID;
            data.get_next_uuid (munitionsID);

            munitionsHandle = _objMod->lookup_handle_from_uuid (munitionsID);

            ArrayUInt32 typeArray;

            typeArray.set (0, data.get_next_uint32 ());
            typeArray.set (1, data.get_next_uint32 ());
            typeArray.set (2, data.get_next_uint32 ());

            _attrMod->to_internal_object_type (typeArray, munitionType);
         }

         Vector pos, vel, offset;

         data.get_next_vector (pos);
         data.get_next_vector (vel);
         data.get_next_vector (offset);

         const Handle EventHandle (_eventMod->create_event (type, EventRemote));

         if (EventHandle) {

            _eventMod->store_object_handle (EventHandle, _sourceHandle, sourceHandle);
            _eventMod->store_object_handle (EventHandle, _targetHandle, targetHandle);

            if ((TypeEnum == _launchHandle) || (TypeEnum == _detonateHandle)) {

               _eventMod->store_object_handle (
                  EventHandle,
                  _munitionsHandle,
                  munitionsHandle);

               _eventMod->store_object_type (
                  EventHandle,
                  _munitionsHandle,
                  munitionType);
            }

            _eventMod->store_position (EventHandle, _defaultHandle, pos);
            _eventMod->store_velocity (EventHandle, _defaultHandle, vel);
            _eventMod->store_vector (EventHandle, _targetHandle, offset);

            result = _eventMod->close_event (EventHandle);
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

   if (_objMod && _attrMod && _eventMod) {

      data.set_next_uuid (_SysID);

      UInt32 typeEnum (0);
      EventType type;
      _eventMod->lookup_event_type (EventHandle, type);

      if (type.is_of_type (_detonateType)) { typeEnum = _detonateHandle; }
      else if (type.is_of_type (_launchType)) { typeEnum = _launchHandle; }
      else if (type.is_of_type (_collisionType)) { typeEnum = _collisionHandle; }

      data.set_next_uint32 (typeEnum);

      Handle sourceHandle (0);

      UUID sourceID, targetID, munitionsID;

      if (_eventMod->lookup_object_handle (EventHandle, _sourceHandle, sourceHandle)) {

         _objMod->lookup_uuid (sourceHandle, sourceID);
      }

      data.set_next_uuid (sourceID);

      Handle targetHandle (0);

      if (_eventMod->lookup_object_handle (EventHandle, _targetHandle, targetHandle)) {

         _objMod->lookup_uuid (targetHandle, targetID);
      }

      data.set_next_uuid (targetID);

      if ((typeEnum == _detonateHandle) || (typeEnum == _launchHandle)) {

         Handle munitionsHandle (0);

         if (_eventMod->lookup_object_handle (
               EventHandle,
               _munitionsHandle,
               munitionsHandle)) {

            _objMod->lookup_uuid (munitionsHandle, munitionsID);
         }

         data.set_next_uuid (munitionsID);

         ObjectType munitionType;
         ArrayUInt32 array;

         if (_eventMod->lookup_object_type (
               EventHandle,
               _munitionsHandle,
               munitionType)) {

            _attrMod->to_net_object_type (munitionType, array);
         }

         data.set_next_uint32 (array.get (0));
         data.set_next_uint32 (array.get (1));
         data.set_next_uint32 (array.get (2));
      }

      Vector pos;
      _eventMod->lookup_position (EventHandle, _defaultHandle, pos);
      data.set_next_vector (pos);

      Vector vel;
      _eventMod->lookup_velocity (EventHandle, _defaultHandle, vel);
      data.set_next_vector (vel);

      Vector offset;
      _eventMod->lookup_vector (EventHandle, _targetHandle, offset);
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
   _munitionsHandle = defs.create_named_handle (EventAttributeMunitionsName);

   _launchType.set_type (
      config_to_string ("events.launch.name", local, EventLaunchName),
      context);

   _detonateType.set_type (
      config_to_string ("events.detonate.name", local, EventDetonationName),
      context);

   _collisionType.set_type (
      config_to_string ("events.collision.name", local, EventCollisionName),
      context);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzNetExtPacketCodecEventBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::NetExtPacketCodecEventBasic (Info, local);
}

};
