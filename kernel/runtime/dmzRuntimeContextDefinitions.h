#ifndef DMZ_RUNTIME_CONTEXT_DEFINITIONS_DOT_H
#define DMZ_RUNTIME_CONTEXT_DEFINITIONS_DOT_H

#include "dmzRuntimeConfigContext.h"
#include <dmzRuntimeData.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeHandle.h>
#include <dmzRuntimeObjectType.h>
#include <dmzSystemRefCount.h>
#include <dmzSystemSpinLock.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesMask.h>

namespace dmz {

   class RuntimeContextDefinitions : public RefCountDeleteOnZero {

      public:
         RuntimeContextDefinitions ();

         ConfigContextLock namedHandleLock; //!< Lock.
         HashTableStringTemplate<RuntimeHandle> namedHandleTable;

         ConfigContextLock namedHandleNameLock; //!< Lock.
         HashTableHandleTemplate<String> namedHandleNameTable;

         ConfigContextLock eventHandleLock; //!< Lock.
         HashTableHandleTemplate<EventType> eventHandleTable; //!< Table.

         ConfigContextLock eventNameLock; //!< Lock.
         HashTableStringTemplate<EventType> eventNameTable; //!< Table.

         ConfigContextLock objectHandleLock; //!< Lock
         HashTableHandleTemplate<ObjectType> objectHandleTable; //!< Table.

         ConfigContextLock objectNameLock; //!< Lock
         HashTableStringTemplate<ObjectType> objectNameTable; //!< Table.

         SpinLock maskShiftLock; //!< Lock
         Int32 maskShift; //!< Shift value.

         ConfigContextLock maskLock; //!< Lock
         HashTableStringTemplate<Mask> maskTable; //!< Table.

         Mutex uniqueNameLock; //!< Lock
         HashTableString uniqueNameTable; //!< Table.

      private:
         ~RuntimeContextDefinitions ();
   };
};

#endif // DMZ_RUNTIME_CONTEXT_DEFINITIONS_DOT_H

