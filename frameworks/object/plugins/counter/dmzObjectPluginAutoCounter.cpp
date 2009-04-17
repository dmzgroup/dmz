#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include "dmzObjectPluginAutoCounter.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::ObjectPluginAutoCounter
\ingroup Object
\brief Automatically increments/decrements a counter at a given rate
\details
\code
<dmz>
<dmzObjectPluginAutoCounter>
   <counter
      attribute="Counter Attribute Name"
      rate="Rate of increase"
      increase="Size of increase"
   />
</dmzObjectPluginAutoCounter>
</dmz>
\endcode

*/

//! \cond
dmz::ObjectPluginAutoCounter::ObjectPluginAutoCounter (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _rate (1.0),
      _increase (1),
      _counterAttrHandle (0) {

   _init (local);
}


dmz::ObjectPluginAutoCounter::~ObjectPluginAutoCounter () {

   _objTable.empty ();
}


// Plugin Interface
void
dmz::ObjectPluginAutoCounter::update_plugin_state (
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
dmz::ObjectPluginAutoCounter::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// TimeSlice Interface
void
dmz::ObjectPluginAutoCounter::update_time_slice (const Float64 TimeDelta) {

   ObjectModule *objMod (get_object_module ());

   if (_counterAttrHandle && objMod) {

      HashTableHandleIterator it;
      CounterStruct *current (0);

      while (_objTable.get_next (it, current)) {

         current->timer += TimeDelta;

         if (current->timer >= _rate) {

            current->timer = current->timer - _rate;

            objMod->add_to_counter (current->ObjectHandle, _counterAttrHandle, _increase);
         }
      }
   }
}


// Object Observer Interface
void
dmz::ObjectPluginAutoCounter::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   CounterStruct *cs = _objTable.remove (ObjectHandle);

   if (cs) { delete cs; cs = 0; }
}


void
dmz::ObjectPluginAutoCounter::update_object_counter (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   CounterStruct *cs = _objTable.lookup (ObjectHandle);

   if (!cs) {

      ObjectModule *objMod (get_object_module ());

      if (objMod) {

         if (objMod->lookup_locality (ObjectHandle) == ObjectLocal) {

            cs = new CounterStruct (ObjectHandle);

            if (!_objTable.store (ObjectHandle, cs)) { delete cs; cs = 0; }
         }
      }
   }
}


void
dmz::ObjectPluginAutoCounter::_init (Config &local) {

   activate_default_object_attribute (ObjectDestroyMask);

   _counterAttrHandle = activate_object_attribute (
      config_to_string ("counter.attribute", local),
      ObjectCounterMask);

   if (!_counterAttrHandle) {

      _log.error << "No attribute name specified for auto counter." << endl;
   }

   _rate = config_to_float64 ("counter.rate", local, _rate);
   _increase = config_to_int64 ("counter.increase", local, _increase);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzObjectPluginAutoCounter (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ObjectPluginAutoCounter (Info, local);
}

};
