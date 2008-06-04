#ifndef DMZ_PLUGIN_MSG_DUMP_DOT_H
#define DMZ_PLUGIN_MSG_DUMP_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class PluginMsgDump :
         public Plugin,
         public MessageObserver {

      public:
         //! \cond
         PluginMsgDump (const PluginInfo &Info, Config &local);
         ~PluginMsgDump () {;}

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // Message Observer Interface
         void receive_message (
            const Message &Msg,
            const UInt32 MessageSendHandle,
            const UInt32 TargetObserverHandle,
            const Data *InData,
            Data *outData);

      protected:
         Log _log;
         //! \endcond

      private:
         PluginMsgDump (const PluginMsgDump &);
         PluginMsgDump &operator= (const PluginMsgDump &);

   };
};

#endif // DMZ_PLUGIN_MSG_DUMP_DOT_H
