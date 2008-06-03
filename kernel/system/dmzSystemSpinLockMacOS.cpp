#include <dmzSystem.h>
#include <dmzSystemSpinLock.h>
#include <libkern/OSAtomic.h>

/*!

\class dmz::SpinLock
\ingroup System
\brief Simple spin lock class.
\details A spin lock will put the thread into busy wait wile trying to obtain the lock.
Spin locks should only be used in cases where resource contention between threads is
unlikely.

*/

struct dmz::SpinLock::State {

   int32_t count;
   State () : count (0) {;}
};


/*!

\brief Base constructor.
\details Created with lock unlocked.

*/
dmz::SpinLock::SpinLock () : _state (*(new State)) {;}


//! Destructor.
dmz::SpinLock::~SpinLock () { delete &_state; }


/*!

\brief Attempts to lock SpinLock.
\details This function is non-blocking. If the SpinLock can not be obtained it
immediately returns with a result of dmz::False.
\return Returns dmz::True if the SpinLock is successfully obtained.

*/
dmz::Boolean
dmz::SpinLock::try_lock () {

   return OSAtomicCompareAndSwap32Barrier (0, 1, &(_state.count));
}


/*!

\brief Acquires spin lock.
\details Function will block until spin lock is acquired.
\note This will put the thread in a busy wait.

*/
void
dmz::SpinLock::lock () {

   while (!OSAtomicCompareAndSwap32Barrier (0, 1, &(_state.count))) { sleep (0.0); }
}


//! Releases spin lock.
void
dmz::SpinLock::unlock () {

   OSAtomicCompareAndSwap32Barrier (1, 0, &(_state.count));
}

