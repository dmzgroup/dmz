#ifndef DMZ_NET_PLUGIN_REMOTE_DR_DOT_H
#define DMZ_NET_PLUGIN_REMOTE_DR_DOT_H

#include <dmzObjectModule.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzRuntimeTime.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class NetPluginRemoteDR :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil {

      public:
         NetPluginRemoteDR (const PluginInfo &Info, Config &local);
         ~NetPluginRemoteDR ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Idnentiy,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_locality (
            const UUID &Idnentiy,
            const Handle ObjectHandle,
            const ObjectLocalityEnum Locality,
            const ObjectLocalityEnum PrevLocality);

      protected:
         void _init (Config &local);

         Log _log;
         Time _time;

         ObjectModule *_objMod;

         Handle _defaultHandle;
         Handle _lnvHandle;

         HashTableHandleTemplate<NetPluginRemoteDR> _objTable;

      private:
         NetPluginRemoteDR ();
         NetPluginRemoteDR (const NetPluginRemoteDR &);
         NetPluginRemoteDR &operator= (const NetPluginRemoteDR &);
   };
};

#endif // DMZ_NET_PLUGIN_REMOTE_DR_DOT_H
