#ifndef DMZ_TYPES_CYLINDER_DOT_H
#define DMZ_TYPES_CYLINDER_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesVector.h>
#include <dmzTypesVolume.h>

namespace dmz {

class DMZ_KERNEL_LINK_SYMBOL Cylinder : public Volume {

   public:
      Cylinder ();
      Cylinder (
         const Vector &Origin,
         const Float64 Radius,
         const Float64 Height,
         const VectorComponentEnum Axis);

      ~Cylinder ();

      virtual void set_origin (const Vector &Origin);
      virtual Vector get_origin () const;
      virtual Boolean contains_point (const Vector &Point) const;
      virtual void get_extents (Vector &origin, Vector &min, Vector &max) const;


      void set_radius (const Float64 Radius);
      Float64 get_radius () const;

      void set_height (const Float64 Height);
      Float64 get_height () const;

      void set_axis (const VectorComponentEnum Axis);
      VectorComponentEnum get_axis () const;

   protected:
      //! \cond
      struct State;
      State &_state;
      //! \endcond
      
   private:
      Cylinder (const Cylinder &);
      Cylinder &operator= (const Cylinder &);
};

};

#endif // DMZ_TYPES_CYLINDER_DOT_H
