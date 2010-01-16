#ifndef DMZ_RUNTIME_VERSION_DOT_H
#define DMZ_RUNTIME_VERSION_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesString.h>

namespace dmz {

class Config;

class DMZ_KERNEL_LINK_SYMBOL Version {

   public:
      Version ();
      Version (const Config &Source);
      Version (const Config &Source, const String &Prefix);
      Version (const Version &Value);
      ~Version ();

      Version &operator= (const Version &Value);
      Boolean operator< (const Version &Value) const;
      Boolean operator> (const Version &Value) const;

      String get_version () const;
      String get_name () const;
      String get_major () const;
      String get_minor () const;
      String get_bug () const;
      String get_build () const;
      String get_release () const;
      String get_image_name () const;

   protected:
      //! \cond
      struct State;
      State &_state;
      //! \endcond
};

};

#endif // DMZ_RUNTIME_VERSION_DOT_H
