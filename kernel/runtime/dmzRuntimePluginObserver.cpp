#include "dmzRuntimeContext.h"
#include "dmzRuntimeContextMessaging.h"
#include "dmzRuntimeContextPluginObserver.h"
#include <dmzRuntimeDataConverterTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePluginObserver.h>

struct dmz::PluginObserver::State {

   const Handle ObsHandle;
   RuntimeContextPluginObserver *context;

   State (const PluginInfo &Info) : ObsHandle (Info.get_handle ()), context (0) {

      RuntimeContext *rt = Info.get_context ();

      if (rt) {

         context = rt->get_plugin_observer_context ();

         if (context) {

            context->ref ();

            RuntimeContextMessaging *msgContext = rt->get_messaging_context ();

            if (msgContext && context->moduleTable.get_count ()) {

               Message msg;

               Definitions defs (rt);

               if (defs.create_message (PluginObserverActivateMessageName, msg)) {

                  DataConverterHandle convert (rt);
                  Data out = convert.to_data (ObsHandle);

                  msgContext->send_delayed (msg, 0, &out);
               }
            }
         }
      }
   }

   ~State () {

      if (context) {

         context->obsTable.remove (ObsHandle); context->unref ();
         context->obsActiveTable.remove (ObsHandle);
         context = 0;
      }
   }
};


dmz::PluginObserver::PluginObserver (const PluginInfo &Info) :
      __state (*(new State (Info))) {

   if (__state.context) {

      __state.context->obsTable.store (__state.ObsHandle, this);
   }
}


dmz::PluginObserver::~PluginObserver () { delete &__state; }


dmz::RuntimeModule *
dmz::PluginObserver::lookup_runtime_module (const Handle RuntimeModuleHandle) {

   RuntimeModule *result (0);

   if (__state.context) {

      result = __state.context->moduleTable.lookup (RuntimeModuleHandle);
   }

   return result;
}

