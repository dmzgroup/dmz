#include "dmzRuntimeContextDefinitions.h"

//! Constructor.
dmz::RuntimeContextDefinitions::RuntimeContextDefinitions () :
      namedHandleTable (&namedHandleLock),
      namedHandleNameTable (&namedHandleNameLock),
      objectHandleTable (&objectHandleLock),
      objectNameTable (&objectNameLock),
      eventHandleTable (&eventHandleLock),
      eventNameTable (&eventNameLock),
      maskShift (0),
      maskTable (&maskLock),
      messageHandleTable (&messageHandleLock),
      messageNameTable (&messageNameLock) {;}


//! Destructor.
dmz::RuntimeContextDefinitions::~RuntimeContextDefinitions () {

   namedHandleTable.empty ();
   namedHandleNameTable.empty ();
   objectHandleTable.clear ();
   objectNameTable.empty ();
   eventHandleTable.clear ();
   eventNameTable.empty ();
   maskTable.empty ();
   messageHandleTable.clear ();
   messageNameTable.empty ();
   uniqueNameTable.clear ();
}

