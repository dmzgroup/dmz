#ifndef DMZ_RUNTIME_CONTEXT_DEFINITIONS_DOT_H
#define DMZ_RUNTIME_CONTEXT_DEFINITIONS_DOT_H

#include "dmzRuntimeConfigContext.h"
#include <dmzRuntimeData.h>
#include <dmzRuntimeDefinitionsObserver.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeHandle.h>
#include <dmzRuntimeMessaging.h>
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

         Message create_message (
            const String &Name,
            const String &ParentName,
            RuntimeContext *context,
            RuntimeContextMessaging *messagingContext);

         void define_named_handle (const Handle TheHandle, const String &Name);
         void define_state (const Mask &TheState, const String &Name);
         void define_object_type (const ObjectType &Type);
         void define_event_type (const EventType &Type);
         void define_message (const Message &Type);

         HashTableHandleTemplate<DefinitionsObserver> handleObsTable;

         ConfigContextLock namedHandleLock; //!< Lock.
         HashTableStringTemplate<RuntimeHandle> namedHandleTable;

         ConfigContextLock namedHandleNameLock; //!< Lock.
         HashTableHandleTemplate<String> namedHandleNameTable;

         HashTableHandleTemplate<DefinitionsObserver> maskObsTable;

         SpinLock maskShiftLock; //!< Lock
         Int32 maskShift; //!< Shift value.

         ConfigContextLock maskLock; //!< Lock
         HashTableStringTemplate<Mask> maskTable; //!< Table.

         HashTableHandleTemplate<DefinitionsObserver> objectObsTable;

         ConfigContextLock objectHandleLock; //!< Lock
         HashTableHandleTemplate<ObjectType> objectHandleTable; //!< Table.

         ConfigContextLock objectNameLock; //!< Lock
         HashTableStringTemplate<ObjectType> objectNameTable; //!< Table.

         HashTableHandleTemplate<DefinitionsObserver> eventObsTable;

         ConfigContextLock eventHandleLock; //!< Lock.
         HashTableHandleTemplate<EventType> eventHandleTable; //!< Table.

         ConfigContextLock eventNameLock; //!< Lock.
         HashTableStringTemplate<EventType> eventNameTable; //!< Table.

         HashTableHandleTemplate<DefinitionsObserver> messageObsTable;

         ConfigContextLock messageHandleLock; //!< Lock.
         HashTableHandleTemplate<Message> messageHandleTable; //!< Table.

         ConfigContextLock messageNameLock; //!< Lock.
         HashTableStringTemplate<Message> messageNameTable; //!< Table.

         Mutex uniqueNameLock; //!< Lock
         HashTableString uniqueNameTable; //!< Table.

      private:
         ~RuntimeContextDefinitions ();
   };
};

inline void
dmz::RuntimeContextDefinitions::define_named_handle (
      const Handle TheHandle,
      const String &Name) {

   HashTableHandleIterator it;
   DefinitionsObserver *obs (0);

   while (handleObsTable.get_next (it, obs)) {

      obs->define_named_handle (TheHandle, Name);
   }
}


inline void
dmz::RuntimeContextDefinitions::define_state (const Mask &TheState, const String &Name) {

   HashTableHandleIterator it;
   DefinitionsObserver *obs (0);

   while (maskObsTable.get_next (it, obs)) { obs->define_state (TheState, Name); }
}


inline void
dmz::RuntimeContextDefinitions::define_object_type (const ObjectType &Type) {

   HashTableHandleIterator it;
   DefinitionsObserver *obs (0);

   while (objectObsTable.get_next (it, obs)) { obs->define_object_type (Type); }
}


inline void
dmz::RuntimeContextDefinitions::define_event_type (const EventType &Type) {

   HashTableHandleIterator it;
   DefinitionsObserver *obs (0);

   while (eventObsTable.get_next (it, obs)) { obs->define_event_type (Type); }
}


inline void
dmz::RuntimeContextDefinitions::define_message (const Message &Type) {

   HashTableHandleIterator it;
   DefinitionsObserver *obs (0);

   while (messageObsTable.get_next (it, obs)) { obs->define_message (Type); }
}

#endif // DMZ_RUNTIME_CONTEXT_DEFINITIONS_DOT_H

