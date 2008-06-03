#include <dmzTypesMath.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>


//! Experimental function. Not for public use at this time.
void
dmz::get_ortho (
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
 

//! Experimental function. Not for public use at this time.
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

   heading = vForward.get_angle (Forward);

   const Vector Cross (vForward.cross (Forward));

   if (Cross.get_y () < 0.0) { heading = TwoPi64 - heading; }

   return heading;
}
