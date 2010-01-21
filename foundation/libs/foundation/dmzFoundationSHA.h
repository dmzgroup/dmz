#ifndef DMZ_FOUNDATION_SHA_DOT_H
#define DMZ_FOUNDATION_SHA_DOT_H

#include <dmzFoundationExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

enum SHATypeEnum { SHA1, SHA224, SHA256, SHA384, SHA512 };

class DMZ_FOUNDATION_LINK_SYMBOL SHA {

   public:
      SHA (const SHATypeEnum Type = SHA512);
      ~SHA ();

      SHATypeEnum get_type () const;
      void init (const SHATypeEnum Type = SHA512);
      void add_data (const char *buffer, const Int32 Size);
      String finish ();

   protected:
      //! \cond
      struct State;
      State &_state;
      //! \endcond
};

};

#endif // DMZ_FOUNDATION_SHA_DOT_H
