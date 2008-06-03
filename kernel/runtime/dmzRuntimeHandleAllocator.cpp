#include "dmzRuntimeHandleAllocator.h"
#include <dmzSystemMutex.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesString.h>

/*!

\class dmz::HandleAllocator
\ingroup Runtime
\brief Allocates 32-bit handles.
\details This class is thread safe. Handles from a single HandleAllocator will be unique.
Handles from two different HandleAllocator are \b not guaranteed to be unique.
The HandleAllocator assumes that zero is an invalid Handle.

*/

struct dmz::HandleAllocator::State {

   const Handle MinHandle;
   const Handle MaxHandle;
   Handle count;
   Mutex lock;
   HashTableHandleTemplate<String> table;

   State (
         const Handle StartingHandle,
         const Handle TheMinHandle,
         const Handle TheMaxHandle) :
         MinHandle (TheMinHandle ? TheMinHandle : 1),
         MaxHandle ((TheMaxHandle > TheMinHandle) ? TheMaxHandle : 0),
         count (StartingHandle) {

      if (count < MinHandle) { count = MinHandle; }
      if (MaxHandle && (MaxHandle < count)) { count = MaxHandle; }
   }

   ~State () { table.empty (); }

   Handle save_handle (const Handle TheHandle, const String &Info) {

      Handle result (0);

      String *ptr = new String (Info);

      if (ptr) {

         if (table.store (TheHandle, ptr)) {

            result = TheHandle;
         }
         else { delete ptr; ptr = 0; }
      }

      return result;
   }
};


/*!

\brief Constructor.
\param[in] StartingHandle Specifies the first handle the allocator will return.
\param[in] MinHandle Smallest value a handle may have.
\param[in] MaxHandle Largest value a handle may have.

*/
dmz::HandleAllocator::HandleAllocator (
      const Handle StartingHandle,
      const Handle MinHandle,
      const Handle MaxHandle) :
      _state (*(new State (StartingHandle, MinHandle, MaxHandle))) {;}

/*!

\brief Default constructor.
\note Starts with the first handle at one and uses entire 32 bit space for handles.

*/
dmz::HandleAllocator::HandleAllocator () :
      _state (*(new State (0, 0, 0))) {;}


//! Destructor.
dmz::HandleAllocator::~HandleAllocator () { delete &_state; }


/*!

\brief returns next Handle.
\param[in] Info String containing information about the handle. String may be empty.
\return Returns new handle. Will return zero if the allocator is out of handles.

*/
dmz::Handle
dmz::HandleAllocator::get_next_handle (const String &Info) {

   Handle handle (0);

   Boolean done (False);

   _state.lock.lock ();

   do {

      if (!_state.table.lookup (_state.count)) {

         done = True;
         handle = _state.save_handle (_state.count, Info);
         _state.count++;
      }
      else {

         _state.count++;

         if (_state.MaxHandle && (_state.MaxHandle < _state.count)) {

            _state.count = _state.MinHandle;
         }
         else if (_state.count < _state.MinHandle) {

            _state.count = _state.MinHandle;
         }
      }

   } while (!done);

   _state.lock.unlock ();

   return handle;
}


/*!

\brief Requests specific handle.
\param[in] RequestedHandle Handle being requested.
\param[in] Info String containing information about the handle. String may be empty.
\return Returns new handle. Will return zero if the allocator is out of handles. The
allocator will do its best to return the requested handle. It may return a different
handle from the one requested if the handle has already been allocator or if it is 
outside the range of the allocator.

*/
dmz::Handle
dmz::HandleAllocator::request_handle (
      const Handle RequestedHandle,
      const String &Info) {

   dmz::Handle handle (0);

   _state.lock.lock ();

   if ((RequestedHandle > _state.MaxHandle) || (RequestedHandle < _state.MinHandle)) {

      handle = get_next_handle (Info);
   }
   else {
      if (_state.table.lookup (RequestedHandle)) { handle = get_next_handle (Info); }
      else { handle = _state.save_handle (RequestedHandle, Info); }
   }

   _state.lock.unlock ();

   return handle;
}


/*!

\brief Returns the String associated with the allocated handle.
\param[in] TheHandle Allocated handle.
\return Returns a String containing the information associated with the allocated handle.

*/
dmz::String
dmz::HandleAllocator::lookup_info (const Handle TheHandle) {

   String result;

   _state.lock.lock ();
   String *ptr = _state.table.lookup (TheHandle);
   if (ptr) { result = *ptr; }
   _state.lock.unlock ();

   return result;
}


/*!

\brief Release allocated handle.
\details An allocated handle that has been released may be returned by future
calls of dmz::HandleAllocator::get_next_handle.
\param[in] TheHandle Allocated handle to be released.
\return Returns dmz::True if the handle was released.

*/
dmz::Boolean
dmz::HandleAllocator::release_handle (const Handle TheHandle) {

   Boolean result (False);

   _state.lock.lock ();

   String *ptr = _state.table.remove (TheHandle);

   if (ptr) {

      delete ptr; ptr = 0;
      result = True;
   }

   _state.lock.unlock ();

   return result;
}

