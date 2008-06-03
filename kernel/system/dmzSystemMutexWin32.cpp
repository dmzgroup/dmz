#include <dmzSystemMutex.h>

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#include <windows.h>

struct dmz::Mutex::State {

   CRITICAL_SECTION cs;
};


dmz::Mutex::Mutex () : _state (*(new State)) {

   InitializeCriticalSection (&(_state.cs));
}


dmz::Mutex::~Mutex () {

   DeleteCriticalSection (&(_state.cs));
   delete &_state;
}


dmz::Boolean
dmz::Mutex::try_lock () {

   return TryEnterCriticalSection (&_state.cs) ? True : False;
}


void
dmz::Mutex::lock () { EnterCriticalSection (&(_state.cs)); }


void
dmz::Mutex::unlock () { LeaveCriticalSection (&(_state.cs)); }

