#include <dmzSystemMutex.h>

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

/*!

\class dmz::Mutex
\ingroup System
\brief Mutex class.
\details Provides a platform independent interface to the system's native mutex.

*/

struct dmz::Mutex::State {

   pthread_mutex_t mutex;
};


/*!

\brief Base Constructor.
\details Mutex is created unlocked.

*/
dmz::Mutex::Mutex () : _state (*(new State)) {

   if (pthread_mutex_init (&_state.mutex, 0)) {

      // The init failed!
   }
}


//! Destructor.
dmz::Mutex::~Mutex () {

   pthread_mutex_destroy (&_state.mutex);
   delete &_state;
}


/*!

\brief Attempts to lock Mutex.
\details This function is non-blocking. If the Mutex can not be obtained it immediately
returns with a result of dmz::False.
\return Returns dmz::True if the Mutex is successfully obtained.

*/
dmz::Boolean
dmz::Mutex::try_lock () {

   return (pthread_mutex_trylock (&_state.mutex) == 0) ? True : False;
}


/*!

\brief Acquires Mutex.
\details Function will block until Mutex is acquired.

*/
void
dmz::Mutex::lock () { pthread_mutex_lock (&_state.mutex); }


//! Releases Mutex.
void
dmz::Mutex::unlock () { pthread_mutex_unlock (&_state.mutex); }

