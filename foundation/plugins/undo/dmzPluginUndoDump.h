#ifndef DMZ_PLUGIN_UNDO_DUMP_DOT_H
#define DMZ_PLUGIN_UNDO_DUMP_DOT_H

#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeUndo.h>

namespace dmz {

   class PluginUndoDump :
         public Plugin,
         public MessageObserver,
         public UndoDump {

      public:
         PluginUndoDump (const PluginInfo &Info, Config &local);
         ~PluginUndoDump ();

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
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

         // Undo Dump Interface
         virtual void start_record (const Handle RecordHandle, const String &Name);

         virtual void store_action (
            const Message &Type,
            const Handle Target,
            const Data *Value);

      protected:
         void _init (Config &local);

         Definitions _defs;
         Undo _undo;
         Log _log;

      private:
         PluginUndoDump ();
         PluginUndoDump (const PluginUndoDump &);
         PluginUndoDump &operator= (const PluginUndoDump &);

   };
};

#endif // DMZ_PLUGIN_UNDO_DUMP_DOT_H
