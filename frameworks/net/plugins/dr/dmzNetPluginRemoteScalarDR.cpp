#include "dmzNetPluginRemoteScalarDR.h"
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::NetPluginRemoteScalarDR
\ingroup Network
\brief Performs linear dead-reckoning on remote object's scalar attributes.
\details
\code
<dmz>
<dmzNetPluginRemoteScalarDR>
   <attribute
      name="Scalar Attribute Name"
      rate="Rate Attribute Name"
      lnv-name="[Optional]Last Network Value of Scalar Attribute Name"
      time-stamp="[Optional]Time Stamp Attribute Name"
   />
</dmzNetPluginRemoteScalarDR>
</dmz>
\endcode

*/

//! \cond
dmz::NetPluginRemoteScalarDR::NetPluginRemoteScalarDR (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _time (Info) {

   _init (local);
}


dmz::NetPluginRemoteScalarDR::~NetPluginRemoteScalarDR () {

   _objTable.empty ();
   _rateTable.empty ();
}


// Plugin Interface
void
dmz::NetPluginRemoteScalarDR::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::NetPluginRemoteScalarDR::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// TimeSlice Interface
void
dmz::NetPluginRemoteScalarDR::update_time_slice (const Float64 TimeDelta) {

   ObjectModule *objMod = get_object_module ();

   if (objMod) {

      const Float64 CurrentTime = _time.get_frame_time ();

      HashTableHandleIterator it;
      ObjectStruct *os (0);

      while (_objTable.get_next (it, os)) {

         HashTableHandleIterator valueIt;
         ValueStruct *vs (0);

         while (os->table.get_next (valueIt, vs)) {

            Float64 stamp (0.0);
            Float64 lnv (0.0);

            objMod->lookup_time_stamp (os->ObjectHandle, vs->rs.TimeStampHandle, stamp);
            objMod->lookup_scalar (os->ObjectHandle, vs->rs.LNVAttrHandle, lnv);

            objMod->store_scalar (
               os->ObjectHandle,
               vs->rs.AttrHandle,
               lnv + (vs->rate * (CurrentTime - stamp)));
         }
      }
   }
}


// Object Observer Interface
void
dmz::NetPluginRemoteScalarDR::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   ObjectStruct *os = _objTable.remove (ObjectHandle);

   if (os) { delete os; os = 0; }
   else { _ignoreObjects.remove_handle (ObjectHandle); }
}


void
dmz::NetPluginRemoteScalarDR::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   if (!_ignoreObjects.contains (ObjectHandle)) {

      ObjectModule *objMod = get_object_module ();

      if (objMod) {

         if (objMod->lookup_locality (ObjectHandle) != ObjectRemote) {

            _ignoreObjects.add_handle (ObjectHandle);
         }
         else {

            ObjectStruct *os = _objTable.lookup (ObjectHandle);

            if (!os) {

               os = new ObjectStruct (ObjectHandle);

               if (os && !_objTable.store (os->ObjectHandle, os)) { delete os; os = 0; }
            }

            if (os) {

               ValueStruct *vs = os->table.lookup (AttributeHandle);

               if (!vs) {

                  RateStruct *rs = _rateTable.lookup (AttributeHandle);

                  if (rs) {

                     vs = new ValueStruct (*rs);

                     if (vs && !os->table.store (AttributeHandle, vs)) {

                        delete vs; vs = 0;
                     }
                  }
               }

               if (vs) { vs->rate = Value; }
            }
         }
      }
   }
}


void
dmz::NetPluginRemoteScalarDR::_init (Config &local) {

   Config attrList;

   if (local.lookup_all_config ("attribute", attrList)) {

      Definitions defs (get_plugin_runtime_context ());

      ConfigIterator it;
      Config attr;

      while (attrList.get_next_config (it, attr)) {

         const String Name = config_to_string ("name", attr);
         const String Rate = config_to_string ("rate", attr);

         const String LNVName =
            config_to_string ("lnv-name", attr, create_last_network_value_name (Name));

         const String Timestamp =
            config_to_string ("time-stamp", attr, ObjectAttributeLastNetworkValueName);

         RateStruct *rs = new RateStruct (
            defs.create_named_handle (Rate),
            defs.create_named_handle (Name),
            defs.create_named_handle (LNVName),
            defs.create_named_handle (Timestamp));

         if (rs && _rateTable.store (rs->RateHandle, rs)) {

            activate_object_attribute (rs->RateHandle, ObjectScalarMask);
         }
         else if (rs) { delete rs; rs = 0; }
      }
   }
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzNetPluginRemoteScalarDR (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::NetPluginRemoteScalarDR (Info, local);
}

};
