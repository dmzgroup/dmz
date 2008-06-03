#ifndef DMZ_PLUGIN_HELLO2_DOT_H
#define DMZ_PLUGIN_HELLO2_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesVector.h>

namespace dmz {

   class PluginHello2 :
         public Plugin {

      public:
         PluginHello2 (const PluginInfo &Info, Config &local);
         ~PluginHello2 ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

      protected:
         void _init (Config &local);
         Vector _point;

         Log _log;

      private:
         PluginHello2 ();
         PluginHello2 (const PluginHello2 &);
         PluginHello2 &operator= (const PluginHello2 &);

   };
};

#endif // DMZ_PLUGIN_HELLO2_DOT_H
