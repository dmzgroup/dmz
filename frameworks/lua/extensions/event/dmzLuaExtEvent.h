#ifndef DMZ_LUA_EXT_EVENT_DOT_H
#define DMZ_LUA_EXT_EVENT_DOT_H

#include <dmzLuaExt.h>
#include <dmzEventObserver.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesHashTableHandleTemplate.h>

struct lua_State;

namespace dmz {

   //! \cond
   void lua_register_event_observer (lua_State *L, EventObserver &obs);
   void lua_release_event_observer (lua_State *L, EventObserver &obs);
   Handle lua_get_default_event_handle (lua_State *L);
   int lua_event_locality_to_int (lua_State *L, const EventLocalityEnum Locality);
   //! \endcond

   class EventModuleCommon;

   class LuaExtEvent :
         public Plugin,
         public LuaExt {

      public:
         //! \cond
         struct EventStruct {

            EventModule *eventMod;
            EventModuleCommon *commonEventMod;
            LuaExtEvent *ext;
            Handle defaultHandle;

            int localityLocal;
            int localityRemote;

            EventStruct () :
               eventMod (0),
               commonEventMod (0),
               ext (0),
               defaultHandle (0),
               localityLocal (0),
               localityRemote (0) {;}
         };

         LuaExtEvent (const PluginInfo &Info, Config &local);
         ~LuaExtEvent ();

         void register_event_observer (EventObserver &obs);
         void release_event_observer (EventObserver &obs);

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // LuaExt Interface
         virtual void store_lua_module (LuaModule &module);
         virtual void open_lua_extension (lua_State *LuaState);
         virtual void close_lua_extension (lua_State *LuaState);
         virtual void remove_lua_module (LuaModule &module);

      protected:
         void _init (Config &local);

         HashTableHandleTemplate<EventObserver> _obsTable;

         lua_State *L;

         EventStruct _event;
         EventStruct **_eventPtr;

         Log _log;
         //! \endcond

      private:
         LuaExtEvent ();
         LuaExtEvent (const LuaExtEvent &);
         LuaExtEvent &operator= (const LuaExtEvent &);
   };
};

#endif // DMZ_LUA_EXT_EVENT_DOT_H
