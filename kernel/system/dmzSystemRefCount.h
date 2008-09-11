#ifndef DMZ_SYSTEM_REF_COUNT_DOT_H
#define DMZ_SYSTEM_REF_COUNT_DOT_H

#include <dmzSystemMutex.h>
#include <dmzTypesBase.h>

namespace dmz {

   class DMZ_KERNEL_LINK_SYMBOL RefCount {

      public:
         Int32 ref ();
         Int32 unref ();

      protected:
         struct State;
         State &_state; //!< Internal state.

         RefCount ();
         virtual ~RefCount ();
         virtual void _ref_count_is_zero () = 0;
   };

   class RefCountDeleteOnZero : public RefCount {

      protected:
         RefCountDeleteOnZero () {;}
         virtual ~RefCountDeleteOnZero () {;}
         virtual void _ref_count_is_zero () { delete this; }
   };
};

#endif //  DMZ_SYSTEM_REF_COUNT_DOT_H

