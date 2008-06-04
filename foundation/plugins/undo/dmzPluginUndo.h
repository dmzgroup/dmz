#ifndef DMZ_PLUGIN_UNDO_DOT_H
#define DMZ_PLUGIN_UNDO_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeUndo.h>

namespace dmz {

   class PluginUndo :
         public Plugin,
         public MessageObserver {

      public:
         PluginUndo (const PluginInfo &Info, Config &local);
         ~PluginUndo ();

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

      protected:
         void _init (Config &local);

         Message _undoMessage;
         Message _redoMessage;

         Undo _undo;
         Log _log;

      private:
         PluginUndo ();
         PluginUndo (const PluginUndo &);
         PluginUndo &operator= (const PluginUndo &);

   };
};

#endif // DMZ_PLUGIN_UNDO_DOT_H
