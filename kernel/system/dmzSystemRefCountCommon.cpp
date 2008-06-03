#include <dmzSystemRefCount.h>
#include <dmzSystemMutex.h>
#include <dmzTypesBase.h>


struct dmz::RefCount::State {

   Mutex mutex;
   Int32 value;
   State () : value (1) {;}
};

dmz::RefCount::RefCount () : _state (*(new State)) {;}


dmz::RefCount::~RefCount () { delete &_state; }


dmz::Int32
dmz::RefCount::ref () {

   _state.mutex.lock ();
      _state.value++;
      const Int32 Result (_state.value);
   _state.mutex.unlock ();

   return Result;
}


dmz::Int32
dmz::RefCount::unref () {

   _state.mutex.lock ();
      _state.value--;
      const Int32  ReturnCount (_state.value);
   _state.mutex.unlock ();

   if (ReturnCount == 0) { ref_count_is_zero (); }

   return ReturnCount;
}
