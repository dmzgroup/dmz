#ifndef DMZ_TYPES_STRING_SUB_DOT_H
#define DMZ_TYPES_STRING_SUB_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesString.h>

namespace dmz {

class DMZ_KERNEL_LINK_SYMBOL StringSub {

   public:
      StringSub ();
      ~StringSub ();

      void store (const String &Tag, const String &Value);
      String lookup (const String &Tag) const;
      void remove (const String &Tag);
      String convert (const String &Value) const;

   protected:
      //! \cond
      struct State;
      State &_state;
      //! \endcond
};

};

#endif // DMZ_TYPES_STRING_SUB_DOT_H
