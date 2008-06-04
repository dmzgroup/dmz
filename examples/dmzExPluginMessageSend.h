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
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

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
