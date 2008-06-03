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
         virtual void discover_plugin (const Plugin *PluginPtr) {;}
         virtual void start_plugin () {;}
         virtual void stop_plugin () {;}
         virtual void shutdown_plugin () {;}
         virtual void remove_plugin (const Plugin *PluginPtr) {;}

         // Message Observer Interface
         void receive_message (
            const MessageType &Msg,
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
