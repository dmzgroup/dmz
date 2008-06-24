#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeInit.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzSystem.h>
#include <dmzTest.h>
#include <dmzTypesBase.h>

using namespace dmz;

class msgObsTest : public MessageObserver {

   public:
      Message type;
      UInt32 sendHandle;
      UInt32 targetHandle;
      Data data;

      msgObsTest (const String &Name, RuntimeContext *context) :
            MessageObserver (0, Name, context),
            sendHandle (0),
            targetHandle (0) {;}

      virtual void receive_message (
            const Message &Type,
            const UInt32 MessageSendHandle,
            const UInt32 TargetObserverHandle,
            const Data *InData,
            Data *outData) {

         type = Type;
         sendHandle = MessageSendHandle;
         targetHandle = TargetObserverHandle;
         data.clear (); if (InData) { data = *InData; }
      }
};


int
main (int argc, char *argv[]) {

   Test test ("dmzRuntimeMessagingTest", argc, argv);
   // RuntimeContext *context (test.rt.get_context ());

   Config runtimeConfig;

   if (test.validate (
         "Initializing runtime",
         test.config.lookup_config ("dmz.runtime", runtimeConfig))) {

      runtime_init (runtimeConfig, test.rt.get_context (), &(test.log));
   }

   return test.result ();
}
