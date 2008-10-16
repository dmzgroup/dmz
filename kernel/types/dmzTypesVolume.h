#ifndef DMZ_TYPES_VOLUME_DOT_H
#define DMZ_TYPES_VOLUME_DOT_H

#include <dmzTypesBase.h>

namespace dmz {

class Vector;

class Volume {

   public:
      virtual void set_origin (const Vector &Origin) = 0;
      virtual Vector get_origin () const = 0;
      virtual Boolean contains_point (const Vector &Point) const = 0; 
      virtual void get_extents (Vector &min, Vector &max) const = 0;

   protected:
      //! \cond
      Volume () {;}
      ~Volume () {;}
      //! \endcond

   private:
      Volume (const Volume &);
      Volume &operator= (const Volume &);
};

};

#endif // DMZ_TYPES_VOLUME_DOT_H
