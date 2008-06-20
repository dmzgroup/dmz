#include <dmzSystemThread.h>
#include <pthread.h>

/*!

\class dmz::ThreadStorage
\ingroup System
\brief Class providing thread specific storage.
\note Please use dmz::ThreadStorageTemplate

*/

struct dmz::ThreadStorage::State {

   pthread_key_t key;
   State () : key (0) {;}
};


static void *
local_thread_function (void *data) {

   dmz::ThreadFunction *tf = (dmz::ThreadFunction *)data;

   if (tf) { tf->run_thread_function (); }

   dmz::cleanup_thread ();

   return data;
}


//! Default constructor. Creates thread specific storage.
dmz::ThreadStorage::ThreadStorage () : _state (*(new State)) {

   pthread_key_create (&(_state.key), 0);
}


//! Destructor. Deletes thread specific storage.
dmz::ThreadStorage::~ThreadStorage ()  {

   pthread_key_delete (_state.key);
   delete &_state;
}


//! Sets thread specific storage.
void
dmz::ThreadStorage::set_data (void *ptr) { pthread_setspecific (_state.key, ptr); }


//! Returns thread specific storage.
void *
dmz::ThreadStorage::get_data () { return pthread_getspecific (_state.key); }



//! \addtogroup System
//! @{

/*!

\brief Platform independent interface for creating a thread.
\details Defined in dmzSystemThread.h.
\param[in] tf ThreadFunction class to be invoked from the new thread.
\return Returns dmz::True if thread successfully created.

*/
dmz::Boolean
dmz::create_thread (ThreadFunction &tf) {

   pthread_t thread;
   return pthread_create (&thread, 0, local_thread_function, (void *)&tf) == 0;
}


/*!

\brief Causes thread to exit.
\details Defined in dmzSystemThread.h.

*/
void
dmz::thread_exit () {

   cleanup_thread ();
   pthread_exit (0);
}


//! @}

/*!

\fn dmz::ThreadFunction::run_thread_function ().
\brief Pure virtual function invoked in thread created by dmz::create_thread.

*/
