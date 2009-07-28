#ifndef DMZ_HANDLE_CONTAINER_DOT_H
#define DMZ_HANDLE_CONTAINER_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class DMZ_KERNEL_LINK_SYMBOL HandleContainerIterator {

      public:
         HandleContainerIterator ();
         ~HandleContainerIterator ();

         void reset ();

         //! \cond
         struct State;
         State &state;
         //! \endcond

      private:
         HandleContainerIterator (const HandleContainerIterator &);
         HandleContainerIterator &operator= (const HandleContainerIterator &);
   };

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
         Handle get_prev () const;
         Handle get_last () const;

         Handle get_first (HandleContainerIterator &it) const;
         Handle get_next (HandleContainerIterator &it) const;
         Handle get_prev (HandleContainerIterator &it) const;
         Handle get_last (HandleContainerIterator &it) const;

      protected:
         //! \cond
         struct State;
         State &_state;
         //! \endcond
   };
};

#endif // DMZ_HANDLE_CONTAINER_DOT_H
