#ifndef DMZ_RUNTIME_MESSAGE_TYPE_CONTEXT_DOT_H
#define DMZ_RUNTIME_MESSAGE_TYPE_CONTEXT_DOT_H

#include "dmzRuntimeContext.h"
#include "dmzRuntimeContextMessaging.h"
#include <dmzRuntimeHandle.h>
#include <dmzRuntimeMessaging.h>
#include <dmzSystemRefCount.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class HandleAllocator;
   class RuntimeContext;

   class MessageContext : public RefCountDeleteOnZero {

      public:
         MessageContext (
            const String &TheName,
            RuntimeContext *theContext,
            RuntimeContextMessaging *msgContext,
            MessageContext *theParent);

         virtual ~MessageContext ();

         const RuntimeHandle Handle;
         const String Name;

         Data *monostate;

         Boolean inSend;

         MessageContext *parent;
         RuntimeContextMessaging *dispatch;

         HashTableHandleTemplate<MessageObserver> obsTable;
   };
};

#endif // DMZ_RUNTIME_MESSAGE_TYPE_CONTEXT_DOT_H
