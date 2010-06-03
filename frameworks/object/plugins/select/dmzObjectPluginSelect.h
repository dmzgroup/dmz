#ifndef DMZ_OBJECT_PLUGIN_SELECT_DOT_H
#define DMZ_OBJECT_PLUGIN_SELECT_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzRuntimeDataConverterTypesBase.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class ObjectModule;
   class ObjectModuleSelect;

   class ObjectPluginSelect :
         public Plugin,
         public MessageObserver,
         public InputObserverUtil {

      public:
         ObjectPluginSelect (const PluginInfo &Info, Config &local);
         ~ObjectPluginSelect ();

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

         // Input Observer Interface
         virtual void receive_key_event (
            const Handle Channel,
            const InputEventKey &Value);

      protected:
         // ObjectPluginSelect Interface
         void _init (Config &local);

         Log _log;
         DataConverterHandle _convert;

         ObjectModule *_object;
         ObjectModuleSelect *_select;

         Message _selectMsg;
         Boolean _addToSelection;
         UInt32 _key;

      private:
         ObjectPluginSelect ();
         ObjectPluginSelect (const ObjectPluginSelect &);
         ObjectPluginSelect &operator= (const ObjectPluginSelect &);

   };
};

#endif // DMZ_OBJECT_PLUGIN_SELECT_DOT_H
