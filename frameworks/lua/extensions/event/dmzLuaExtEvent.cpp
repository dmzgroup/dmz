#include "dmzLuaExtEvent.h"
#include "dmzLuaExtEventObserver.h"
#include <dmzLuaKernelValidate.h>
#include <dmzEventCallbackMasks.h>
#include <dmzEventConsts.h>
#include <dmzEventModule.h>
#include <dmzEventModuleCommon.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesUUID.h>
#include <dmzTypesVector.h>

#include <luacpp.h>

/*!

\class dmz::LuaExtEvent
\ingroup Lua
\brief Provides a Lua API the Event Framework.
\details
\htmlonly See the <a href="dmzlua.html#dmz.event">Lua Event API</a>.
\endhtmlonly

*/

using namespace dmz;

namespace {

typedef LuaExtEvent::EventStruct estruct;

static const char EventKey = 'e';
static const char EventNamespace[] = "event";


static inline EventModuleCommon *
get_common_event_module (lua_State *L) {

  EventModuleCommon *result (0);
  lua_pushlightuserdata (L, (void *)&EventKey);
  lua_rawget (L, LUA_REGISTRYINDEX);
  estruct **es = (estruct **)lua_touserdata (L, -1);
  if (es && *es) { result = (*es)->commonEventMod; }
  lua_pop (L, 1); // pop estruct
  return result;
}


static inline EventModule *
get_event_module (lua_State *L) {

  EventModule *result (0);
  lua_pushlightuserdata (L, (void *)&EventKey);
  lua_rawget (L, LUA_REGISTRYINDEX);
  estruct **es = (estruct **)lua_touserdata (L, -1);
  if (es && *es) { result = (*es)->eventMod; }
  lua_pop (L, 1); // pop estruct
  return result;
}


static inline estruct *
get_event_struct (lua_State *L) {

  estruct *result (0);
  lua_pushlightuserdata (L, (void *)&EventKey);
  lua_rawget (L, LUA_REGISTRYINDEX);
  estruct **es = (estruct **)lua_touserdata (L, -1);
  if (es) { result = *es; }
  lua_pop (L, 1); // pop estruct
  return result;
}


static EventModule *
get_event_params (lua_State *L, Handle &eventHandle, Handle &attrHandle) {

   estruct *es (get_event_struct (L));

   Handle *ptr (lua_check_handle (L, 1));
   eventHandle = (ptr ? *ptr : 0);

   if (es) {

      attrHandle = es->defaultHandle;

      if (!lua_isnoneornil (L, 2)) {

         ptr = lua_check_handle (L, 2);
         attrHandle = (ptr ? *ptr : 0);
      }
   }

   return es ? es->eventMod : 0;
}


static int
event_launch (lua_State *L) {

   int result (0);

   EventModuleCommon *eventMod (get_common_event_module (L));

   const Handle *Source = lua_check_handle (L, 1);
   const Handle *Target (0);
   if (!lua_isnoneornil (L, 2)) { Target = lua_check_handle (L, 2); }

   if (eventMod) {

      const Handle EventHandle = eventMod->create_launch_event (
         (Source ? *Source : 0),
         (Target ? *Target : 0)); 

      if (EventHandle) { lua_create_handle (L, EventHandle); result = 1; }
   }

   return result;
}


static int
event_detonation (lua_State *L) {

   int result (0);

   EventModuleCommon *eventMod (get_common_event_module (L));

   const Handle *Source = lua_check_handle (L, 1);
   const Handle *Target (0);
   if (!lua_isnoneornil (L, 2)) { Target = lua_check_handle (L, 2); }

   if (eventMod) {

      const Handle EventHandle = eventMod->create_detonation_event (
         (Source ? *Source : 0),
         (Target ? *Target : 0)); 

      if (EventHandle) { lua_create_handle (L, EventHandle); result = 1; }
   }

   return result;
}


static int
event_collision (lua_State *L) {

   int result (0);

   EventModuleCommon *eventMod (get_common_event_module (L));

   const Handle *Source = lua_check_handle (L, 1);
   const Handle *Target (0);
   if (!lua_isnoneornil (L, 2)) { Target = lua_check_handle (L, 2); }

   if (eventMod) {

      const Handle EventHandle = eventMod->create_collision_event (
         (Source ? *Source : 0),
         (Target ? *Target : 0)); 

      if (EventHandle) { lua_create_handle (L, EventHandle); result = 1; }
   }

   return result;
}


static int
event_open_launch (lua_State *L) {

   int result (0);

   EventModuleCommon *eventMod (get_common_event_module (L));

   const Handle *Source = lua_check_handle (L, 1);
   const Handle *Target (0);
   if (!lua_isnoneornil (L, 2)) { Target = lua_check_handle (L, 2); }

   if (eventMod) {

      const Handle EventHandle = eventMod->create_open_launch_event (
         (Source ? *Source : 0),
         (Target ? *Target : 0)); 

      if (EventHandle) { lua_create_handle (L, EventHandle); result = 1; }
   }

   return result;
}


static int
event_open_detonation (lua_State *L) {

   int result (0);

   EventModuleCommon *eventMod (get_common_event_module (L));

   const Handle *Source = lua_check_handle (L, 1);
   const Handle *Target (0);
   if (!lua_isnoneornil (L, 2)) { Target = lua_check_handle (L, 2); }

   if (eventMod) {

      const Handle EventHandle = eventMod->create_open_detonation_event (
         (Source ? *Source : 0),
         (Target ? *Target : 0)); 

      if (EventHandle) { lua_create_handle (L, EventHandle); result = 1; }
   }

   return result;
}


static int
event_open_collision (lua_State *L) {

   int result (0);

   EventModuleCommon *eventMod (get_common_event_module (L));

   const Handle *Source = lua_check_handle (L, 1);
   const Handle *Target (0);
   if (!lua_isnoneornil (L, 2)) { Target = lua_check_handle (L, 2); }

   if (eventMod) {

      const Handle EventHandle = eventMod->create_open_collision_event (
         (Source ? *Source : 0),
         (Target ? *Target : 0)); 

      if (EventHandle) { lua_create_handle (L, EventHandle); result = 1; }
   }

   return result;
}


static int
event_create (lua_State *L) {

   int result (0);

   estruct *es (get_event_struct (L));
   EventType *typePtr (lua_check_event_type (L, 1));

   if (es && es->eventMod && typePtr) {

      EventLocalityEnum locality (EventLocalityUnknown);

      int localityLua (es->localityLocal);

      if (!lua_isnone (L, 2)) { localityLua = luaL_checkinteger (L, 2); }

      if (localityLua == es->localityLocal) { locality = EventLocal; }
      else if (localityLua == es->localityRemote) { locality = EventRemote; }
      else { lua_enum_error (L, 2, localityLua); }

      Handle value = es->eventMod->create_event (*typePtr, locality);

      if (lua_create_handle (L, value)) { result = 1; }
   }

   return result;
}


static int
event_close (lua_State *L) {

   int result (0);

   EventModule *eventMod (get_event_module (L));
   Handle *ptr (lua_check_handle (L, 1));

   if (eventMod && ptr) {

      lua_pushboolean (L, eventMod->close_event (*ptr) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
event_type (lua_State *L) {

   int result (0);

   Handle eventHandle (0);
   EventModule *eventMod (get_event_module (L));
   Handle *ptr (lua_check_handle (L, 1));

   if (eventMod && ptr) {

      EventType type;

      if (eventMod->lookup_event_type (eventHandle, type)) {

         if (lua_create_event_type (L, &type)) { result = 1; }
      }
   }

   return result;
}


static int
event_locality (lua_State *L) {

   int result (0);

   estruct *es (get_event_struct (L));
   Handle *ptr = lua_check_handle (L, 1);
   Boolean setLocality (False);
   int locality (0);

   if (es && es->eventMod && ptr) {

      Boolean foundLocality (True);
      const EventLocalityEnum Value (es->eventMod->lookup_locality (*ptr));

      if (Value == EventLocal) { locality = es->localityLocal; }
      else if (Value == EventRemote) { locality = es->localityRemote; }
      else { foundLocality = False; }

      if (foundLocality) { lua_pushinteger (L, locality); }
      else { lua_pushnil (L); }

      result = 1;
   }

   return result;
}


static int
event_object_handle (lua_State *L) {

   int result (0);

   Handle eventHandle (0);
   Handle attrHandle (0);
   EventModule *eventMod (get_event_params (L, eventHandle, attrHandle));

   if (eventMod && eventHandle && attrHandle) {

      Handle *ptr (0);
      if (!lua_isnone (L, 3)) { ptr = lua_check_handle (L, 3); }

      if (ptr) {

         if (eventMod->store_object_handle (eventHandle, attrHandle, *ptr)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         Handle value (0);

         if (eventMod->lookup_object_handle (eventHandle, attrHandle, value)) {

            if (lua_create_handle (L, value)) { result = 1; }
         }
      }
   }

   return result;
}


static int
event_object_type (lua_State *L) {

   int result (0);

   Handle eventHandle (0);
   Handle attrHandle (0);
   EventModule *eventMod (get_event_params (L, eventHandle, attrHandle));

   if (eventMod && eventHandle && attrHandle) {

      ObjectType *ptr (0);
      if (!lua_isnone (L, 3)) { ptr = lua_check_object_type (L, 3); }

      if (ptr) {

         if (eventMod->store_object_type (eventHandle, attrHandle, *ptr)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         ObjectType value;

         if (eventMod->lookup_object_type (eventHandle, attrHandle, value)) {

            if (lua_create_object_type (L, &value)) { result = 1; }
         }
      }
   }

   return result;
}


static int
event_state (lua_State *L) {

   int result (0);

   Handle eventHandle (0);
   Handle attrHandle (0);
   EventModule *eventMod (get_event_params (L, eventHandle, attrHandle));

   if (eventMod && eventHandle && attrHandle) {

      Boolean storeMask = False;

      Mask value;

      if (!lua_isnone (L, 3)) {

         storeMask = True;

         if (lua_isstring (L, 3)) {

            Definitions defs (lua_get_runtime_context (L));

            const String Name (lua_tostring (L, 3));

            defs.lookup_state (Name, value);
         }
         else {

            Mask *ptr = lua_check_mask (L, 3);

            if (ptr) { value = *ptr; }
         }
      }

      if (storeMask) {

         if (eventMod->store_state (eventHandle, attrHandle, value)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         if (eventMod->lookup_state (eventHandle, attrHandle, value)) {

            if (lua_create_mask (L, &value)) { result = 1; }
         }
      }
   }

   return result;
}


static int
event_time_stamp (lua_State *L) {

   int result (0);

   Handle eventHandle (0);
   Handle attrHandle (0);
   EventModule *eventMod (get_event_params (L, eventHandle, attrHandle));

   if (eventMod && eventHandle && attrHandle) {

      Boolean store (False);
      Float64 value (0.0);
      if (!lua_isnone (L, 3)) { value = luaL_checknumber (L, 3); store = True; }

      if (store) {

         if (eventMod->store_time_stamp (eventHandle, attrHandle, value)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         if (eventMod->lookup_time_stamp (eventHandle, attrHandle, value)) {

            lua_pushnumber (L, value);
            result = 1;
         }
      }
   }

   return result;
}


static int
event_position (lua_State *L) {

   int result (0);

   Handle eventHandle (0);
   Handle attrHandle (0);
   EventModule *eventMod (get_event_params (L, eventHandle, attrHandle));

   if (eventMod && eventHandle && attrHandle) {

      Vector *ptr (0);
      if (!lua_isnone (L, 3)) { ptr = lua_check_vector (L, 3); }

      if (ptr) {

         if (eventMod->store_position (eventHandle, attrHandle, *ptr)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         Vector value;

         if (eventMod->lookup_position (eventHandle, attrHandle, value)) {

            if (lua_create_vector (L, &value)) { result = 1; }
         }
      }
   }

   return result;
}


static int
event_orientation (lua_State *L) {

   int result (0);

   Handle eventHandle (0);
   Handle attrHandle (0);
   EventModule *eventMod (get_event_params (L, eventHandle, attrHandle));

   if (eventMod && eventHandle && attrHandle) {

      Matrix *ptr (0);
      if (!lua_isnone (L, 3)) { ptr = lua_check_matrix (L, 3); }

      if (ptr) {

         if (eventMod->store_orientation (eventHandle, attrHandle, *ptr)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         Matrix value;

         if (eventMod->lookup_orientation (eventHandle, attrHandle, value)) {

            if (lua_create_matrix (L, &value)) { result = 1; }
         }
      }
   }

   return result;
}


static int
event_velocity (lua_State *L) {

   int result (0);

   Handle eventHandle (0);
   Handle attrHandle (0);
   EventModule *eventMod (get_event_params (L, eventHandle, attrHandle));

   if (eventMod && eventHandle && attrHandle) {

      Vector *ptr (0);
      if (!lua_isnone (L, 3)) { ptr = lua_check_vector (L, 3); }

      if (ptr) {

         if (eventMod->store_velocity (eventHandle, attrHandle, *ptr)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         Vector value;

         if (eventMod->lookup_velocity (eventHandle, attrHandle, value)) {

            if (lua_create_vector (L, &value)) { result = 1; }
         }
      }
   }

   return result;
}


static int
event_acceleration (lua_State *L) {

   int result (0);

   Handle eventHandle (0);
   Handle attrHandle (0);
   EventModule *eventMod (get_event_params (L, eventHandle, attrHandle));

   if (eventMod && eventHandle && attrHandle) {

      Vector *ptr (0);
      if (!lua_isnone (L, 3)) { ptr = lua_check_vector (L, 3); }

      if (ptr) {

         if (eventMod->store_acceleration (eventHandle, attrHandle, *ptr)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         Vector value;

         if (eventMod->lookup_acceleration (eventHandle, attrHandle, value)) {

            if (lua_create_vector (L, &value)) { result = 1; }
         }
      }
   }

   return result;
}


static int
event_scale (lua_State *L) {

   int result (0);

   Handle eventHandle (0);
   Handle attrHandle (0);
   EventModule *eventMod (get_event_params (L, eventHandle, attrHandle));

   if (eventMod && eventHandle && attrHandle) {

      Vector *ptr (0);
      if (!lua_isnone (L, 3)) { ptr = lua_check_vector (L, 3); }

      if (ptr) {

         if (eventMod->store_scale (eventHandle, attrHandle, *ptr)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         Vector value;

         if (eventMod->lookup_scale (eventHandle, attrHandle, value)) {

            if (lua_create_vector (L, &value)) { result = 1; }
         }
      }
   }

   return result;
}


static int
event_vector (lua_State *L) {

   int result (0);

   Handle eventHandle (0);
   Handle attrHandle (0);
   EventModule *eventMod (get_event_params (L, eventHandle, attrHandle));

   if (eventMod && eventHandle && attrHandle) {

      Vector *ptr (0);
      if (!lua_isnone (L, 3)) { ptr = lua_check_vector (L, 3); }

      if (ptr) {

         if (eventMod->store_vector (eventHandle, attrHandle, *ptr)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         Vector value;

         if (eventMod->lookup_vector (eventHandle, attrHandle, value)) {

            if (lua_create_vector (L, &value)) { result = 1; }
         }
      }
   }

   return result;
}


static int
event_scalar (lua_State *L) {

   int result (0);

   Handle eventHandle (0);
   Handle attrHandle (0);
   EventModule *eventMod (get_event_params (L, eventHandle, attrHandle));

   if (eventMod && eventHandle && attrHandle) {

      Boolean store (False);
      Float64 value (0.0);
      if (!lua_isnone (L, 3)) { value = luaL_checknumber (L, 3); store = True; }

      if (store) {

         if (eventMod->store_scalar (eventHandle, attrHandle, value)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         if (eventMod->lookup_scalar (eventHandle, attrHandle, value)) {

            lua_pushnumber (L, value);
            result = 1;
         }
      }
   }


   return result;
}


static int
event_text (lua_State *L) {

   int result (0);

   Handle eventHandle (0);
   Handle attrHandle (0);
   EventModule *eventMod (get_event_params (L, eventHandle, attrHandle));

   if (eventMod && eventHandle && attrHandle) {

      const char *ptr (0);
      if (!lua_isnone (L, 3)) { ptr = luaL_checkstring (L, 3); }

      if (ptr) {

         if (eventMod->store_text (eventHandle, attrHandle, ptr)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         String value;

         if (eventMod->lookup_text (eventHandle, attrHandle, value)) {

            lua_pushstring (L, value.get_buffer ());
            result = 1;
         }
      }
   }

   return result;
}


static int
event_data (lua_State *L) {

   int result (0);

   Handle eventHandle (0);
   Handle attrHandle (0);
   EventModule *eventMod (get_event_params (L, eventHandle, attrHandle));

   if (eventMod && eventHandle && attrHandle) {

      Data *ptr (0);
      if (!lua_isnone (L, 3)) { ptr = lua_check_data (L, 3); }

      if (ptr) {

         if (eventMod->store_data (eventHandle, attrHandle, *ptr)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         Data value;

         if (eventMod->lookup_data (eventHandle, attrHandle, value)) {

            if (lua_create_data (L, &value)) { result = 1; }
         }
      }
   }

   return result;
}


static const luaL_Reg arrayFunc[] = {
   {"launch", event_launch},
   {"detonation", event_detonation},
   {"collision", event_collision},
   {"open_launch", event_open_launch},
   {"open_detonation", event_open_detonation},
   {"open_collision", event_open_collision},
   {"create", event_create},
   {"close", event_close},
   {"type", event_type},
   {"locality", event_locality},
   {"object_handle", event_object_handle},
   {"object_type", event_object_type},
   {"state", event_state},
   {"time_stamp", event_time_stamp},
   {"position", event_position},
   {"orientation", event_orientation},
   {"velocity", event_velocity},
   {"acceleration", event_acceleration},
   {"scale", event_scale},
   {"vector", event_vector},
   {"scalar", event_scalar},
   {"text", event_text},
   {"data", event_data},
   {NULL, NULL},
};

};


//! \cond
void
dmz::lua_register_event_observer (lua_State *L, EventObserver &obs) {

   estruct *es (get_event_struct (L));

   if (es && es->ext) { es->ext->register_event_observer (obs); }
}


void
dmz::lua_release_event_observer (lua_State *L, EventObserver &obs) {

   estruct *es = get_event_struct (L);

   if (es && es->ext) { es->ext->release_event_observer (obs); }
}


dmz::Handle
dmz::lua_get_default_event_handle (lua_State *L) {

   Handle result (0);

   estruct *es = get_event_struct (L);

   if (es) { result = es->defaultHandle; }

   return result;
}


int
dmz::lua_event_locality_to_int (lua_State *L, const EventLocalityEnum Locality) {

   int result (0);

   estruct *es = get_event_struct (L);

   if (Locality == EventLocal) { result = es->localityLocal; }
   else if (Locality == EventRemote) { result = es->localityRemote; }

   return result;
}


dmz::LuaExtEvent::LuaExtEvent (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      LuaExt (Info),
      L (0),
      _eventPtr (0),
      _log (Info) {

   _event.ext = this;
   _init (local);
}


dmz::LuaExtEvent::~LuaExtEvent () {

   _obsTable.clear ();
}


void
dmz::LuaExtEvent::register_event_observer (EventObserver &obs) {

   if (_obsTable.store (obs.get_event_observer_handle (), &obs)) {

      if (_event.eventMod) { obs.store_event_module ("", *(_event.eventMod)); }
   }
}


void
dmz::LuaExtEvent::release_event_observer (EventObserver &obs) {

   if (_obsTable.remove (obs.get_event_observer_handle ())) {

      if (_event.eventMod) { obs.remove_event_module ("", *(_event.eventMod)); }
   }
}


// Plugin Interface
void
dmz::LuaExtEvent::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_event.commonEventMod) {

         _event.commonEventMod = EventModuleCommon::cast (PluginPtr);
      }

      if (!_event.eventMod) {

         _event.eventMod = EventModule::cast (PluginPtr);

         if (_event.eventMod) {

            HashTableHandleIterator it;

            EventObserver *obs (_obsTable.get_first (it));

            while (obs) {

               obs->store_event_module ("", *(_event.eventMod));

               obs = _obsTable.get_next (it);
            }
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_event.eventMod && (_event.eventMod == EventModule::cast (PluginPtr))) {

         HashTableHandleIterator it;

         EventObserver *obs (_obsTable.get_first (it));

         while (obs) {

            obs->remove_event_module ("", *(_event.eventMod));

            obs = _obsTable.get_next (it);
         }
      }
   }
}


// LuaExt Interface
void
dmz::LuaExtEvent::store_lua_module (LuaModule &module) {

}


void
dmz::LuaExtEvent::open_lua_extension (lua_State *LuaState) {

   L = LuaState;

   LUA_START_VALIDATE (L);

   lua_init_event_observer (L);

   Definitions defs (get_plugin_runtime_context (), &_log);

   lua_pushlightuserdata (L, (void *)&EventKey);
   _eventPtr = (EventStruct **)lua_newuserdata (L, sizeof (EventStruct *));

   if (_eventPtr) {

      *_eventPtr = &_event;
      lua_rawset (L, LUA_REGISTRYINDEX);
   }
   else { lua_pop (L, 1); } // pop light data

   lua_create_dmz_namespace (L, EventNamespace);
   const int Table (lua_gettop (L));
   luaL_register (L, NULL, arrayFunc);

   _event.localityLocal = lua_create_enum (L, "dmzEventLocalityLocal");
   _event.localityRemote = lua_create_enum (L, "dmzEventLocalityRemote");

   lua_pushinteger (L, _event.localityLocal);
   lua_setfield (L, -2, "Local");

   lua_pushinteger (L, _event.localityRemote);
   lua_setfield (L, -2, "Remote");

   lua_create_mask (L, &EventCreateMask);
   lua_setfield (L, Table, "CreateMask");

   lua_create_mask (L, &EventCloseMask);
   lua_setfield (L, Table, "CloseMask");

   lua_create_handle (L, defs.create_named_handle (EventAttributeDefaultName));
   lua_setfield (L, Table, "DefaultHandle");

   lua_create_handle (L, defs.create_named_handle (EventAttributeSourceName));
   lua_setfield (L, Table, "SourceHandle");

   lua_create_handle (L, defs.create_named_handle (EventAttributeTargetName));
   lua_setfield (L, Table, "TargetHandle");

   lua_create_handle (L, defs.create_named_handle (EventAttributeMunitionsName));
   lua_setfield (L, Table, "MunitionsHandle");

   lua_make_readonly (L, -1); // make event read only
   lua_pop (L, 1); // pop event table

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtEvent::close_lua_extension (lua_State *LuaState) {

   HashTableHandleIterator it;
   EventObserver *obs (0);
   while (_obsTable.get_next (it, obs)) { release_event_observer (*obs); }

   _obsTable.clear ();

   L = 0;
}


void
dmz::LuaExtEvent::remove_lua_module (LuaModule &module) {

}


void
dmz::LuaExtEvent::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _event.defaultHandle = defs.create_named_handle (EventAttributeDefaultName);
}


//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzLuaExtEvent (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::LuaExtEvent (Info, local);
}

};


