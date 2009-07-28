#ifndef DMZ_RUNTIME_PLUGIN_INFO_DOT_H
#define DMZ_RUNTIME_PLUGIN_INFO_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class RuntimeContext;
   class DynamicLibrary;

   enum PluginDeleteModeEnum {
      PluginDeleteModeDelete, //!< Plugin should be deleted by container.
      PluginDeleteModeDoNotDelete, //!< Plugin should \b not be delete by container.
   };

   class DMZ_KERNEL_LINK_SYMBOL PluginInfo {

      public:
         PluginInfo (
            const String &Name,
            const String &ClassName,
            const String &FactoryName,
            const String &ScopeName,
            const PluginDeleteModeEnum DeleteMode,
            RuntimeContext *context,
            DynamicLibrary *lib);

         PluginInfo (
            const String &Name,
            const PluginDeleteModeEnum DeleteMode,
            RuntimeContext *context,
            DynamicLibrary *lib);

         ~PluginInfo ();

         String get_name () const;
         Handle get_handle () const;
         String get_class_name () const;
         String get_factory_name () const;
         String get_scope_name () const;
         PluginDeleteModeEnum get_delete_mode () const;
         RuntimeContext *get_context () const;
         DynamicLibrary *get_dynamic_library () const;

         Boolean uses_level (const UInt32 Level) const;
         void add_level (const UInt32 Level);
         UInt32 get_first_level () const;
         UInt32 get_next_level () const;

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         PluginInfo ();
         PluginInfo (const PluginInfo &Info);
         PluginInfo &operator= (const PluginInfo &Info);
   };
};

#endif // DMZ_RUNTIME_PLUGIN_INFO_DOT_H
