#include <dmzTypesStringContainer.h>
#include <dmzTypesHashTableStringTemplate.h>

/*!

\class dmz::StringContainer
\ingroup Types
\brief Container class for storing unique Strings.

*/

struct dmz::StringContainer::State {

   HashTableStringIterator it;
   HashTableString table;
   ~State () { table.clear (); }
};


//! Constructor.
dmz::StringContainer::StringContainer () : _state (*(new State)) {;}


//! Copy Constructor.
dmz::StringContainer::StringContainer (
      const StringContainer &Container) : _state (*(new State)) { *this = Container; }


//! Destructor.
dmz::StringContainer::~StringContainer () { delete &_state; }


//! Assignment operator.
dmz::StringContainer &
dmz::StringContainer::operator= (const StringContainer &Container) {

   _state.table.clear ();

   HashTableStringIterator it;

   void *ptr = Container._state.table.get_next (it);

   while (ptr) {

      _state.table.store (it.get_hash_key (), (void *)this);
      ptr = Container._state.table.get_next (it);
   }

   return *this;
}


//! Clears container.
void
dmz::StringContainer::clear () { _state.table.clear (); }


//! Returns number of unique Strings stored in container.
dmz::Int32
dmz::StringContainer::get_count () const { return _state.table.get_count (); }



/*!

\brief Test if String is stored in container.
\param[in] Value String to be tested.
\return Returns dmz::True if the handle is stored in the container.

*/
dmz::Boolean
dmz::StringContainer::contains (const String &Value) {

   return _state.table.lookup (Value) != 0;
}


/*!

\brief Adds String to container.
\param[in] Value String to be added.
\return Returns dmz::True if the \a String was added. Returns dmz::False if the
\a String was not added or it is already in the container.

*/
dmz::Boolean
dmz::StringContainer::add_string (const String &Value) {

   Boolean result (False);

   result = _state.table.store (Value, (void *)this);

   return result;
}


/*!

\brief Remove handle from container.
\param[in] Value String to remove from container.
\return Returns dmz::True if the \a String was removed.

*/
dmz::Boolean
dmz::StringContainer::remove_string (const String &Value) {

   return (_state.table.remove (Value) != 0);
}


/*!

\brief Gets first String in the container.
\param[out] value String use to return the first String in the container.
\return Returns dmz::True if the first string is returned. Returns dmz::False if the
container is empty.

*/
dmz::Boolean
dmz::StringContainer::get_first (String &value) const {

   _state.it.reset ();

   return get_next (value);
}


/*!

\brief Gets next String in the container.
\param[out] value String use to return the next String in the container.
\return Returns dmz::True if the next string is returned. Returns dmz::False if all
the Strings have been returned.

*/
dmz::Boolean
dmz::StringContainer::get_next (String &value) const {

   Boolean result (False);

   if (_state.table.get_next (_state.it) != 0) {

      result = True;
      value = _state.it.get_hash_key ();
   }

   return result;
}


/*!

\brief Gets previous String in the container.
\param[out] value String use to return the previous String in the container.
\return Returns dmz::True if the previous string is returned. Returns dmz::False if all
the Strings have been returned.

*/
dmz::Boolean
dmz::StringContainer::get_prev (String &value) const {

   Boolean result (False);

   if (_state.table.get_next (_state.it, True) != 0) {

      result = True;
      value = _state.it.get_hash_key ();
   }

   return result;
}


/*!

\brief Gets last String in the container.
\param[out] value String use to return the last String in the container.
\return Returns dmz::True if the last string is returned. Returns dmz::False if the
container is empty.

*/
dmz::Boolean
dmz::StringContainer::get_last (String &value) const {

   Boolean result (False);

   _state.it.reset ();

   if (_state.table.get_next (_state.it, True) != 0) {

      result = True;
      value = _state.it.get_hash_key ();
   }

   return result;
}

