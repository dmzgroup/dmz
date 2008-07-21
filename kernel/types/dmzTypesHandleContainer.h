#ifndef DMZ_HANDLE_CONTAINER_DOT_H
#define DMZ_HANDLE_CONTAINER_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class DMZ_KERNEL_LINK_SYMBOL HandleContainer {

      public:
         HandleContainer ();
         HandleContainer (const HandleContainer &Container);
         ~HandleContainer ();

         HandleContainer &operator= (const HandleContainer &Container);

         void clear ();

         Int32 get_count () const;

         Boolean contains (const Handle Value);

         Boolean add_handle (const Handle Value);
         Boolean remove_handle (const Handle Value);

         Handle get_first () const;
         Handle get_next () const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };
};

#endif // DMZ_HANDLE_CONTAINER_DOT_H
