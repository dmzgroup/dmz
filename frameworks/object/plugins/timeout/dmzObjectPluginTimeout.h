#ifndef DMZ_WEAPON_PLUGIN_TIMEOUT_DOT_H
#define DMZ_WEAPON_PLUGIN_TIMEOUT_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class EventModuleCommon;

   class ObjectPluginTimeout :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil {

      public:
         ObjectPluginTimeout (const PluginInfo &Info, Config &local);
         ~ObjectPluginTimeout ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 DeltaTime);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

      protected:
         Float64 *_find_timeout (const ObjectType &Type);
         void _init (Config &local);

         Log _log;
         EventModuleCommon *_eventMod;
         HashTableHandleTemplate<Float64> _objTable;
         HashTableHandleTemplate<Float64> _timeoutTable;
         HashTableHandleTemplate<Float64> _masterTimeoutTable;
         Float64 *_defaultTimeout;

      private:
         ObjectPluginTimeout ();
         ObjectPluginTimeout (const ObjectPluginTimeout &);
         ObjectPluginTimeout &operator= (const ObjectPluginTimeout &);

   };
};

#endif // DMZ_WEAPON_PLUGIN_TIMEOUT_DOT_H
