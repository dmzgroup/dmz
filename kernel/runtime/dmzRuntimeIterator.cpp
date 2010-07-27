#include <dmzRuntimeIterator.h>
#include "dmzRuntimeIteratorState.h"
#include <dmzTypesHashTableHandleTemplate.h>

/*!

\class dmz::RuntimeIterator
\ingroup Runtime
\brief Generic runtime iterator.

*/

//! Constructor.
dmz::RuntimeIterator::RuntimeIterator () : state (*(new State)) {;}


//! Destructor.
dmz::RuntimeIterator::~RuntimeIterator () { delete &state; }


//! Resets the iterator.
void
dmz::RuntimeIterator::reset () { state.it.reset (); }

