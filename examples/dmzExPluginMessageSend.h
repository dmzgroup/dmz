#ifndef DMZ_EX_PLUGIN_MESSAGE_SEND_DOT_H
#define DMZ_EX_PLUGIN_MESSAGE_SEND_DOT_H

#include <dmzRuntimeDataBinder.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeSync.h>

namespace dmz {

   class ExPluginMessageSend :
         public Plugin,
         public Sync {

      public:
         ExPluginMessageSend (const PluginInfo &Info, Config &local);
         ~ExPluginMessageSend ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // Sync Interface
         virtual void update_sync (const Float64 TimeDelta);
      protected:
         void _init (Config &local);

         Log _log;
         Message _type;
         DataBinder _binder;
         String _value;
         Float64 _time;

      private:
         ExPluginMessageSend ();
         ExPluginMessageSend (const ExPluginMessageSend &);
         ExPluginMessageSend &operator= (const ExPluginMessageSend &);

   };
};

#endif // DMZ_EX_PLUGIN_MESSAGE_SEND_DOT_H
