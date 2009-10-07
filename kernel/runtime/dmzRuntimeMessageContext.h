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

         ~MessageContext ();

         const RuntimeHandle Handle;
         const String Name;

         Data *monostate;

         Boolean inSend;

         MessageContext *parent;
         RuntimeContextMessaging *dispatch;

         HashTableHandleTemplate<MessageObserver> obsTable;
   };
};


inline
dmz::MessageContext::MessageContext (
      const String &TheName,
      RuntimeContext *theContext,
      RuntimeContextMessaging *msgContext,
      MessageContext *theParent) :
      Handle (TheName + ".Message", theContext),
      Name (TheName),
      monostate (0),
      inSend (False),
      parent (theParent),
      dispatch (msgContext) {

   if (dispatch) { dispatch->ref (); }
   if (parent) { parent->ref (); }
}


inline
dmz::MessageContext::~MessageContext () {

   obsTable.clear ();
   if (parent) { parent->unref (); parent = 0; }
   if (monostate) { delete monostate; monostate = 0; }
   if (dispatch) { dispatch->unref (); dispatch = 0; }
}

#endif // DMZ_RUNTIME_MESSAGE_TYPE_CONTEXT_DOT_H
