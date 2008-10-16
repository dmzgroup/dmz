#ifndef DMZ_TYPES_CYLINDER_DOT_H
#define DMZ_TYPES_CYLINDER_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesVolume.h>

namespace dmz {

class DMZ_KERNEL_LINK_SYMBOL Cylinder : public Volume {

   public:
      Cylinder ();
      Cylinder (const Vector &Origin, const Float64 Radius);
      ~Cylinder ();

      virtual void set_origin (const Vector &Origin);
      virtual Vector get_origin ();
      virtual Boolean contains_point (const Vector &Point);

      void set_radius (const Float64 Radius);
      Float64 get_radius ();

   protected:
      struct State;
      State &_state;
      
   private:
      Cylinder (const Cylinder &);
      Cylinder &operator= (const Cylinder &);
};

};

#endif // DMZ_TYPES_CYLINDER_DOT_H
