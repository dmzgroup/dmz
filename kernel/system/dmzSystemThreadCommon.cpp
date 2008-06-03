#include <dmzSystemThread.h>

/*!

\file dmzSystemThread.h
\brief Contains classes and functions related to threading.

\class dmz::ThreadFunction
\ingroup System
\brief Base class that threadable classes should be derived from.

*/
namespace {

   struct cleanupStruct {

      cleanupStruct *next;
      cleanupStruct *prev;
      dmz::Boolean aboutToDelete;

      dmz::ThreadCleanup *tc;

      cleanupStruct (dmz::ThreadCleanup *theTC) :
         next (0),
         prev (0),
         aboutToDelete (dmz::False),
         tc (theTC) {;}

      ~cleanupStruct () {

         aboutToDelete = dmz::True;
         if (tc) { delete tc; tc = 0; }
         if (next) { delete next; next = 0; }
         prev = 0;
      }
   };
};


/*!

\class dmz::ThreadCleanup
\ingroup System
\brief Class that auto deletes when associated thread exits.
\note Always create the derived object in the thread where the object will be used.

*/
struct dmz::ThreadCleanup::ThreadCleanupState {

   cleanupStruct *cs;
   ThreadCleanupState () : cs (0) {;}
};



static dmz::ThreadStorageTemplate<cleanupStruct> &
local_get_thread_storage () {

   static dmz::ThreadStorageTemplate<cleanupStruct> storage;
   return storage;
}


//! Base constructor.
dmz::ThreadCleanup::ThreadCleanup () : _threadCleanupState (*(new ThreadCleanupState)) {

   ThreadStorageTemplate<cleanupStruct> &storage = local_get_thread_storage ();
   cleanupStruct *head = storage.get_data ();
   _threadCleanupState.cs = new cleanupStruct (this);
   _threadCleanupState.cs->next = head;
   if (head) { head->prev = _threadCleanupState.cs->next; }
   storage.set_data (_threadCleanupState.cs);
}


//! Destructor.
dmz::ThreadCleanup::~ThreadCleanup () {

   cleanupStruct *cs = _threadCleanupState.cs;

   if (cs && !cs->aboutToDelete) {

      if (cs->prev) {

         cs->prev->next = cs->next;
         if (cs->next) { cs->next->prev = cs->prev; }
      }
      else {

         ThreadStorageTemplate<cleanupStruct> &storage = local_get_thread_storage ();

         cleanupStruct *head = storage.get_data ();
         if (head == cs) {

            head = head->next;
            if (head) { head->prev = 0; }
         }
      }

      cs->next = 0;
      cs->prev = 0;
      cs->tc = 0;
      delete cs;
   }

   delete &_threadCleanupState;
}

/*!

\ingroup System
\brief Deletes thread specific classes.
\details Defined in dmzSystemThread.h.
\note This function only needs to be called if you are using code in a thread
not created by dmz::create_thread and you do not call dmz::thread_exit.

*/
void
dmz::cleanup_thread () {

   ThreadStorageTemplate<cleanupStruct> &storage = local_get_thread_storage ();
   cleanupStruct *head = storage.get_data ();
   if (head) { delete head; head = 0; }
   storage.set_data (0);
}
