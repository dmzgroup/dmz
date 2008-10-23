#ifndef DMZ_LUA_EXT_OBJECT_DOT_H
#define DMZ_LUA_EXT_OBJECT_DOT_H

#include <dmzLuaExt.h>
#include <dmzObjectModuleGrid.h>
#include <dmzObjectModuleSelect.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesHashTableHandleTemplate.h>

struct lua_State;

namespace dmz {

   //! \cond
   void lua_register_object_observer (lua_State *L, ObjectObserver &obs);
   void lua_release_object_observer (lua_State *L, ObjectObserver &obs);
   Handle lua_get_default_handle (lua_State *L);
   int lua_locality_to_int (lua_State *L, const ObjectLocalityEnum Locality);
   //! \endcond

   class LuaExtObject :
         public Plugin,
         public LuaExt,
         public ObjectObserverUtil {

      public:
         //! \cond
         struct ObjectStruct {

            ObjectModule *objMod;
            ObjectModuleSelect *selectMod;
            ObjectModuleGrid *gridMod;
            LuaExtObject *ext;
            Handle defaultHandle;
            Handle hilHandle;
            HandleContainer tempObjects;

            int localityLocal;
            int localityRemote;
            int linksRetain;
            int linksIgnore;
            int selectAdd;
            int selectNew;

            ObjectStruct () :
               objMod (0),
               selectMod (0),
               gridMod (0),
               ext (0),
               defaultHandle (0),
               hilHandle (0),
               localityLocal (0),
               localityRemote (0),
               linksRetain (0),
               linksIgnore (0),
               selectAdd (0),
               selectNew (0) {;}
         };

         LuaExtObject (const PluginInfo &Info, Config &local);
         ~LuaExtObject ();

         void register_object_observer (ObjectObserver &obs);
         void release_object_observer (ObjectObserver &obs);

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

         // Object Observer Interface
        virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

      protected:
         void _init (Config &local);
         virtual void _store_object_module (ObjectModule &objMod);
         virtual void _remove_object_module (ObjectModule &objMod);

         lua_State *L;

         ObjectStruct _obj;
         ObjectStruct **_objPtr;

         Handle _hilAttrHandle;

         HashTableHandleTemplate<ObjectObserver> _obsTable;

         Log _log;
         //! \endcond

      private:
         LuaExtObject ();
         LuaExtObject (const LuaExtObject &);
         LuaExtObject &operator= (const LuaExtObject &);

   };
};

#endif // DMZ_LUA_EXT_OBJECT_DOT_H
