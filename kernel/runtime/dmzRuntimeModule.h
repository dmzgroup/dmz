#ifndef DMZ_RUNTIME_MODULE_DOT_H
#define DMZ_RUNTIME_MODULE_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class HandleContainer;
   class Plugin;
   class PluginContainer;
   class PluginInfo;

   class RuntimeModule {

      public:
         static RuntimeModule *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         String get_runtime_module_name () const;
         Handle get_runtime_module_handle () const;

         // RuntimeModule Interface
         virtual void get_plugin_list (HandleContainer &container) = 0;
         virtual const PluginInfo *lookup_plugin_info (const Handle PluginHandle) = 0;
         virtual Boolean add_plugin (PluginInfo *info, Plugin *plugin) = 0;
         virtual Boolean add_plugins (PluginContainer &container) = 0;
         virtual Boolean unload_plugin (const Handle PluginHandle) = 0;

      protected:
         RuntimeModule (const PluginInfo &Info);
         ~RuntimeModule ();

      private:
         RuntimeModule ();
         RuntimeModule (const RuntimeModule &);
         RuntimeModule &operator= (const RuntimeModule &);

         const PluginInfo &__Info;
   };

   //! \cond
   const char RuntimeModuleInterfaceName[] = "RuntimeModuleInterface";
   //! \endcond
};


inline dmz::RuntimeModule *
dmz::RuntimeModule::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (RuntimeModule *)lookup_rtti_interface (
      RuntimeModuleInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::RuntimeModule::RuntimeModule (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (RuntimeModuleInterfaceName, __Info, (void *)this);
}


inline
dmz::RuntimeModule::~RuntimeModule () {

   remove_rtti_interface (RuntimeModuleInterfaceName, __Info);
}


inline dmz::String
dmz::RuntimeModule::get_runtime_module_name () const { return __Info.get_name (); }


inline dmz::Handle
dmz::RuntimeModule::get_runtime_module_handle () const { return __Info.get_handle (); }

#endif // DMZ_RUNTIME_MODULE_DOT_H
