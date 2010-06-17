#include <dmzTypesMatrix.h>
#include <dmzTypesMath.h>

namespace {

static const dmz::Vector Forward (0.0, 0.0, -1.0);
static const dmz::Vector Right (1.0, 0.0, 0.0);
static const dmz::Vector Up (0.0, 1.0, 0.0);

};

/*!

\class dmz::Matrix
\brief 3x3 Matrix.
\ingroup Types
\details This is a row-major 3x3 Matrix. Each component is a 64 bit floating point number.
The indices are as follows:
- 0 1 2
- 3 4 5
- 6 7 8

\htmlonly Lua bindings are <a href="dmzlua.html#dmz.matrix">available</a>
\endhtmlonly

*/

/*!

\brief Creates a Matrix from three Euler angles.
\details The angles are applied Heading * Pitch * Roll.
\param[in] Hy Heading in radians.
\param[in] Px Pitch in radians.
\param[in] Rz Roll in radians.

*/
void
dmz::Matrix::from_euler_angles (const Float64 Hy, const Float64 Px, const Float64 Rz) {

   *this = Matrix (Up, Hy) * Matrix (Right, Px) * Matrix (Forward, Rz);
}


/*!

\brief Decomposes the Matrix into three Euler angles.
\param[out] hy Heading in radians.
\param[out] px Pitch in radians.
\param[out] rz Roll in radians.

*/
void
dmz::Matrix::to_euler_angles (Float64 &hy, Float64 &px, Float64 &rz)  const {

   Vector hvec (Forward), pvec (Forward), rvec (Up);
   transform_vector(hvec);
   Matrix cmat (*this), hmat, pmat, rmat;

   hvec.set_y (0.0);

   if (hvec.is_zero ()) { hy = 0.0; }
   else {

      hvec.normalize_in_place ();
      hy = Forward.get_signed_angle (hvec);
      hmat.from_axis_and_angle (Up, hy);
      hmat.transpose_in_place ();
      cmat = hmat * cmat;
   }

   cmat.transform_vector (pvec);

   if (is_zero64 (pvec.get_y ())) { px = 0.0; }
   else {

      px = Forward.get_signed_angle (pvec);
      pmat.from_axis_and_angle (Right, px);
      pmat.transpose_in_place ();
      cmat = pmat * cmat;
   }

   cmat.transform_vector (rvec);

   if (is_zero64 (rvec.get_x ())) { rz = 0.0; }
   else { rz = Up.get_signed_angle (rvec); }
}


/*!

\brief Creates a Matrix from a single Vector.
\details The Matrix will look in the direction of the given vector. The Matrix will
only update heading a pitch and will not contain a roll component.
\param[in] Direction Vector pointing in the direction to look.

*/
void
dmz::Matrix::from_vector (const Vector &Direction) {

   Vector hvec (Direction), pvec (Direction);
   Matrix hmat, pmat;

   hvec.set_y (0.0);

   if (!hvec.is_zero ()) {

      hvec.normalize_in_place ();
      hmat.from_axis_and_angle (Up, Forward.get_signed_angle (hvec));
   }

   hmat.transpose ().transform_vector (pvec);

   if (!is_zero64 (pvec.get_y ())) {

      pmat.from_axis_and_angle (Right, Forward.get_signed_angle (pvec));
   }

   *this = hmat * pmat;
}
