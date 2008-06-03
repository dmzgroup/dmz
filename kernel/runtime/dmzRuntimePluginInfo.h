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
            const PluginDeleteModeEnum DeleteMode,
            RuntimeContext *context,
            DynamicLibrary *lib);

         ~PluginInfo ();

         String get_name () const;

         Handle get_handle () const;

         void set_delete_mode (const PluginDeleteModeEnum Mode);
         PluginDeleteModeEnum get_delete_mode () const;

         void set_context (RuntimeContext *context);
         RuntimeContext *get_context () const;

         void set_dynamic_library (DynamicLibrary *lib);
         DynamicLibrary *get_dynamic_library ();

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
