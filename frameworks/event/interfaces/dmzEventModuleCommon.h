#ifndef DMZ_EVENT_MODULE_COMMON_DOT_H
#define DMZ_EVENT_MODULE_COMMON_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   const char EventModuleCommonInterfaceName[] = "EventModuleCommonInterface";

   class EventModuleCommon {

      public:
         static EventModuleCommon *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         String get_event_module_common_name () const;
         Handle get_event_module_common_handle () const;

         // EventModuleCommon Interface
         virtual Handle create_launch_event (
            const Handle MunitionsHandle,
            const Handle TargetHandle) = 0;

         virtual Handle create_open_launch_event (
            const Handle MunitionsHandle,
            const Handle TargetHandle) = 0;

         virtual Handle create_detonation_event (
            const Handle MunitionsHandle,
            const Handle TargetHandle) = 0;

         virtual Handle create_open_detonation_event (
            const Handle MunitionsHandle,
            const Handle TargetHandle) = 0;

         virtual Handle create_collision_event (
            const Handle SourceHandle,
            const Handle TargetHandle) = 0;

         virtual Handle create_open_collision_event (
            const Handle SourceHandle,
            const Handle TargetHandle) = 0;

      protected:
         EventModuleCommon (const PluginInfo &Info);
         ~EventModuleCommon ();

      private:
         EventModuleCommon ();
         EventModuleCommon (const EventModuleCommon &);
         EventModuleCommon &operator= (const EventModuleCommon &);

         const PluginInfo &__Info;
   };
};


inline dmz::EventModuleCommon *
dmz::EventModuleCommon::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (EventModuleCommon *)lookup_rtti_interface (
      EventModuleCommonInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::EventModuleCommon::EventModuleCommon (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (EventModuleCommonInterfaceName, __Info, (void *)this);
}


inline
dmz::EventModuleCommon::~EventModuleCommon () {

   remove_rtti_interface (EventModuleCommonInterfaceName, __Info);
}


inline dmz::String
dmz::EventModuleCommon::get_event_module_common_name () const {

   return __Info.get_name ();
}


inline dmz::Handle
dmz::EventModuleCommon::get_event_module_common_handle () const {

   return __Info.get_handle ();
}

#endif // DMZ_EVENT_MODULE_COMMON_DOT_H
