#ifndef DMZ_TYPES_STRING_CONTAINER_DOT_H
#define DMZ_TYPES_STRING_CONTAINER_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class String;

   class DMZ_KERNEL_LINK_SYMBOL StringContainerIterator {

      public:
         StringContainerIterator ();
         ~StringContainerIterator ();

         void reset ();

         //! \cond
         struct State;
         State &state;
         //! \endcond

      private:
         StringContainerIterator (const StringContainerIterator &);
         StringContainerIterator &operator= (const StringContainerIterator &);
   };

   class DMZ_KERNEL_LINK_SYMBOL StringContainer {

      public:
         StringContainer ();
         StringContainer (const StringContainer &Container);
         ~StringContainer ();

         StringContainer &operator= (const StringContainer &Container);
         Boolean operator== (const StringContainer &Container) const;
         Boolean has_same_content (const StringContainer &Container) const;

         StringContainer &operator+= (const StringContainer &Container);

         void clear ();

         Int32 get_count () const;

         Boolean contains (const String &Value) const;

         Boolean add (const String &Value);
         Boolean remove (const String &Value);

         Boolean get_first (String &value) const;
         Boolean get_next (String &value) const;
         Boolean get_prev (String &value) const;
         Boolean get_last (String &value) const;

         Boolean get_first (StringContainerIterator &it, String &value) const;
         Boolean get_next (StringContainerIterator &it, String &value) const;
         Boolean get_prev (StringContainerIterator &it, String &value) const;
         Boolean get_last (StringContainerIterator &it, String &value) const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };
};

#endif // DMZ_TYPES_STRING_CONTAINER_DOT_H
