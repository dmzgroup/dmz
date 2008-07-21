#ifndef DMZ_STRING_CONTAINER_DOT_H
#define DMZ_STRING_CONTAINER_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class String;

   class DMZ_KERNEL_LINK_SYMBOL StringContainer {

      public:
         StringContainer ();
         StringContainer (const StringContainer &Container);
         ~StringContainer ();

         StringContainer &operator= (const StringContainer &Container);

         void clear ();

         Int32 get_count () const;

         Boolean contains (const String &Value);

         Boolean add_string (const String &Value);
         Boolean remove_string (const String &Value);

         Boolean get_first (String &value) const;
         Boolean get_next (String &value) const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };
};

#endif // DMZ_STRING_CONTAINER_DOT_H
