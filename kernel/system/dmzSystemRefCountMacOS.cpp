#include <dmzSystemRefCount.h>
#include <dmzTypesBase.h>
#include <libkern/OSAtomic.h>

/*!

\class dmz::RefCount
\ingroup System
\brief Thread safe reference counter.

\class dmz::RefCountDeleteOnZero
\ingroup System
\brief Self deleting thread safe reference counter.

\fn dmz::RefCountDeleteOnZero::_ref_count_is_zero ()
\brief Deletes class when reference count reaches zero.
\details Implements the pure virtual function found in dmz::RefCount base class.
Function is invoked by base class when reference count reaches zero.

*/


struct dmz::RefCount::State {

   int32_t value;
   State () : value (1) {;}
};

//! Base constructor.
dmz::RefCount::RefCount () : _state (*(new State)) {;}


//! Destructor.
dmz::RefCount::~RefCount () { delete &_state; }


/*!

\brief Increments reference count by one.
\details This call is thread safe.
\return Returns current reference count.

*/
dmz::Int32
dmz::RefCount::ref () {

   return dmz::Int32 (OSAtomicIncrement32Barrier (&(_state.value)));
}


/*!

\brief Decrements reference count by one.
\details This call is thread safe. If the reference count reaches zero. The pure
virtual function dmz::RefCount::_ref_count_is_zero is invoked.
\return Returns current reference count.

*/
dmz::Int32
dmz::RefCount::unref () {

   const int32_t ReturnCount (OSAtomicDecrement32Barrier (&(_state.value)));
   if (ReturnCount == 0) { _ref_count_is_zero (); }

   return ReturnCount;
}

/*!

\fn dmz::RefCount::_ref_count_is_zero ()
\brief Pure virtual function for derived classes to implement. When the reference count
reaches zero, the function is invoked.

*/
