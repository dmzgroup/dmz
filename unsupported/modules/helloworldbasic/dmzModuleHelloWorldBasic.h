#ifndef DMZ_MODULE_HELLO_WORLD_BASIC_DOT_H
#define DMZ_MODULE_HELLO_WORLD_BASIC_DOT_H

#include <dmzModuleHelloWorld.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesString.h>

namespace dmz {

   class RuntimeContext;

   class ModuleHelloWorldBasic : public Plugin, public ModuleHelloWorld {

      public:
         ModuleHelloWorldBasic (const PluginInfo &Info);
         virtual ~ModuleHelloWorldBasic ();

         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         virtual void print_hello ();

      protected:
         Log _log;

      private:
         ModuleHelloWorldBasic ();
         ModuleHelloWorldBasic (const ModuleHelloWorldBasic &);
         ModuleHelloWorldBasic &operator= (const ModuleHelloWorldBasic &);
   };
};

#endif // DMZ_MODULE_HELLO_WORLD_BASIC_DOT_H
