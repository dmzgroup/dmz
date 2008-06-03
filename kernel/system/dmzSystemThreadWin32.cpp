#include <dmzSystemThread.h>
#include <windows.h>
#include <process.h>

struct dmz::ThreadStorage::State {

   DWORD handle;
   State () : handle (0) {;}
};


static unsigned int WINAPI
local_thread_function (LPVOID data) {

   dmz::ThreadFunction *tf = (dmz::ThreadFunction *)data;

   if (tf) { tf->run_thread_function (); }

   dmz::cleanup_thread ();

   return unsigned int (1);
}


dmz::ThreadStorage::ThreadStorage () : _state (*(new State)) {

   _state.handle = TlsAlloc ();;
}


dmz::ThreadStorage::~ThreadStorage ()  {

   TlsFree (_state.handle);
   delete &_state;
}


void
dmz::ThreadStorage::set_data (void *data) { TlsSetValue (_state.handle, data); }


void *
dmz::ThreadStorage::get_data () { return TlsGetValue (_state.handle); }


dmz::Boolean
dmz::create_thread (ThreadFunction &tf) {

   unsigned int id (0);
   return  _beginthreadex (0, 0, local_thread_function, LPVOID (&tf), 0, &id) != NULL;
}


void
dmz::thread_exit () {

   cleanup_thread ();
   _endthreadex (0);
}

