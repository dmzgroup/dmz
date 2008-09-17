#ifndef DMZ_EVENT_MODULE_COMMON_BASIC_DOT_H
#define DMZ_EVENT_MODULE_COMMON_BASIC_DOT_H

#include <dmzEventModuleCommon.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class EventModule;
   class ObjectModule;

   class EventModuleCommonBasic :
         public Plugin,
         public EventModuleCommon {

      public:
         //! \cond
         EventModuleCommonBasic (const PluginInfo &Info, Config &local);
         ~EventModuleCommonBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // EventModuleCommon Interface
         virtual Handle create_launch_event (
            const Handle MunitionsHandle,
            const Handle TargetHandle);

         virtual Handle create_open_launch_event (
            const Handle MunitionsHandle,
            const Handle TargetHandle);

         virtual Handle create_detonation_event (
            const Handle MunitionsHandle,
            const Handle TargetHandle);

         virtual Handle create_open_detonation_event (
            const Handle MunitionsHandle,
            const Handle TargetHandle);

         virtual Handle create_collision_event (
            const Handle SourceHandle,
            const Handle TargetHandle);

         virtual Handle create_open_collision_event (
            const Handle SourceHandle,
            const Handle TargetHandle);

      protected:
         Handle _get_source (const Handle ObjectHandle);
         void _init (Config &local);

         Log _log;

         EventModule *_eventMod;
         ObjectModule *_objMod;

         EventType _launchType;
         EventType _detonationType;
         EventType _collisionType;

         Handle _defaultObjectHandle;
         Handle _defaultEventHandle;
         Handle _sourceHandle;
         Handle _targetHandle;
         Handle _munitionsHandle;
         //! \endcond

      private:
         EventModuleCommonBasic ();
         EventModuleCommonBasic (const EventModuleCommonBasic &);
         EventModuleCommonBasic &operator= (const EventModuleCommonBasic &);

   };
};

#endif // DMZ_EVENT_MODULE_COMMON_BASIC_DOT_H
