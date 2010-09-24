#ifndef DMZ_RUNTIME_RESOURCES_OBSERVER_DOT_H
#define DMZ_RUNTIME_RESOURCES_OBSERVER_DOT_H

#include <dmzKernelExport.h>

namespace dmz {

class PluginInfo;
class RuntimeContext;

//! \addtogroup Runtime
//! @(

//! \brief Activate update_resources_path callback in dmz::ResourcesObserver
//! \details Defined in dmzRuntimeResourcesObserver.h
//! \sa dmz::ResourcesObserver::set_resources_observer_callback_mask()
const UInt32 ResourcesPathMask = 0x01;
//! \brief Activate update_resource callback in dmz::ResourcesObserver
//! \details Defined in dmzRuntimeResourcesObserver.h
//! \sa dmz::ResourcesObserver::set_resources_observer_callback_mask()
const UInt32 ResourcesResourceMask = 0x02;
//! \brief Activate all callbacks in dmz::ResourcesObserver
//! \details Defined in dmzRuntimeResourcesObserver.h
//! \sa dmz::ResourcesObserver::set_definitions_observer_callback_mask()
const UInt32 ResourcesAllMask = 0xFF;

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

//! @}

class DMZ_KERNEL_LINK_SYMBOL ResourcesObserver {

   public:
      UInt32 get_resources_observer_callback_mask () const;
      UInt32 set_resources_observer_callback_mask (
         const ResourcesActivateModeEnum Mode,
         const UInt32 Mask);

      void dump_current_resources ();

      virtual void update_resources_path (
         const String &Name,
         const ResourcesUpdateTypeEnum Type);

      virtual void update_resource (
         const String &Name,
         const ResourcesUpdateTypeEnum Type);

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
