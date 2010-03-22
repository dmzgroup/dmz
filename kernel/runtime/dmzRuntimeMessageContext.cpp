#include "dmzRuntimeMessageContext.h"

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


dmz::MessageContext::~MessageContext () {

   obsTable.clear ();
   if (parent) { parent->unref (); parent = 0; }
   if (monostate) { delete monostate; monostate = 0; }
   if (dispatch) { dispatch->unref (); dispatch = 0; }
}

