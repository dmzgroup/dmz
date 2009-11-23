#ifndef DMZ_PLUGIN_SAVE_MESSAGE_MONOSTATE_DOT_H
#define DMZ_PLUGIN_SAVE_MESSAGE_MONOSTATE_DOT_H

#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesStringContainer.h>


namespace dmz {

   class PluginSaveMessageMonostate :
         public Plugin {

      public:
         //! \cond
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
         void _restore_messages (Config &list);
         void _init_messages (Config &list);
         void _init (Config &local);

         Log _log;
         Definitions _defs;
         StringContainer _messages;
         //! \endcond

      private:
         PluginSaveMessageMonostate ();
         PluginSaveMessageMonostate (const PluginSaveMessageMonostate &);
         PluginSaveMessageMonostate &operator= (const PluginSaveMessageMonostate &);
   };
};

#endif // DMZ_PLUGIN_SAVE_MESSAGE_MONOSTATE_DOT_H
