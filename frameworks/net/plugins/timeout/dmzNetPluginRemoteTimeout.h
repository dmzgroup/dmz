#ifndef DMZ_NET_PLUGIN_REMOTE_TIMEOUT_DOT_H
#define DMZ_NET_PLUGIN_REMOTE_TIMEOUT_DOT_H

#include <dmzObjectModule.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeSync.h>
#include <dmzRuntimeTime.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class NetPluginRemoteTimeout :
         public Plugin,
         public Sync,
         public ObjectObserverUtil {

      public:
         NetPluginRemoteTimeout (const PluginInfo &Info, Config &local);
         ~NetPluginRemoteTimeout ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Sync Interface
         virtual void update_sync (const Float64 TimeDelta);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_locality (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectLocalityEnum Locality,
            const ObjectLocalityEnum PrevLocality);

         virtual void update_object_time_stamp (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue);

      protected:
         struct ObjStruct {

            const Handle ObjectHandle;
            Float64 value;
            Boolean isSet;

            ObjStruct (const Handle TheHandle) :
               ObjectHandle (TheHandle),
               value (0.0),
               isSet (False) {;}
         };

         void _init (Config &local);

         Log _log;
         Time _time;

         Handle _lnvHandle;
         Float64 _timeoutInterval;

         HashTableHandleTemplate<ObjStruct> _objTable;

         ObjectModule *_objMod;

      private:
         NetPluginRemoteTimeout ();
         NetPluginRemoteTimeout (const NetPluginRemoteTimeout &);
         NetPluginRemoteTimeout &operator= (const NetPluginRemoteTimeout &);

   };
};

#endif // DMZ_NET_PLUGIN_REMOTE_TIMEOUT_DOT_H
