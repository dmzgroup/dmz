#include <dmzTypesHandleContainer.h>
#include <dmzTypesHashTableHandleTemplate.h>

/*!

\class dmz::HandleContainer
\ingroup Types
\brief Container class for storing Handles.

*/

struct dmz::HandleContainer::State {

   HashTableHandleIterator it;
   HashTableHandle table;
   ~State () { table.clear (); }
};


//! Constructor.
dmz::HandleContainer::HandleContainer () : _state (*(new State)) {;}


//! Copy Constructor.
dmz::HandleContainer::HandleContainer (
      const HandleContainer &Container) : _state (*(new State)) { *this = Container; }


//! Destructor.
dmz::HandleContainer::~HandleContainer () { delete &_state; }


//! Assignment operator.
dmz::HandleContainer &
dmz::HandleContainer::operator= (const HandleContainer &Container) {

   _state.table.clear ();

   HashTableHandleIterator it;

   void *ptr = Container._state.table.get_next (it);

   while (ptr) {

      _state.table.store (it.get_hash_key (), (void *)this);
      ptr = Container._state.table.get_next (it);
   }

   return *this;
}


//! Clears container.
void
dmz::HandleContainer::clear () { _state.table.clear (); }


//! Returns number of unique Handles stored in container.
dmz::Int32
dmz::HandleContainer::get_count () const { return _state.table.get_count (); }



/*!

\brief Test if Handle is stored in container.
\param[in] Value Handle to be tested.
\return Returns dmz::True if the handle is stored in the container.

*/
dmz::Boolean
dmz::HandleContainer::contains (const Handle Value) {

   return _state.table.lookup (Value) != 0;
}


/*!

\brief Adds Handle to container.
\param[in] Value Handle to be added.
\return Returns dmz::True if the \a Handle was added. Returns dmz::False if the
\a Handle was not added or it is already in the container.

*/
dmz::Boolean
dmz::HandleContainer::add_handle (const Handle Value) {

   Boolean result (False);

   result = _state.table.store (Value, (void *)this);

   return result;
}


/*!

\brief Remove handle from container.
\param[in] Value Handle to remove from container.
\return Returns dmz::True if the \a Handle was removed.

*/
dmz::Boolean
dmz::HandleContainer::remove_handle (const Handle Value) {

   return (_state.table.remove (Value) != 0);
}


/*!

\brief Gets first Handle in the container.
\return Returns First Handle in the container. Returns zero if the container is
empty.

*/
dmz::Handle
dmz::HandleContainer::get_first () const {

   Handle result (0);

   _state.it.reset ();

   if (_state.table.get_next (_state.it) != 0) { result = _state.it.get_hash_key (); }

   return result;
}


/*!

\brief Gets next Handle in the container.
\return Returns the next Handle in the container. Returns zero if all Handles
have been returned.

*/
dmz::Handle
dmz::HandleContainer::get_next () const {

   Handle result (0);

   if (_state.table.get_next (_state.it) != 0) { result = _state.it.get_hash_key (); }

   return result;
}

