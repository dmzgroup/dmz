#include <dmzRuntimeConfig.h>
#include "dmzRuntimeContext.h"
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeEventType.h>
#include "dmzRuntimeIteratorState.h"
#include "dmzRuntimeTypeContext.h"
#include <dmzTypesHashTableHandle.h>

/*!

\class dmz::EventType
\ingroup Runtime
\brief Class provides interface to event types.
\details Events are temporal in nature and do not persist. Examples of events are:
explosions, collisions, munitions fire, etc.
Event types are hierarchical. An event type may have one parent and multiple children.
Each event type may contain a config context. This config context contains information
specific to the event type. It may contain model definitions for rendering, audio files
to play, damage information, etc.

*/


//! Constructor.
dmz::EventType::EventType () : _context (0) {;}


//! Copy constructor.
dmz::EventType::EventType (const EventType &Type) : _context (0) {

   *this = Type;
}


//! Context constructor. For internal use.
dmz::EventType::EventType (TypeContext *context) : _context (0) {

   set_type_context (context);
}


/*!

\brief Named constructor.
\param[in] Name String containing name of event type.
\param[in] context Pointer to runtime context.
\see dmz::EventType::set_type

*/
dmz::EventType::EventType (const String &Name, RuntimeContext *context) : _context (0) {

   set_type (Name, context);
}


/*!

\brief Handle constructor.
\param[in] TypeHandle Unique event type handle.
\param[in] context Pointer to runtime context.

*/
dmz::EventType::EventType (const Handle TypeHandle, RuntimeContext *context) :
      _context (0) {

   set_type (TypeHandle, context);
}


//! Destructor.
dmz::EventType::~EventType () { if (_context) { _context->unref (); } }


//! Assignment operator.
dmz::EventType &
dmz::EventType::operator= (const EventType &Type) {

   set_type_context (Type._context);
   return *this;
}


//! Relational "equal to" operator.
dmz::Boolean
dmz::EventType::operator== (const EventType &Type) const {

   return Type._context == _context;
}


//! Relational "not equal to" operator.
dmz::Boolean
dmz::EventType::operator!= (const EventType &Type) const {

   return Type._context != _context;
}


//! Logical not operator.
dmz::Boolean
dmz::EventType::operator! () const {

   return _context == 0;
}


/*!

\brief Sets event type by name.
\param[in] Name String containing name of event type.
\param[in] context Pointer to runtime context.
\return Returns dmz::True if the named event type was found.

*/
dmz::Boolean
dmz::EventType::set_type (const String &Name, RuntimeContext *context) {

   Definitions defs (context);

   return defs.lookup_event_type (Name, *this);
}


/*!

\brief Sets event type by handle.
\param[in] TypeHandle Unique event type handle.
\param[in] context Pointer to runtime context.
\return Returns dmz::True if the event type was found.

*/
dmz::Boolean
dmz::EventType::set_type (const Handle TypeHandle, RuntimeContext *context) {

   Definitions defs (context);

   return defs.lookup_event_type (TypeHandle, *this);
}


/*!

\brief Test if event type is a related type.
\param[in] Type EventType to test against.
\return Returns dmz::True if \a Type is the same or a parent.

*/
dmz::Boolean
dmz::EventType::is_of_type (const EventType &Type) const {

   Boolean result (False);

   EventType current (_context);

   if (Type) {

      while (current && !result) {

         if (Type == current) { result = True; }
         else { current.become_parent (); }
      }
   }

   return result;
}


/*!

\brief Test if event type is an exact type.
\param[in] Type EventType to test against.
\return Returns dmz::True if \a Type is the same type.

*/
dmz::Boolean
dmz::EventType::is_of_exact_type (const EventType &Type) const {

   return _context && (Type == *this);
}


//! Gets event type name.
dmz::String
dmz::EventType::get_name () const {

   return _context ? _context->Name : "";
}


/*!

\brief Gets event type handle.
\return Returns event type's unique handle. Returns zero if the EventType is empty.

*/
dmz::Handle
dmz::EventType::get_handle () const {

   return _context ? _context->Handle.get_runtime_handle () : 0;
}


/*!

\brief Gets event type's parent.
\return Returns event types parent. The returned EvenType will be empty if the event type
has no parent.

*/
dmz::EventType
dmz::EventType::get_parent () const {

   EventType result;

   if (_context) {

      _context->lock.lock ();
      result.set_type_context (_context->parent);
      _context->lock.unlock ();
   }

   return result;
}


/*!

\brief Becomes event type's parent.
\details The EventType will be empty if it has no parent.
\return Returns dmz::True if the event type becomes its parent. Returns dmz::False
if the current event type has no parent.

*/
dmz::Boolean
dmz::EventType::become_parent () {

   if (_context) {

      _context->lock.lock ();
      TypeContext *parent (_context->parent);
      if (parent) { parent->ref (); }
      _context->lock.unlock ();

      _context->unref ();
      _context = parent;
   }

   return _context != 0;
}


/*!

\brief Gets first event type child.
\param[in] it RuntimeIterator.
\param[out] type EventType to store first child.
\return Returns dmz::True if the first child is returned. Returns dmz::False if
the event type has no children.

*/
dmz::Boolean
dmz::EventType::get_first_child (RuntimeIterator &it, EventType &type) const {

   Boolean result (False);

   if (_context && (this != &type)) {

      _context->lock.lock ();
      type.set_type_context (_context->table.get_first (it.state.it));
      _context->lock.unlock ();
   }

   if (type) { result = True; }

   return result;
}


/*!

\brief Gets next event type child.
\param[in] it RuntimeIterator.
\param[out] type EventType to store next child.
\return Returns dmz::True if the next child is returned. Returns dmz::False if
the event type has no more children to return.

*/
dmz::Boolean
dmz::EventType::get_next_child (RuntimeIterator &it, EventType &type) const {

   Boolean result (False);

   if (_context && (this != &type)) {

      _context->lock.lock ();
      type.set_type_context (_context->table.get_next (it.state.it));
      _context->lock.unlock ();
   }

   if (type) { result = True; }

   return result;
}


/*!

\brief Gets event type config.
\return Returns the config data associated with the event type.

*/
dmz::Config
dmz::EventType::get_config () const {

   Config result (_context ? _context->config : 0);

   return result;
}


/*!

\brief Finds Config.
\details Looks for Config up the EventType tree until Config is found or the root
EventType is reached. Uses dmz::Config::lookup_all_config_merged().
\param[in] Name String containing the name of the Config data to find.
\return Returns a Config object. The Config object will be empty if the named Config
can not be found.

*/
dmz::Config
dmz::EventType::find_config (const String &Name) const {

   EventType type;

   return find_config (Name, type);
}


/*!

\brief Finds Config.
\details Looks for Config up the EventType tree until Config is found or the root
EventType is reached. Uses dmz::Config::lookup_all_config_merged().
\param[in] Name String containing the name of the Config data to find.
\param[out] type EventType that the Config was found in.
\return Returns a Config object. The Config object will be empty if the named Config
can not be found.

*/
dmz::Config
dmz::EventType::find_config (const String &Name, EventType &type) const {

   Config result;

   EventType current (_context);

   while (current) {

      if (current.get_config ().lookup_all_config_merged (Name, result)) {

         type = current;
         current.set_type_context (0);
      }
      else { current.become_parent (); }
   }

   return result;
}


//! For internal use.
void
dmz::EventType::set_type_context (TypeContext *context) {

   if (context != _context) {

      if (_context) { _context->unref (); _context = 0; }
      if (context) { _context = context; _context->ref (); }
   }
}


//! For internal use.
dmz::TypeContext *
dmz::EventType::get_type_context () const { return _context; }


struct dmz::EventTypeSet::State {

   HashTableHandleTemplate<EventType> table;

   ~State () { table.empty (); }

   State &operator= (const State &Value) {

      table.copy (Value.table);
      return *this;
   }
};


/*!

\class dmz::EventTypeSet
\ingroup Runtime
\brief Container for storing sets of EventTypes.

*/

//! Constructor.
dmz::EventTypeSet::EventTypeSet () : _state (*(new State)) {

}


//! Copy constructor.
dmz::EventTypeSet::EventTypeSet (const EventTypeSet &Value) : _state (*(new State)) {

   _state = Value._state;
}


//! Destructor.
dmz::EventTypeSet::~EventTypeSet () { delete &_state; }


//! Assignment operator.
dmz::EventTypeSet &
dmz::EventTypeSet::operator= (const EventTypeSet &Value) {

   _state = Value._state;
   return *this;
}


//! Gets number event types stored in the set.
dmz::Int32
dmz::EventTypeSet::get_count () const { return _state.table.get_count (); }


/*!

\brief Adds and event type to the set.
\param[in] Type EventType to add to the set.
\return Returns dmz::True if the event type was successfully added to the set.

*/
dmz::Boolean
dmz::EventTypeSet::add_event_type (const EventType &Type) {

   Boolean result (False);

   EventType *ptr (new EventType (Type));

   if (ptr && *ptr) {

      result = _state.table.store (ptr->get_handle (), ptr);

      if (!result) { delete ptr; ptr = 0; }
   }

   return result;
}


/*!

\brief Adds an event type of a specified name to the set.
\param[in] Name String containing name of event type to add to the set.
\param[in] context Pointer to the runtime context.
\return Returns dmz::True if the event type was successfully added to the set.

*/
dmz::Boolean
dmz::EventTypeSet::add_event_type (const String &Name, RuntimeContext *context) {

   Boolean result (False);

   EventType *ptr (new EventType (Name, context));

   if (ptr && *ptr) {

      result = _state.table.store (ptr->get_handle (), ptr);

      if (!result) { delete ptr; ptr = 0; }
   }

   return result;
}


/*!

\brief Adds event type with specified unique handle to the set.
\param[in] TypeHandle Unique handle of the event type to add to the set.
\param[in] context Pointer to the runtime context.
\return Returns dmz::True if the event type was successfully added to the set.

*/
dmz::Boolean
dmz::EventTypeSet::add_event_type (const Handle TypeHandle, RuntimeContext *context) {

   Boolean result (False);

   EventType *ptr (new EventType (TypeHandle, context));

   if (ptr && *ptr) {

      result = _state.table.store (ptr->get_handle (), ptr);

      if (!result) { delete ptr; ptr = 0; }
   }

   return result;
}


/*!

\brief Adds a set of EventTypes to the set.
\param[in] Set EventTypeSet of EventTypes to be added to the set.

*/
void
dmz::EventTypeSet::add_event_set (const EventTypeSet &Set) {

   HashTableHandleIterator it;

   EventType *ptr (Set._state.table.get_first (it));

   while (ptr) {

      add_event_type (*ptr);

      ptr = Set._state.table.get_next (it);
   }
}


/*!

\brief Removes event type from set.
\param[in] Type EventType to remove from the set.
\return Returns dmz::True if the type was removed from the set.

*/
dmz::Boolean
dmz::EventTypeSet::remove_event_type (const EventType &Type) {

   Boolean result (False);

   EventType *ptr (_state.table.remove (Type.get_handle ()));

   if (ptr)  {

      delete ptr; ptr = 0;
      result = True;
   }

   return result;
}


/*!

\brief Removes named event type from the set.
\param[in] Name String containing name of event type to remove from the set.
\param[in] context Pointer to runtime context.
\return Returns dmz::True if named event type was successfully removed from set.

*/
dmz::Boolean
dmz::EventTypeSet::remove_event_type (const String &Name, RuntimeContext *context) {

   Boolean result (False);

   EventType tmp (Name, context);

   EventType *ptr (_state.table.remove (tmp.get_handle ()));

   if (ptr)  { delete ptr; ptr = 0; result = True; }

   return result;
}


/*!

\brief Removes event type with the given unique handle.
\param[in] TypeHandle Unique handle of event type to remove from the set.
\param[in] context Pointer to runtime context.
\return Returns dmz::True if event type was successfully remove from the set.

*/
dmz::Boolean
dmz::EventTypeSet::remove_event_type (const Handle TypeHandle, RuntimeContext *context) {

   Boolean result (False);

   EventType tmp (TypeHandle, context);

   EventType *ptr (_state.table.remove (tmp.get_handle ()));

   if (ptr)  { delete ptr; ptr = 0; result = True; }

   return result;
}


/*!

\brief Tests if event type is stored in set.
\param[in] Type EventType to use in test.
\return Returns dmz::True if the \a Type or one of its parents is contained in the set.

*/
dmz::Boolean
dmz::EventTypeSet::contains_type (const EventType &Type) const {

   Boolean result (False);

   EventType current (Type);

   Boolean done (current ? False : True);

   while (!done) {

      if (_state.table.lookup (current.get_handle ())) { result = done = True; }
      else { current.become_parent (); if (!current) { done = True; } }
   }

   return result;
}


/*!

\brief Tests if exact event type is stored in set.
\param[in] Type EventType to use in test.
\return Returns dmz::True if the \a Type is contained in the set.

*/
dmz::Boolean
dmz::EventTypeSet::contains_exact_type (const EventType &Type) const {

  return _state.table.lookup (Type.get_handle ()) != 0;
}


/*!

\brief Gets first event type in the set.
\param[in] it RuntimeIterator used to iterate over the event types in the set.
\param[out] type EventType to store first event type.
\return Returns dmz::True if the first event type is stored in \a type. Returns dmz::False
if the set is empty.

*/
dmz::Boolean
dmz::EventTypeSet::get_first (RuntimeIterator &it, EventType &type) const {

   Boolean result (False);

   EventType *ptr (_state.table.get_first (it.state.it));

   if (ptr) { type = *ptr; result = True; }

   return result;
}


/*!

\brief Gets next event type in the set.
\param[in] it RuntimeIterator used to iterate over the event types in the set.
\param[out] type EventType to store next event type.
\return Returns dmz::True if the next event type is stored in \a type. Returns dmz::False
if all event types have been returned.

*/
dmz::Boolean
dmz::EventTypeSet::get_next (RuntimeIterator &it, EventType &type) const {

   Boolean result (False);

   EventType *ptr (_state.table.get_next (it.state.it));

   if (ptr) { type = *ptr; result = True; }

   return result;
}

