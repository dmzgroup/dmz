#ifndef DMZ_RUNTIME_HANDLE_DOT_H
#define DMZ_RUNTIME_HANDLE_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class RuntimeContext;
   class HandleAllocator;

   class DMZ_KERNEL_LINK_SYMBOL RuntimeHandle {

      public:
         RuntimeHandle (const String &Info, RuntimeContext *context);
         RuntimeHandle (const String &Info, HandleAllocator *allocator);
         ~RuntimeHandle ();

         Handle get_runtime_handle () const;
         String get_handle_info () const;

      private:
         struct RuntimeHandleState;
         RuntimeHandleState &__state; //!< Internal state.
   };
};

#endif // DMZ_RUNTIME_HANDLE_DOT_H
