#include <dmzRuntimeConfig.h>
#include "dmzRuntimeContext.h"
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeObjectType.h>
#include "dmzRuntimeTypeContext.h"
#include <dmzTypesHashTableHandle.h>

/*!

\class dmz::ObjectTypeIterator
\ingroup Runtime
\brief Class used to iterate over dmz::ObjectType children.

*/

struct dmz::ObjectTypeIterator::State {

   HashTableHandleIterator it;
};


//! Constructor.
dmz::ObjectTypeIterator::ObjectTypeIterator () : state (*(new State)) {;}


//! Destructor.
dmz::ObjectTypeIterator::~ObjectTypeIterator () { delete &state; }


/*!

\class dmz::ObjectType
\ingroup Runtime
\brief Class provides interface to object types.
\details Objects are persistent in nature. Examples of objects are: entities,
paths, markers, etc.
Objects types are hierarchical. An object type may have one parent and multiple children.
Each object type may contain a config context. This config context contains information
specific to the object type. It may contain model definitions for rendering, audio
files to play, physics data, etc.

*/


//! Constructor.
dmz::ObjectType::ObjectType () : _context (0) {;}


//! Copy constructor.
dmz::ObjectType::ObjectType (const ObjectType &Type) : _context (0) {

   *this = Type;
}


//! Context constructor. For internal use.
dmz::ObjectType::ObjectType (TypeContext *context) : _context (0) {

   set_type_context (context);
}


/*!

\brief Named constructor.
\param[in] Name String containing name of object type.
\param[in] context Pointer to runtime context.
\see dmz::ObjectType::set_type

*/
dmz::ObjectType::ObjectType (const String &Name, RuntimeContext *context) : _context (0) {

   set_type (Name, context);
}


/*!

\brief Handle constructor.
\param[in] TypeHandle Unique object type handle.
\param[in] context Pointer to runtime context.

*/
dmz::ObjectType::ObjectType (const Handle TypeHandle, RuntimeContext *context) :
      _context (0) {

   set_type (TypeHandle, context);
}


//! Destructor.
dmz::ObjectType::~ObjectType () { if (_context) { _context->unref (); } }


//! Assignment operator.
dmz::ObjectType &
dmz::ObjectType::operator= (const ObjectType &Type) {

   set_type_context (Type._context);
   return *this;
}


//! Relational "equal to" operator.
dmz::Boolean
dmz::ObjectType::operator== (const ObjectType &Type) const {

   return Type._context == _context;
}


//! Relational "not equal to" operator.
dmz::Boolean
dmz::ObjectType::operator!= (const ObjectType &Type) const {

   return Type._context != _context;
}


//! Logical not operator.
dmz::Boolean
dmz::ObjectType::operator! () const {

   return _context == 0;
}


/*!

\brief Sets object type by name.
\param[in] Name String containing name of object type.
\param[in] context Pointer to runtime context.
\return Returns dmz::True if the named object type was found.

*/
dmz::Boolean
dmz::ObjectType::set_type (const String &Name, RuntimeContext *context) {

   Definitions defs (context);

   return defs.lookup_object_type (Name, *this);
}


/*!

\brief Sets object type by handle.
\param[in] TypeHandle Unique object type handle.
\param[in] context Pointer to runtime context.
\return Returns dmz::True if the object type was found.

*/
dmz::Boolean
dmz::ObjectType::set_type (const Handle TypeHandle, RuntimeContext *context) {

   Definitions defs (context);

   return defs.lookup_object_type (TypeHandle, *this);
}


/*!

\brief Test if object type is a related type.
\param[in] Type ObjectType to test against.
\return Returns dmz::True if \a Type is the same or a parent.

*/
dmz::Boolean
dmz::ObjectType::is_of_type (const ObjectType &Type) const {

   Boolean result (False);

   ObjectType current (_context);

   if (Type) {

      while (current && !result) {

         if (Type == current) { result = True; }
         else { current.become_parent (); }
      }
   }

   return result;
}


/*!

\brief Test if object type is an exact type.
\param[in] Type ObjectType to test against.
\return Returns dmz::True if \a Type is the same type.

*/
dmz::Boolean
dmz::ObjectType::is_of_exact_type (const ObjectType &Type) const {

   return _context && (Type == *this);
}


//! Gets object type name.
dmz::String
dmz::ObjectType::get_name () const {

   return _context ? _context->Name : "";
}


/*!

\brief Gets object type handle.
\return Returns object type's unique handle. Returns zero if the ObjectType is empty.

*/
dmz::Handle
dmz::ObjectType::get_handle () const {

   return _context ? _context->Handle.get_runtime_handle () : 0;
}


/*!

\brief Gets object type's parent.
\return Returns object types parent. The returned EvenType will be empty if the object
type has no parent.

*/
dmz::ObjectType
dmz::ObjectType::get_parent () {

   ObjectType result;

   if (_context) {

      _context->lock.lock ();
      result.set_type_context (_context->parent);
      _context->lock.unlock ();
   }

   return result;
}


/*!

\brief Becomes object type's parent.
\details The ObejctType will be empty if it has no parent.
\return Returns dmz::True if the object type becomes its parent. Returns dmz::False
if the current object type has no parent.

*/
dmz::Boolean
dmz::ObjectType::become_parent () {

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

\brief Gets first object type child.
\param[in] it ObjectTypeIterator.
\param[out] type ObjectType to store first child.
\return Returns dmz::True if the first child is returned. Returns dmz::False if
the object type has no children.

*/
dmz::Boolean
dmz::ObjectType::get_first_child (ObjectTypeIterator &it, ObjectType &type) {

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

\brief Gets next object type child.
\param[in] it ObjectTypeIterator.
\param[out] type ObjectType to store next child.
\return Returns dmz::True if the next child is returned. Returns dmz::False if
the object type has no more children to return.

*/
dmz::Boolean
dmz::ObjectType::get_next_child (ObjectTypeIterator &it, ObjectType &type) {

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

\brief Gets object type config.
\return Returns the config data associated with the object type.

*/
dmz::Config
dmz::ObjectType::get_config () const {

   Config result (_context ? _context->config : 0);

   return result;
}


//! For internal use.
void
dmz::ObjectType::set_type_context (TypeContext *context) {

   if (context != _context) {

      if (_context) { _context->unref (); _context = 0; }
      if (context) { _context = context; _context->ref (); }
   }
}


//! For internal use.
dmz::TypeContext *
dmz::ObjectType::get_type_context () { return _context; }



struct dmz::ObjectTypeSet::State {

   HashTableHandleTemplate<ObjectType> table;

   ~State () { table.empty (); }

   State &operator= (const State &Value) {

      table.copy (Value.table);
      return *this;
   }
};


dmz::ObjectTypeSet::ObjectTypeSet () : _state (*(new State)) {

}

/*!

\class dmz::ObjectTypeSet
\ingroup Runtime
\brief Container for storing sets of ObjectTypes.

*/

//! Constructor.
dmz::ObjectTypeSet::ObjectTypeSet (const ObjectTypeSet &Value) : _state (*(new State)) {

   _state = Value._state;
}


//! Destructor.
dmz::ObjectTypeSet::~ObjectTypeSet () { delete &_state; }


//! Assignment operator.
dmz::ObjectTypeSet &
dmz::ObjectTypeSet::operator= (const ObjectTypeSet &Value) {

   _state = Value._state;

   return *this;
}


//! Gets number object types stored in the set.
dmz::Int32
dmz::ObjectTypeSet::get_count () const { return _state.table.get_count (); }


//! Adds object type to the set.
dmz::Boolean
dmz::ObjectTypeSet::add_object_type (const ObjectType &Type) {

   Boolean result (False);

   ObjectType *ptr (new ObjectType (Type));

   if (ptr && *ptr) {

      result = _state.table.store (ptr->get_handle (), ptr);

      if (!result) { delete ptr; ptr = 0; }
   }

   return result;
}


/*!

\brief Adds an object type of a specified name to the set.
\param[in] Name String containing name of object type to add to the set.
\param[in] context Pointer to the runtime context.
\return Returns dmz::True if the object type was successfully added to the set.

*/
dmz::Boolean
dmz::ObjectTypeSet::add_object_type (const String &Name, RuntimeContext *context) {

   Boolean result (False);

   ObjectType *ptr (new ObjectType (Name, context));

   if (ptr && *ptr) {

      result = _state.table.store (ptr->get_handle (), ptr);

      if (!result) { delete ptr; ptr = 0; }
   }

   return result;
}


/*!

\brief Adds object type with specified unique handle to the set.
\param[in] TypeHandle Unique handle of the object type to be added.
\param[in] context Pointer to the runtime context.
\return Returns dmz::True if the object type was successfully added to the set.

*/
dmz::Boolean
dmz::ObjectTypeSet::add_object_type (const Handle TypeHandle, RuntimeContext *context) {

   Boolean result (False);

   ObjectType *ptr (new ObjectType (TypeHandle, context));

   if (ptr && *ptr) {

      result = _state.table.store (ptr->get_handle (), ptr);

      if (!result) { delete ptr; ptr = 0; }
   }

   return result;
}


/*!

\brief Adds a set of ObjectTypes to the set.
\param[in] Set ObjectTypeSet of ObjectTypes to be added to the set.

*/
void
dmz::ObjectTypeSet::add_object_set (const ObjectTypeSet &Set) {

   HashTableHandleIterator it;

   ObjectType *ptr (Set._state.table.get_first (it));

   while (ptr) {

      add_object_type (*ptr);

      ptr = Set._state.table.get_next (it);
   }
}


/*!

\brief Removes object type from set.
\param[in] Type ObjectType to remove from the set.
\return Returns dmz::True if the type was removed from the set.

*/
dmz::Boolean
dmz::ObjectTypeSet::remove_object_type (const ObjectType &Type) {

   Boolean result (False);

   ObjectType *ptr (_state.table.remove (Type.get_handle ()));

   if (ptr)  {

      delete ptr; ptr = 0;
      result = True;
   }

   return result;
}


/*!

\brief Removes named object type from the set.
\param[in] Name String containing name of object type to remove.
\param[in] context Pointer to runtime context.
\return Returns dmz::True if named object type was successfully removed from set.

*/
dmz::Boolean
dmz::ObjectTypeSet::remove_object_type (const String &Name, RuntimeContext *context) {

   Boolean result (False);

   ObjectType tmp (Name, context);

   ObjectType *ptr (_state.table.remove (tmp.get_handle ()));

   if (ptr)  { delete ptr; ptr = 0; result = True; }

   return result;
}


/*!

\brief Removes object type with the given unique handle.
\param[in] TypeHandle Unique handle of object type to remove from set.
\param[in] context Pointer to runtime context.
\return Returns dmz::True if object type was successfully remove from the set.

*/
dmz::Boolean
dmz::ObjectTypeSet::remove_object_type (
      const Handle TypeHandle,
      RuntimeContext *context) {

   Boolean result (False);

   ObjectType tmp (TypeHandle, context);

   ObjectType *ptr (_state.table.remove (tmp.get_handle ()));

   if (ptr)  { delete ptr; ptr = 0; result = True; }

   return result;
}


/*!

\brief Tests if object type is stored in set.
\param[in] Type ObjectType to use in test.
\return Returns dmz::True if the \a Type or one of its parents is contained in the set.

*/
dmz::Boolean
dmz::ObjectTypeSet::contains_type (const ObjectType &Type) const {

   Boolean result (False);

   ObjectType current (Type);

   Boolean done (current ? False : True);

   while (!done) {

      if (_state.table.lookup (current.get_handle ())) { result = done = True; }
      else { current.become_parent (); if (!current) { done = True; } }
   }

   return result;
}


/*!

\brief Tests if exact object type is stored in set.
\param[in] Type ObjectType to use in test.
\return Returns dmz::True if the \a Type is contained in the set.

*/
dmz::Boolean
dmz::ObjectTypeSet::contains_exact_type (const ObjectType &Type) const {

  return _state.table.lookup (Type.get_handle ()) != 0;
}


/*!

\brief Gets first object type in the set.
\param[in] it ObjectTypeIterator used to iterate over the object types in the set.
\param[out] type ObjectType to store first object type.
\return Returns dmz::True if the first object type is stored in \a type.
Returns dmz::False if the set is empty.

*/
dmz::Boolean
dmz::ObjectTypeSet::get_first (ObjectTypeIterator &it, ObjectType &type) const {

   Boolean result (False);

   ObjectType *ptr (_state.table.get_first (it.state.it));

   if (ptr) { type = *ptr; result = True; }

   return result;
}


/*!

\brief Gets next object type in the set.
\param[in] it ObjectTypeIterator used to iterate over the object types in the set.
\param[out] type ObjectType to store next object type.
\return Returns dmz::True if the next object type is stored in \a type.
Returns dmz::False if all even types have been returned.

*/
dmz::Boolean
dmz::ObjectTypeSet::get_next (ObjectTypeIterator &it, ObjectType &type) const {

   Boolean result (False);

   ObjectType *ptr (_state.table.get_next (it.state.it));

   if (ptr) { type = *ptr; result = True; }

   return result;
}

