#include <dmzSystem.h>
#include <dmzSystemSpinLock.h>
#include <windows.h>

struct dmz::SpinLock::State {

   LONG value;
   State () : value (0) {;}
};


dmz::SpinLock::SpinLock () : _state (*(new State)) {;}


dmz::SpinLock::~SpinLock () { delete &_state; }


dmz::Boolean
dmz::SpinLock::try_lock () {

   const LONG Exchange (1);
   const LONG Comparand (0);

   return Comparand == InterlockedCompareExchange (&(_state.value), Exchange, Comparand);
}


void
dmz::SpinLock::lock () {

   const LONG Exchange (1);
   const LONG Comparand (0);

   while (Comparand !=
         InterlockedCompareExchange (&(_state.value), Exchange, Comparand)) {

      sleep (0.0);
   }
}


void
dmz::SpinLock::unlock () {

   const LONG Exchange (0);
   const LONG Comparand (1);

   InterlockedCompareExchange (&(_state.value), Exchange, Comparand);
}

