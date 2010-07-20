#ifndef DMZ_OBJECT_PLUGIN_SELECT_SIMILAR_DOT_H
#define DMZ_OBJECT_PLUGIN_SELECT_SIMILAR_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class ObjectModule;
   class ObjectModuleSelect;

   class ObjectPluginSelectSimilar :
         public Plugin,
         public MessageObserver {

      public:
         //! \cond
         ObjectPluginSelectSimilar (const PluginInfo &Info, Config &local);
         ~ObjectPluginSelectSimilar ();

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
         // ObjectPluginSelectSimilar Interface
         void _init (Config &local);

         Log _log;

         Message _selectSimilarMsg;
         Message _selectExactMsg;

         ObjectModule *_objMod;
         ObjectModuleSelect *_select;
         //! \endcond

      private:
         ObjectPluginSelectSimilar ();
         ObjectPluginSelectSimilar (const ObjectPluginSelectSimilar &);
         ObjectPluginSelectSimilar &operator= (const ObjectPluginSelectSimilar &);

   };
};

#endif // DMZ_OBJECT_PLUGIN_SELECT_SIMILAR_DOT_H
