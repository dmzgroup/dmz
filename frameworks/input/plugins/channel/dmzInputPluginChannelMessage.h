#ifndef DMZ_INPUT_PLUGIN_CHANNEL_MESSAGE_DOT_H
#define DMZ_INPUT_PLUGIN_CHANNEL_MESSAGE_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class InputPluginChannelMessage :
         public Plugin,
         public InputObserverUtil {

      public:
         InputPluginChannelMessage (const PluginInfo &Info, Config &local);
         ~InputPluginChannelMessage ();

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
         // InputPluginChannelMessage Interface
         struct ChannelStruct {

            const Handle Target;
            const Message Msg;
            Data *data;

            ChannelStruct (
                  const Handle TheTarget,
                  const Message &TheMsg,
                  const Data *TheData) :
                  Target (TheTarget),
                  Msg (TheMsg),
                  data (TheData ? new Data (*TheData) : 0) {;}

            ~ChannelStruct () { if (data) { delete data; data = 0; } }
         };

         void _init (Config &local);

         Log _log;
         HashTableHandleTemplate<ChannelStruct> _channelTable;

      private:
         InputPluginChannelMessage ();
         InputPluginChannelMessage (const InputPluginChannelMessage &);
         InputPluginChannelMessage &operator= (const InputPluginChannelMessage &);

   };
};

#endif // DMZ_INPUT_PLUGIN_CHANNEL_MESSAGE_DOT_H
