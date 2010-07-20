#ifndef DMZ_INPUT_PLUGIN_CHANNEL_RULES_DOT_H
#define DMZ_INPUT_PLUGIN_CHANNEL_RULES_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class InputModule;

   class InputPluginChannelRules :
         public Plugin,
         public InputObserverUtil {

      public:
         //! \cond
         InputPluginChannelRules (const PluginInfo &Info, Config &local);
         ~InputPluginChannelRules ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State);

      protected:
         struct ChannelStruct {

            const Handle Channel;
            ChannelStruct *next;

            ChannelStruct (const Handle TheHandle) :
                  Channel (TheHandle),
                  next (0) {;}

            ~ChannelStruct () {;}
         };

         // InputPluginChannelRules Interface
         void _init (Config &local);

         Log _log;
         Int32 _active;
         ChannelStruct *_channelList;
         Handle _defaultChannel;
         InputModule *_input;
         String _inputModuleName;
         //! \endcond

      private:
         InputPluginChannelRules ();
         InputPluginChannelRules (const InputPluginChannelRules &);
         InputPluginChannelRules &operator= (const InputPluginChannelRules &);

   };
};

#endif // DMZ_INPUT_PLUGIN_CHANNEL_RULES_DOT_H
