#include "dmzRuntimeContextDefinitions.h"

//! Constructor.
dmz::RuntimeContextDefinitions::RuntimeContextDefinitions () :
      namedHandleTable (&namedHandleLock),
      namedHandleNameTable (&namedHandleNameLock),
      eventHandleTable (&eventHandleLock),
      eventNameTable (&eventNameLock),
      objectHandleTable (&objectHandleLock),
      objectNameTable (&objectNameLock),
      maskShift (0),
      maskTable (&maskLock) {;}


//! Destructor.
dmz::RuntimeContextDefinitions::~RuntimeContextDefinitions () {

   namedHandleTable.empty ();
   namedHandleNameTable.empty ();
   eventHandleTable.clear ();
   eventNameTable.empty ();
   objectHandleTable.clear ();
   objectNameTable.empty ();
   maskTable.empty ();
   uniqueNameTable.clear ();
}

