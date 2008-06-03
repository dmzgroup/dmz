#ifndef DMZ_RUNTIME_CONTEXT_RTTI_DOT_H
#define DMZ_RUNTIME_CONTEXT_RTTI_DOT_H

#include <dmzSystemMutex.h>
#include <dmzSystemRefCount.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class RuntimeContextRTTI : public RefCountDeleteOnZero {

      public:
         RuntimeContextRTTI ();

         Boolean store_named_interface (const String &Name, void *ptr);
         void *lookup_named_interface (const String &Name);
         void *remove_named_interface (const String &Name);

         Boolean store_interface (
            const String &Name,
            const Handle InterfaceHandle,
            void *ptr);

         void *lookup_interface (const String &Name, const Handle InterfaceHandle);
         Boolean is_valid (const Handle InterfaceHandle);
         void *remove_interface (const String &Name, const Handle InterfaceHandle);

      protected:
         Mutex _lock; //!< Lock.
         HashTableStringTemplate<HashTableHandle> _table; //!< Table.
         HashTableHandleTemplate<Int32> _countTable; //!< Table.
         HashTableString _namedTable; //!< Table.

      private:
         RuntimeContextRTTI (const RuntimeContextRTTI &);
         RuntimeContextRTTI &operator= (const RuntimeContextRTTI &);
         ~RuntimeContextRTTI ();
   };
};

#endif // DMZ_RUNTIME_CONTEXT_RTTI_DOT_H
