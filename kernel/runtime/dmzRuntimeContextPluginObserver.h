#ifndef DMZ_RUNTIME_CONTEXT_PLUGIN_OBSERVER_DOT_H
#define DMZ_RUNTIME_CONTEXT_PLUGIN_OBSERVER_DOT_H

#include <dmzRuntimeModule.h>
#include <dmzRuntimePluginObserver.h>
#include <dmzSystemRefCount.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

const char PluginObserverActivateMessageName[] =
   "DMZ_Runtime_Internal_Message_Plugin_Observer_Activate";

class RuntimeContextPluginObserver : public RefCountDeleteOnZero {

   public:
      HashTableHandleTemplate<PluginObserver> obsTable;
      HashTableHandleTemplate<PluginObserver> obsActiveTable;
      HashTableHandleTemplate<RuntimeModule> moduleTable;

      virtual ~RuntimeContextPluginObserver () {;}
};

};

#endif // DMZ_RUNTIME_CONTEXT_PLUGIN_OBSERVER_DOT_H
