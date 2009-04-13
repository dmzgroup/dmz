#ifndef DMZ_OBJECT_PLUGIN_AUTO_COUNTER_DOT_H
#define DMZ_OBJECT_PLUGIN_AUTO_COUNTER_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class ObjectPluginAutoCounter :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil {

      public:
         ObjectPluginAutoCounter (const PluginInfo &Info, Config &local);
         ~ObjectPluginAutoCounter ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Object Observer Interface
         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_counter (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Int64 Value,
            const Int64 *PreviousValue);

      protected:
         struct CounterStruct {

            const Handle ObjectHandle;
            Float64 timer;
            CounterStruct (const Handle TheObjectHandle) :
                  ObjectHandle (TheObjectHandle),
                  timer (0.0) {;}
         };

         void _init (Config &local);

         Log _log;

         Float64 _rate;
         Int64 _increase;

         Handle _counterAttrHandle;

         HashTableHandleTemplate<CounterStruct> _objTable;

      private:
         ObjectPluginAutoCounter ();
         ObjectPluginAutoCounter (const ObjectPluginAutoCounter &);
         ObjectPluginAutoCounter &operator= (const ObjectPluginAutoCounter &);

   };
};

#endif // DMZ_OBJECT_PLUGIN_AUTO_COUNTER_DOT_H
