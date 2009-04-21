#include <dmzEntityConsts.h>
#include "dmzEntityPluginRestoreHealth.h"
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::EntityPluginRestoreHealth
\ingroup Entity
\brief Restores the human-in-the-loop health when the object unsets the dead state.
\details
\code
<dmz>
<dmzEntityPluginRestoreHealth>
   <health value="Value to set restored health"/>
</dmzEntityPluginRestoreHealth>
health.value
</dmz>
\endcode

*/

//! \cond
dmz::EntityPluginRestoreHealth::EntityPluginRestoreHealth (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defaultAttrHandle (0),
      _healthAttrHandle (0),
      _health (100.0) {

   _init (local);
}


dmz::EntityPluginRestoreHealth::~EntityPluginRestoreHealth () {

}


// Plugin Interface
void
dmz::EntityPluginRestoreHealth::update_plugin_state (
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
dmz::EntityPluginRestoreHealth::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Object Observer Interface
void
dmz::EntityPluginRestoreHealth::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   ObjectModule *objMod (get_object_module ());

   if (objMod) {

      if (objMod->lookup_locality (ObjectHandle) == ObjectLocal) {

         const Boolean IsDead (Value.contains (_dead));
         const Boolean WasDead (PreviousValue ? PreviousValue->contains (_dead) : False);

         if (!IsDead && WasDead) {

            objMod->store_scalar (ObjectHandle, _healthAttrHandle, _health);
         }
      }
   }
}


void
dmz::EntityPluginRestoreHealth::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context ());

   defs.lookup_state (DefaultStateNameDead, _dead);

   _healthAttrHandle = defs.create_named_handle (EntityAttributeHealthName);

   _defaultAttrHandle = activate_default_object_attribute (ObjectStateMask);

   _health = config_to_float64 ("health.value", local, _health);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginRestoreHealth (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginRestoreHealth (Info, local);
}

};
