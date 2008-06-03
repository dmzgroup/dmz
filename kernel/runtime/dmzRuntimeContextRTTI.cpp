#include "dmzRuntimeContextRTTI.h"

//! Constructor.
dmz::RuntimeContextRTTI::RuntimeContextRTTI () {;}


//! Destructor.
dmz::RuntimeContextRTTI::~RuntimeContextRTTI () {

   _lock.lock ();
   _table.empty ();
   _countTable.empty ();
   _namedTable.clear ();
   _lock.unlock ();
}


//! Stores named interface.
dmz::Boolean
dmz::RuntimeContextRTTI::store_named_interface (
      const String &Name,
      void *ptr) {

   Boolean result (False);

   _lock.lock ();

   result = _namedTable.store (Name, ptr);

   _lock.unlock ();

   return result;
}


//! Looks up named interface.
void *
dmz::RuntimeContextRTTI::lookup_named_interface (const String &Name) {

   void *result (0);

   _lock.lock ();

   result = _namedTable.lookup (Name);

   _lock.unlock ();

   return result;
}


//! Removes named interface.
void *
dmz::RuntimeContextRTTI::remove_named_interface (const String &Name) {

   void *result (0);

   _lock.lock ();

   result = _namedTable.remove (Name);

   _lock.unlock ();

   return result;
}


//! Stores interface.
dmz::Boolean
dmz::RuntimeContextRTTI::store_interface (
      const String &Name,
      const Handle InterfaceHandle,
      void *ptr) {

   Boolean result (False);

   _lock.lock ();

   HashTableHandle *store = _table.lookup (Name);

   if (!store) {

      store = new HashTableHandle;
      if (!_table.store (Name, store)) { delete store; store = 0; }
   }

   if (store) { result = store->store (InterfaceHandle, ptr); }

   if (result) {

      Int32 *count (_countTable.lookup (InterfaceHandle));

      if (!count) {

         count = new Int32 (0);

         if (count) {

            if (!_countTable.store (InterfaceHandle, count)) { delete count; count = 0; }
         }
      }

      if (count) { *count += 1; }
   }

   _lock.unlock ();

   return result;
}


//! Looks up interface.
void *
dmz::RuntimeContextRTTI::lookup_interface (
      const String &Name,
      const Handle InterfaceHandle) {

   void *result (0);

   _lock.lock ();

   HashTableHandle *store = _table.lookup (Name);
   if (store) { result = store->lookup (InterfaceHandle); }

   _lock.unlock ();

   return result;
}


//! Validates interface handle.
dmz::Boolean
dmz::RuntimeContextRTTI::is_valid (const Handle InterfaceHandle) {

   _lock.lock ();

   Int32 *countPtr (_countTable.lookup (InterfaceHandle));

   Int32 count (countPtr ? *countPtr : 0);
   
   _lock.unlock ();

   return count > 0;
}


//! Removes interface.
void *
dmz::RuntimeContextRTTI::remove_interface (
      const String &Name,
      const Handle InterfaceHandle) {

   void *result (0);

   _lock.lock ();

   HashTableHandle *store = _table.lookup (Name);

   if (store) {

      result = store->remove (InterfaceHandle);

      if (!store->get_count ()) {

         store = _table.remove (Name);
         if (store) { delete store; store = 0; }
      }

      if (result) {

         Int32 *count (_countTable.lookup (InterfaceHandle));

         if (count) {

            *count -= 1;

            if (*count <= 0) {

               if (_countTable.remove (InterfaceHandle)) { delete count; count = 0; }
            }
         }
      }
   }

   _lock.unlock ();

   return result;
}

