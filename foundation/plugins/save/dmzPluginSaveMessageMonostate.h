#ifndef DMZ_PLUGIN_SAVE_MESSAGE_MONOSTATE_DOT_H
#define DMZ_PLUGIN_SAVE_MESSAGE_MONOSTATE_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>


namespace dmz {

   class PluginSaveMessageMonostate :
         public Plugin {

      public:
         PluginSaveMessageMonostate (const PluginInfo &Info, Config &local);
         ~PluginSaveMessageMonostate ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

      protected:
         void _init (Config &local);

         Log _log;

      private:
         PluginSaveMessageMonostate ();
         PluginSaveMessageMonostate (const PluginSaveMessageMonostate &);
         PluginSaveMessageMonostate &operator= (const PluginSaveMessageMonostate &);
   };
};

#endif // DMZ_PLUGIN_SAVE_MESSAGE_MONOSTATE_DOT_H
