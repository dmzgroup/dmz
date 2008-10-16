#ifndef DMZ_TYPES_SPHERE_DOT_H
#define DMZ_TYPES_SPHERE_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesVolume.h>

namespace dmz {

class DMZ_KERNEL_LINK_SYMBOL Sphere : public Volume {

   public:
      Sphere ();
      Sphere (const Vector &Origin, const Float64 Radius);
      ~Sphere ();

      virtual void set_origin (const Vector &Origin);
      virtual Vector get_origin () const;
      virtual Boolean contains_point (const Vector &Point) const;
      virtual void get_extents (Vector &min, Vector &max) const;

      void set_radius (const Float64 Radius);
      Float64 get_radius ();

   protected:
      struct State;
      State &_state;
      
   private:
      Sphere (const Sphere &);
      Sphere &operator= (const Sphere &);
};

};

#endif // DMZ_TYPES_SPHERE_DOT_H
