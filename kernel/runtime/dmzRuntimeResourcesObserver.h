#ifndef DMZ_RUNTIME_RESOURCES_OBSERVER_DOT_H
#define DMZ_RUNTIME_RESOURCES_OBSERVER_DOT_H

#include <dmzKernelExport.h>

namespace dmz {

class PluginInfo;
class RuntimeContext;

enum ResourcesModeEnum {
   ResourceCreated, //!< Resource was created.
   ResourceUpdated, //!< Resource was updated.
   ResourceRemoved, //!< Resource was removed.
   ResourceDumped   //!< Resource was dumped.
};

class DMZ_KERNEL_LINK_SYMBOL ResourcesObserver {

   public:
      void activate_resources_calllback ();
      void deactivate_resources_calllback ();
      void dump_current_resources ();
      virtual void update_resource (const String &Name, const ResourcesModeEnum Mode) = 0;

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
