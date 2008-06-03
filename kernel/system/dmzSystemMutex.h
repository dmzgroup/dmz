#ifndef DMZ_SYSTEM_MUTEX_DOT_H
#define DMZ_SYSTEM_MUTEX_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class DMZ_KERNEL_LINK_SYMBOL Mutex {

      public:
         Mutex ();
         ~Mutex ();

         Boolean try_lock ();
         void lock ();
         void unlock ();

      protected:
         struct State;
         State &_state; //!< Internal state.
   };
};

#endif // DMZ_SYSTEM_MUTEX_DOT_H
