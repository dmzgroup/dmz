#include "dmzEntityPluginDamage.h"
#include <dmzEventCallbackMasks.h>
#include <dmzEventConsts.h>
#include <dmzEventModule.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::EntityPluginDamage
\ingroup Entity
\brief Determines damage for the human-in-the-loop object.
\details The current implementation currently uses a one-hit-one-kill model.
Hits are determined by detonation events and have the human-in-the-loop object
as the target object.

*/

//! \cond
dmz::EntityPluginDamage::EntityPluginDamage (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      EventObserverUtil (Info, local),
      _log (Info),
      _hil (0),
      _hilHandle (0),
      _defaultObjectHandle (0),
      _targetHandle (0) {

   _init (local);
}


dmz::EntityPluginDamage::~EntityPluginDamage () {

}


// Object Observer Interface
void
dmz::EntityPluginDamage::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   if (ObjectHandle == _hil) { _hil = 0; }
}


void
dmz::EntityPluginDamage::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (Value && (AttributeHandle == _hilHandle)) { _hil = ObjectHandle; }
}


// Event Observer Interface
void
dmz::EntityPluginDamage::close_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

   if (_hil && Type.is_of_type (_detonationType)) {

      EventModule *eventMod (get_event_module ());

      if (eventMod) {

         Handle target (0);

         if (eventMod->lookup_object_handle (EventHandle, _targetHandle, target)) {

            if (_hil == target) {

               ObjectModule *objMod (get_object_module ());

               if (objMod) {

                  Mask state;

                  objMod->lookup_state (_hil, _defaultObjectHandle, state);

                  state |= _deadState;

                  objMod->store_state (_hil, _defaultObjectHandle, state);
               }
            }
         }
      }
   }
}


void
dmz::EntityPluginDamage::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _targetHandle = defs.create_named_handle (EventAttributeTargetName);

   const String StateNames = config_to_string ("state.name", local, DefaultStateNameDead);

   defs.lookup_state (StateNames, _deadState);

   _defaultObjectHandle = activate_default_object_attribute (ObjectDestroyMask);

   _hilHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);

   _detonationType = activate_event_callback (EventDetonationName, EventCloseMask);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginDamage (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginDamage (Info, local);
}

};
