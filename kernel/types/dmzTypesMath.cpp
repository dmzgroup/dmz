#include <dmzTypesMath.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

//! \addtogroup Types
//! @{

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


/*!

\brief Tests if 32 bit \a Value is near zero.
\details Defined in dmzTypesBase.h.
\param[in] Value contains the dmz::Float32 to be tested.
\return Returns dmz::True if: fabs (Value) \< dmz::Epsilon32.

*/
dmz::Boolean
dmz::is_zero32 (const Float32 Value) {

   return fabs (Value) < Epsilon32;
}


/*!

\brief Tests if 32 bit \a Value is near zero.
\details Defined in dmzTypesBase.h.
\param[in] Value contains the dmz::Float32 to be tested.
\param[in] Epsilon contains the dmz::Float32 to use for the min and max range.
\return Returns dmz::True if: fabs (Value) \< \a Epsilon.

*/
dmz::Boolean
dmz::is_zero32 (const Float32 Value, const Float32 Epsilon) {

   return fabs (Value) < fabs (Epsilon);
}


/*!

\brief Tests if 64 bit \a Value is near zero.
\details Defined in dmzTypesBase.h.
\param[in] Value contains the dmz::Float64 to be tested.
\return Returns dmz::True if: fabs (Value) \< dmz::Epsilon64.

*/
dmz::Boolean
dmz::is_zero64 (const Float64 Value) {

   return fabs (Value) < fabs (Epsilon64);
}


/*!

\brief Tests if 64 bit \a Value is near zero.
\details Defined in dmzTypesBase.h.
\param[in] Value contains the dmz::Float64 to be tested.
\param[in] Epsilon contains the dmz::Float64 to use for the min and max range.
\return Returns dmz::True if: fabs (Value) \< \a Epsilon.

*/
dmz::Boolean
dmz::is_zero64 (const Float64 Value, const Float64 Epsilon) {

   return fabs (Value) < fabs (Epsilon);
}


/*!

\brief Converts 64 bit \a Value from radians to degrees.
\details Defined in dmzTypesBase.h.
\param[in] Value contains the dmz::Float64 to be converted to degrees.

*/
dmz::Float64
dmz::to_degrees (const Float64 Value) { return (Value / Pi64) * 180.0; }


/*!

\brief Converts 64 bit \a Value from degrees to radians.
\details Defined in dmzTypesBase.h.
\param[in] Value contains the dmz::Float64 to be converted to radians.

*/
dmz::Float64
dmz::to_radians (const Float64 Value) { return (Value / 180.0) * Pi64; }


/*!

\brief Normalize an angle.
\details Normalizes and within the given range. That range is \a Min to \a Min + dmz::TwoPi64.
\param[in] Value Angle to normalize.
\param[in] Min Minimum value the angle may have.
\return Returns the normalized angle.

*/
dmz::Float64
dmz::normalize_angle (const Float64 Value, const Float64 Min) {

   Float64 result (Value);

   const Float64 Max = Min + TwoPi64;

   while (result > Max) { result -= TwoPi64; }
   while (result < Min) { result += TwoPi64; }

   return result;
}


/*!

\brief Normalize an angle.
\details Normalizes angle within the range \b negative dmz::Pi64 to \b positive dmz::Pi64.
\param[in] Value Angle to normalize.
\return Returns the normalized angle.

*/
dmz::Float64
dmz::normalize_angle (const Float64 Value) { return normalize_angle (Value, -Pi64); }

//! @}
