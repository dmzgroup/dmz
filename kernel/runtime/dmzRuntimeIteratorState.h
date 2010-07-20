#ifndef DMZ_RUNTIME_ITERATOR_STATE_DOT_H
#define DMZ_RUNTIME_ITERATOR_STATE_DOT_H

#include <dmzRuntimeIterator.h>
#include <dmzTypesHashTableHandleTemplate.h>


struct dmz::RuntimeIterator::State {

   HashTableHandleIterator it;
};


inline
dmz::RuntimeIterator::RuntimeIterator () : state (*(new State)) {;}


inline
dmz::RuntimeIterator::~RuntimeIterator () { delete &state; }


inline void
dmz::RuntimeIterator::reset () { state.it.reset (); }

#endif // DMZ_RUNTIME_ITERATOR_STATE_DOT_H
