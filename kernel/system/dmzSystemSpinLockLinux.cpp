#include <dmzSystem.h>
#include <dmzSystemSpinLock.h>
#include <atomic_ops.h>


struct dmz::SpinLock::State {

   AO_t count;
   State () : count (0) {;}
};


dmz::SpinLock::SpinLock () : _state (*(new State)) {;}


dmz::SpinLock::~SpinLock () { delete &_state; }


dmz::Boolean
dmz::SpinLock::try_lock () {

   return AO_compare_and_swap (&(_state.count), 0, 1) == 1;
}


void
dmz::SpinLock::lock () {

   while (AO_compare_and_swap (&(_state.count), 0, 1) != 1) { sleep (0.0); }
}


void
dmz::SpinLock::unlock () {

   AO_compare_and_swap (&(_state.count), 1, 0);
}

