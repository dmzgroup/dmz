#include <dmzSystemRefCount.h>
#include <dmzTypesBase.h>
#include <atomic_ops.h>

struct dmz::RefCount::State {

   AO_t value;
   State () : value (1) {;}
};

dmz::RefCount::RefCount () : _state (*(new State)) {;}


dmz::RefCount::~RefCount () { delete &_state; }


dmz::Int32
dmz::RefCount::ref () {

   return dmz::Int32 (AO_fetch_and_add1 (&(_state.value)));
}


dmz::Int32
dmz::RefCount::unref () {

   const Int32 ReturnCount (AO_fetch_and_sub1 (&(_state.value)));
   if (ReturnCount == 0) { _ref_count_is_zero (); }

   return ReturnCount;
}
