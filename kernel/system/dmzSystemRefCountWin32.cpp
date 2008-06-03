#include <dmzSystemRefCount.h>
#include <dmzTypesBase.h>
#include <windows.h>

struct dmz::RefCount::State {

   LONG value;
   State () : value (1) {;}
};

dmz::RefCount::RefCount () : _state (*(new State)) {;}


dmz::RefCount::~RefCount () { delete &_state; }


dmz::Int32
dmz::RefCount::ref () {

   return dmz::Int32 (InterlockedIncrement (&(_state.value)));
}


dmz::Int32
dmz::RefCount::unref () {

   const LONG ReturnCount (InterlockedDecrement (&(_state.value)));
   if (ReturnCount == 0) { ref_count_is_zero (); }

   return Int32 (ReturnCount);
}
