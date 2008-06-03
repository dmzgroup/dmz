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
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // Message Observer Interface
         void receive_message (
            const MessageType &Msg,
            const UInt32 MessageSendHandle,
            const UInt32 TargetObserverHandle,
            const Data *InData,
            Data *outData);

      protected:
         void _init (Config &local);

         Log _log;
         MessageType _type;
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
