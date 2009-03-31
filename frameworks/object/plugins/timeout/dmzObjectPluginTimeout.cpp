#include <dmzEventModuleCommon.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include "dmzObjectPluginTimeout.h"

/*!

\class dmz::ObjectPluginTimeout
\ingroup Object
\brief Destroys local objects of specified types after a defined interval.
\details The plugin uses the timeout field in the object-type definition. If the timeout
field is defined, all local object of that object type will be destroyed after the
specified time has elapsed. If the detonate parameter is set to true, the plugin
will create a detonation event before deleting the object.
\code
<dmz>
<runtime>
   <object-type name="object name">
      <timeout value="time is seconds" detonate="true/false"/>
   </object-type>
</runtime>
</dmz>
\endcode

*/

//! \cond
dmz::ObjectPluginTimeout::ObjectPluginTimeout (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _eventMod (0),
      _defaultTimeout (new TimeoutStruct (False, -1.0)) {

   _init (local);
}


dmz::ObjectPluginTimeout::~ObjectPluginTimeout () {

   _timeoutTable.clear ();
   if (_defaultTimeout) { delete _defaultTimeout; _defaultTimeout = 0; }
   _masterTimeoutTable.empty ();
}


// Plugin Interface
void
dmz::ObjectPluginTimeout::update_plugin_state (
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
dmz::ObjectPluginTimeout::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_eventMod) { _eventMod = EventModuleCommon::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_eventMod && (_eventMod == EventModuleCommon::cast (PluginPtr))) {

         _eventMod = 0; 
      }
   }
}


// TimeSlice Interface
void
dmz::ObjectPluginTimeout::update_time_slice (const Float64 DeltaTime) {

   HashTableHandleIterator it;
   TimeoutStruct *ptr (0);

   while (_objTable.get_next (it, ptr)) {

      ptr->timeout -= DeltaTime;

      if (ptr->timeout < 0.0) {

         const Handle Object (it.get_hash_key ());

         ObjectModule *objMod = get_object_module ();

         if (objMod) {
     
            if (ptr->Detonate && _eventMod) {

               _eventMod->create_detonation_event (Object, 0);
            }

            objMod->destroy_object (Object);
         }
      }
   }
}


// Object Observer Interface
void
dmz::ObjectPluginTimeout::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (Locality == ObjectLocal) {

      TimeoutStruct *ts = _find_timeout (Type);

      if (ts && (ts->timeout > 0.0)) {

         TimeoutStruct *ptr = new TimeoutStruct (*ts);

         if (ptr && !_objTable.store (ObjectHandle, ptr)) { delete ptr; ptr = 0; }
      }
   }
}


void
dmz::ObjectPluginTimeout::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   TimeoutStruct *ptr = _objTable.remove (ObjectHandle);

   if (ptr) { delete ptr; ptr = 0; }
}


dmz::ObjectPluginTimeout::TimeoutStruct *
dmz::ObjectPluginTimeout::_find_timeout (const ObjectType &Type) {

   ObjectType current (Type);

   TimeoutStruct *result = _timeoutTable.lookup (current.get_handle ());

   while (!result && current) {

      const Float64 Timeout =
         config_to_float64 ("timeout.value", current.get_config (), -1.0);

      const Boolean Detonate =
         config_to_boolean ("timeout.detonate", current.get_config (), False);

      if (Timeout > 0.0) {

         result = new TimeoutStruct (Detonate, Timeout);

         if (result && _masterTimeoutTable.store (current.get_handle (), result)) {

            _timeoutTable.store (Type.get_handle (), result);
         }
         else if (result) { delete result; result = 0; }

         current.set_type_context (0);
      }
      else {

         current.become_parent ();

         if (current) {

            result = _timeoutTable.lookup (current.get_handle ());

            if (result) { _timeoutTable.store (Type.get_handle (), result); }
         }
      }
   }

   if (!result) {

      _timeoutTable.store (Type.get_handle (), _defaultTimeout);
   }

   return result;
}

void
dmz::ObjectPluginTimeout::_init (Config &local) {

   activate_default_object_attribute (ObjectCreateMask | ObjectDestroyMask);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzObjectPluginTimeout (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ObjectPluginTimeout (Info, local);
}

};
