#ifndef DMZ_TYPES_VOLUME_DOT_H
#define DMZ_TYPES_VOLUME_DOT_H

#include <dmzTypesBase.h>

namespace dmz {

class Vector;

class Volume {

   public:
      virtual void set_origin (const Vector &Origin) = 0;
      virtual Vector get_origin () = 0;
      virtual Boolean contains_point (const Vector &Point) = 0; 

   protected:
       Volume () {;}
       ~Volume () {;}

   private:
       Volume (const Volume &);
       Volume &operator= (const Volume &);
};

};

#endif // DMZ_TYPES_VOLUME_DOT_H
