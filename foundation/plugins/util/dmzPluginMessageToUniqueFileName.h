#ifndef DMZ_PLUGIN_MESSAGE_TO_UNIQUE_FILE_NAME_DOT_H
#define DMZ_PLUGIN_MESSAGE_TO_UNIQUE_FILE_NAME_DOT_H

#include <dmzRuntimeDataConverterTypesBase.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class PluginMessageToUniqueFileName :
         public Plugin,
         public MessageObserver {

      public:
         PluginMessageToUniqueFileName (const PluginInfo &Info, Config &local);
         ~PluginMessageToUniqueFileName ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Message Observer Interface
         virtual void receive_message (
            const Message &Type,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

      protected:
         void _init (Config &local);

         Log _log;
         DataConverterString _convert;

         Message _inMsg;
         Message _outMsg;
         Handle _target;
         Int32 _index;
         String _filePath;
         String _fileRoot;
         String _fileExt;

      private:
         PluginMessageToUniqueFileName ();
         PluginMessageToUniqueFileName (const PluginMessageToUniqueFileName &);
         PluginMessageToUniqueFileName &operator= (const PluginMessageToUniqueFileName &);

   };
};

#endif // DMZ_PLUGIN_MESSAGE_TO_UNIQUE_FILE_NAME_DOT_H
