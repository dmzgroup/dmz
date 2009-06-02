#include <dmzTypesMath.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>


//! Experimental function. Not for general use at this time.
void
dmz::get_orthogonal_component (
      const Vector &Normal,
      const Vector &Value,
      Vector &ortho,
      Vector *remainder) {

   const Vector UpY (0.0, 1.0, 0.0);
   Matrix mat (Normal, UpY);

   ortho = Value;
   mat.transform_vector (ortho);

   mat.invert_in_place ();

   if (remainder) {

      remainder->set_xyz (0.0, ortho.get_y (), 0.0);
      mat.transform_vector (*remainder);
   }

   ortho.set_y (0.0);

   mat.transform_vector (ortho);
}


//! Experimental function. Not for general use at this time.
dmz::Float64
dmz::get_heading (const Matrix &Ori) {

   Float64 heading (0.0);
   const Vector Up (0.0, 1.0, 0.0);
   const Vector Forward (0.0, 0.0, -1.0);

   Vector vUp (Up), vForward (Forward);

   Ori.transform_vector (vUp);
   Ori.transform_vector (vForward);

   Matrix mat (vUp, Up);

   mat.transform_vector (vForward);

   heading = Forward.get_angle (vForward);

   const Vector Cross (Forward.cross (vForward));

   if (Cross.get_y () < 0.0) { heading = TwoPi64 - heading; }

   return heading;
}


//! Experimental function. Not for general use at this time.
dmz::Float64
dmz::get_rotation_angle (const Vector &V1, const Vector &V2) {

   Float64 result = V1.get_angle (V2);

   const Vector Cross = V1.cross (V2);

   const Float64 ValueY = Cross.get_y ();

   if (is_zero64 (ValueY)) {

      const Float64 ValueX = Cross.get_x ();

      if (is_zero64 (ValueX)) {

         const Float64 ValueZ = Cross.get_z ();

         if (ValueZ > 0.0) { result = -result; }
      }
      else if (ValueX < 0.0) { result = -result; }
   }
   else if (ValueY < 0.0) { result = -result; }

   return result;
}
