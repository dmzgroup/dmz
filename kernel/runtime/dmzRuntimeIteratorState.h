#ifndef DMZ_RUNTIME_ITERATOR_STATE_DOT_H
#define DMZ_RUNTIME_ITERATOR_STATE_DOT_H

#include <dmzRuntimeIterator.h>
#include <dmzTypesHashTableHandleTemplate.h>

/*!

\class dmz::RuntimeIterator
\ingroup Runtime
\brief Generic runtime iterator.

*/


//! \cond
struct dmz::RuntimeIterator::State {

   HashTableHandleIterator it;
};
//! \endcond


//! Constructor.
inline
dmz::RuntimeIterator::RuntimeIterator () : state (*(new State)) {;}


//! Destructor.
inline
dmz::RuntimeIterator::~RuntimeIterator () { delete &state; }


//! Resets the iterator.
inline void
dmz::RuntimeIterator::reset () { state.it.reset (); }

#endif // DMZ_RUNTIME_ITERATOR_STATE_DOT_H
