#include "dmzNetExtPacketCodecObjectBasic.h"
#include <dmzNetModuleAttributeMap.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimeUUID.h>
#include <dmzSystemMarshal.h>
#include <dmzSystemUnmarshal.h>
#include <dmzTypesMask.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesUUID.h>
#include <dmzTypesVector.h>

/*!

\class dmz::NetExtPacketCodecObjectBasic
\ingroup Net
\brief Basic object network codec.

*/

//! \cond
dmz::NetExtPacketCodecObjectBasic::NetExtPacketCodecObjectBasic (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      NetExtPacketCodecObject (Info),
      _SysID (get_runtime_uuid (Info)),
      _log (Info),
      _time (Info.get_context ()),
      _defaultHandle (0),
      _lnvHandle (0),
      _objMod (0),
      _attrMod (0) {

   _init (local);
}


dmz::NetExtPacketCodecObjectBasic::~NetExtPacketCodecObjectBasic () {

   _scalarTable.empty ();
}


// Plugin Interface
void
dmz::NetExtPacketCodecObjectBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_objMod) { _objMod = ObjectModule::cast (PluginPtr); }
      if (!_attrMod) { _attrMod = NetModuleAttributeMap::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_objMod && (_objMod == ObjectModule::cast (PluginPtr))) { _objMod = 0; }

      if (_attrMod && (_attrMod == NetModuleAttributeMap::cast (PluginPtr))) {

         _attrMod = 0;
      }
   }
}


// NetExtPacketCodecObject Interface
dmz::Boolean
dmz::NetExtPacketCodecObjectBasic::decode (Unmarshal &data, Boolean &isLoopback) {

   Boolean result (False);

   isLoopback = False;

   if (_attrMod && _objMod) {

      UUID uuid;
      data.get_next_uuid (uuid);

      if (_SysID == uuid) {

         isLoopback = True;
      }
      else {

         data.get_next_uuid (uuid);

         Vector pos;
         Matrix ori;
         Vector vel;
         ArrayUInt32 typeArray;
         ArrayUInt32 stateArray;

         data.get_next_vector (pos);
         data.get_next_matrix (ori);
         data.get_next_vector (vel);
         typeArray.set (0, data.get_next_uint32 ());
         typeArray.set (1, data.get_next_uint32 ());
         typeArray.set (2, data.get_next_uint32 ());
         stateArray.set (0, data.get_next_uint32 ());

         ObjectType type;

         _attrMod->to_internal_object_type (typeArray, type);

         Handle handle (_objMod->lookup_handle_from_uuid (uuid));

         Boolean activateObject (False);

         if (type && !handle) {

            handle = _objMod->create_object (type, ObjectRemote);

            if (handle) {

               _objMod->store_uuid (handle, uuid);
               activateObject = True;
            }
         }

         if (handle) {

            Mask state;
            _objMod->lookup_state (handle, _defaultHandle, state);
            _attrMod->to_internal_object_mask (type, stateArray, state);

            if (_deactivateState && state.contains (_deactivateState)) {

               _objMod->destroy_object (handle);
            }
            else {

               _objMod->store_position (handle, _defaultHandle, pos);
               _objMod->store_position (handle, _lnvHandle, pos);
               _objMod->store_orientation (handle, _defaultHandle, ori);
               _objMod->store_velocity (handle, _defaultHandle, vel);
               _objMod->store_state (handle, _defaultHandle, state);
               _objMod->store_time_stamp (handle, _lnvHandle, _time.get_frame_time ());

               const Handle ScalarCount (data.get_next_uint32 ());

               if (ScalarCount) {

                  for (UInt32 ix = 0; ix < ScalarCount; ix++) {

                     const Handle NetHandle = data.get_next_uint32 ();
                     const Float64 Value = data.get_next_float64 ();

                     ScalarStruct *ss = _scalarTable.lookup (NetHandle);

                     if (ss && ss->AttrHandle) {

                        _objMod->store_scalar (handle, ss->AttrHandle, Value);
                     }
                  }
               }

               if (activateObject) { _objMod->activate_object (handle); }
            }

            result = True;
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::NetExtPacketCodecObjectBasic::encode_object (
      const Handle ObjectHandle,
      const NetObjectEncodeEnum EncodeMode,
      Marshal &data) {

   Boolean result (False);

   if (_attrMod && _objMod) {

      UUID uuid;

      if (_objMod->lookup_uuid (ObjectHandle, uuid)) {

         const ObjectType Type (_objMod->lookup_object_type (ObjectHandle));
         ArrayUInt32 typeArray;

         if (_attrMod->to_net_object_type (Type, typeArray)) {

            Vector pos;
            Matrix ori;
            Vector vel;
            ArrayUInt32 stateArray;
            Mask state;

            _objMod->lookup_position (ObjectHandle, _defaultHandle, pos);
            _objMod->lookup_orientation (ObjectHandle, _defaultHandle, ori);
            _objMod->lookup_velocity (ObjectHandle, _defaultHandle, vel);
            _objMod->lookup_state (ObjectHandle, _defaultHandle, state);

            if (EncodeMode == NetObjectDeactivate) {

               state |= _deactivateState;
            }

            _attrMod->to_net_object_mask (Type, state, stateArray);

            data.set_next_uuid (_SysID);
            data.set_next_uuid (uuid);
            data.set_next_vector (pos);
            data.set_next_matrix (ori);
            data.set_next_vector (vel);
            data.set_next_uint32 (typeArray.get (0));
            data.set_next_uint32 (typeArray.get (1));
            data.set_next_uint32 (typeArray.get (2));
            data.set_next_uint32 (stateArray.get (0));

            const Int32 Place (data.get_place ());

            data.set_next_uint32 (0);

            UInt32 scalarCount (0);

            HashTableHandleIterator it;

            ScalarStruct *ss (_scalarTable.get_first (it));

            while (ss) {

               Float64 value (0.0);

               if (_objMod->lookup_scalar (ObjectHandle, ss->AttrHandle, value)) {

                  data.set_next_uint32 (ss->NetHandle);
                  data.set_next_float64 (value);
                  scalarCount++;

                  if (ss->LNVAttrHandle) {

                     _objMod->store_scalar (ObjectHandle, ss->LNVAttrHandle, value);
                  }
               }

               ss = _scalarTable.get_next (it);
            }

            const Int32 EndPlace (data.get_place ());
            data.set_place (Place);
            data.set_next_uint32 (scalarCount);
            data.set_place (EndPlace);

            _objMod->store_time_stamp (
               ObjectHandle,
               _lnvHandle,
               _time.get_last_frame_time ());

            _objMod->store_position (ObjectHandle, _lnvHandle, pos);
            _objMod->store_orientation (ObjectHandle, _lnvHandle, ori);
            _objMod->store_velocity (ObjectHandle, _lnvHandle, vel);

            if (EncodeMode != NetObjectDeactivate) {

               _objMod->store_state (ObjectHandle, _lnvHandle, state);
            }

            result = True;
         }
      }
   }

   return result;
}


void
dmz::NetExtPacketCodecObjectBasic::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _defaultHandle = defs.create_named_handle (ObjectAttributeDefaultName);
   _lnvHandle = defs.create_named_handle (ObjectAttributeLastNetworkValueName);

   defs.lookup_state (
      config_to_string ("deactivate.name", local, DefaultStateNameDeactivate),
      _deactivateState);

   Config scalarData;

   if (local.lookup_all_config ("scalar", scalarData)) {

      ConfigIterator it;

      Config data;

      Boolean found (scalarData.get_first_config (it, data));

      while (found) {

         const String Name (config_to_string ("name", data));

         const String LNVName (
            config_to_string (
               "lnvname",
                data,
                create_last_network_value_name (Name)));

         const UInt32 Value (config_to_uint32 ("value", data));

         if (Name) {

            ScalarStruct *ss (new ScalarStruct (
               Value,
               defs.create_named_handle (Name),
               defs.create_named_handle (LNVName)));

            if (ss && !_scalarTable.store (ss->NetHandle, ss)) {

               delete ss; ss = 0;
            }
         }
         else {

         }

         found = scalarData.get_next_config (it, data);
      }
   }
   else {

      ScalarStruct *ss (
         new ScalarStruct (
            1,
            defs.create_named_handle (ObjectAttributeScalarThrottleName),
            defs.create_named_handle (
               create_last_network_value_name (ObjectAttributeScalarThrottleName))));

      if (ss && !_scalarTable.store (ss->NetHandle, ss)) { delete ss; ss = 0; }
   }
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzNetExtPacketCodecObjectBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::NetExtPacketCodecObjectBasic (Info, local);
}

};
