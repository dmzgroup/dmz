#include "dmzRuntimeContext.h"
#include <dmzRuntimeHandle.h>
#include <dmzRuntimeHandleAllocator.h>


/*!

\class dmz::RuntimeHandle
\ingroup Runtime
\brief Class for managing runtime handles.
\details This class may be used to allocate a handle that is unique to the runtime context
or one that is unique to a specific dmz::HandleAllocator. Handles that are unique to
the runtime will most likely not be unique to handles created with a
dmz::HandleAllocator.

*/

struct dmz::RuntimeHandle::RuntimeHandleState {

   Handle handle;
   HandleAllocator *allocator;

   RuntimeHandleState (const String &Info, RuntimeContext *context) :
         handle (0),
         allocator (0) {

      if (context) {

         allocator = context->get_handle_allocator ();

         if (allocator) {

            allocator->ref ();
            handle = allocator->get_next_handle (Info);
         }
      }
   }

   RuntimeHandleState (const String &Info, HandleAllocator *theAllocator) :
         handle (0),
         allocator (theAllocator) {

      if (allocator) {

         allocator->ref ();
         handle = allocator->get_next_handle (Info);
      }
   }


   ~RuntimeHandleState () {

      if (allocator) {

         allocator->release_handle (handle);
         allocator->unref (); allocator = 0;
      }
   }
};


/*!

\brief Unique runtime handle constructor.
\details This constructor will create a handle that is unique within the runtime.
It will not be unique with handles allocated using a HandleAllocator.
\param[in] Info String containing a description of the allocated handle.
\param[in] context Pointer to the runtime context.

*/
dmz::RuntimeHandle::RuntimeHandle (const String &Info, RuntimeContext *context) :
      __state (*(new RuntimeHandleState (Info, context))) {;}


/*!

\brief Unique handle from a given HandleAllocator constructor.
\details This constructor will create a handle that is unique within the alloator.
It will not be unique with handles allocated using other HandleAllocators.
\param[in] Info String containing a description of the allocated handle.
\param[in] allocator Pointer to the HandleAllocator to use in handle creation.

*/
dmz::RuntimeHandle::RuntimeHandle (const String &Info, HandleAllocator *allocator) :
      __state (*(new RuntimeHandleState (Info, allocator))) {;}


//! Destructor. Release allocated handle.
dmz::RuntimeHandle::~RuntimeHandle () { delete &__state; }


/*!

\brief Returns allocated handle.
\return Returns 32-bit allocated handle. Will return zero if no handle was allocated.

*/
dmz::Handle
dmz::RuntimeHandle::get_runtime_handle () const { return __state.handle; }


/*!

\brief Returns handle's info.
\return Returns String containing handle's info.

*/
dmz::String
dmz::RuntimeHandle::get_handle_info () const {

   return __state.allocator ? __state.allocator->lookup_info (__state.handle) : "";
}
