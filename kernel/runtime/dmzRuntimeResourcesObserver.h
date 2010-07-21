#ifndef DMZ_RUNTIME_RESOURCES_OBSERVER_DOT_H
#define DMZ_RUNTIME_RESOURCES_OBSERVER_DOT_H

#include <dmzKernelExport.h>

namespace dmz {

class PluginInfo;
class RuntimeContext;

enum ResourcesActivateModeEnum {
   ResourcesDumpAll, //!< Dump all currently defined resources.
   ResourcesDumpNone //!< Do not dump all currently defined resources.
};

enum ResourcesUpdateTypeEnum {
   ResourcesCreated, //!< Resource was created.
   ResourcesUpdated, //!< Resource was updated.
   ResourcesRemoved, //!< Resource was removed.
   ResourcesDumped   //!< Resource was dumped.
};

class DMZ_KERNEL_LINK_SYMBOL ResourcesObserver {

   public:
      void activate_resources_callback (const ResourcesActivateModeEnum Mode);
      void deactivate_resources_callback ();
      void dump_current_resources ();

      virtual void update_resource (
         const String &Name,
         const ResourcesUpdateTypeEnum Type) = 0;

   protected:
      ResourcesObserver (RuntimeContext *context);
      ResourcesObserver (const PluginInfo &Info);
      virtual ~ResourcesObserver ();

   private:
      struct State;
      State &__state;

      ResourcesObserver ();
      ResourcesObserver (const ResourcesObserver &);
      ResourcesObserver &operator= (const ResourcesObserver &);
};

};

#endif // DMZ_RUNTIME_RESOURCES_OBSERVER_DOT_H
