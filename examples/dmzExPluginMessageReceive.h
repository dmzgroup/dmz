#ifndef DMZ_EX_PLUGIN_MESSAGE_RECEIVE_DOT_H
#define DMZ_EX_PLUGIN_MESSAGE_RECEIVE_DOT_H

#include <dmzRuntimeDataBinder.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>


namespace dmz {

   class ExPluginMessageReceive :
         public Plugin,
         public MessageObserver {

      public:
         ExPluginMessageReceive (const PluginInfo &Info, Config &local);
         ~ExPluginMessageReceive ();

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
         void _init (Config &local);

         Log _log;
         Message _type;
         DataBinder _binder;
         String _value;
         Float64 _time;
         
      private:
         ExPluginMessageReceive ();
         ExPluginMessageReceive (const ExPluginMessageReceive &);
         ExPluginMessageReceive &operator= (const ExPluginMessageReceive &);

   };
};

#endif // DMZ_EX_PLUGIN_MESSAGE_RECEIVE_DOT_H
