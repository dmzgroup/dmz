#ifndef DMZ_RUNTIME_ITERATOR_DOT_H
#define DMZ_RUNTIME_ITERATOR_DOT_H

#include <dmzKernelExport.h>

namespace dmz {

class DMZ_KERNEL_LINK_SYMBOL RuntimeIterator {

   public:
      RuntimeIterator ();
      ~RuntimeIterator ();

      void reset ();

      struct State;
      State &state;

   private:
      RuntimeIterator (const RuntimeIterator &);
      RuntimeIterator &operator= (const RuntimeIterator &);
};

};

#endif // DMZ_RUNTIME_ITERATOR_DOT_H
