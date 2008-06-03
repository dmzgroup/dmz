#ifndef DMZ_RUNTIME_HANDLE_ALLOCATOR_DOT_H
#define DMZ_RUNTIME_HANDLE_ALLOCATOR_DOT_H

#include <dmzKernelExport.h>
#include <dmzSystemRefCount.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class DMZ_KERNEL_LINK_SYMBOL HandleAllocator : public RefCountDeleteOnZero {

      public:
         HandleAllocator (
            const Handle StartingHandle,
            const Handle MinHandle,
            const Handle MaxHandle);

         HandleAllocator ();
         ~HandleAllocator ();

         Handle get_next_handle (const String &Info);
         Handle request_handle (const Handle RequestedHandle, const String &Info);
         String lookup_info (const Handle TheHandle);
         Boolean release_handle (const Handle TheHandle);

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         HandleAllocator (const HandleAllocator &);
         HandleAllocator &operator= (const HandleAllocator &);
   };
};

#endif // DMZ_RUNTIME_HANDLE_ALLOCATOR_DOT_H

