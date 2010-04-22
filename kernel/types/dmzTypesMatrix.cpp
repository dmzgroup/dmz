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
void
dmz::Matrix::from_euler_angles (const Float64 Hy, const Float64 Px, const Float64 Rz) {

   *this = Matrix (Up, Hy) * Matrix (Right, Px) * Matrix (Forward, Rz);
}


void
dmz::Matrix::to_euler_angles (Float64 &hy, Float64 &px, Float64 &rz)  const {

   Vector hvec (Forward), pvec (Forward), rvec (Up);
   transform_vector(hvec);
   Matrix cmat (*this), hmat, pmat, rmat;

   hvec.set_y (0.0);

   if (hvec.is_zero ()) { hy = 0.0; }
   else {

      hvec.normalize_in_place ();
      hy = get_rotation_angle (Forward, hvec);
      hmat.from_axis_and_angle (Up, hy);
      hmat.transpose_in_place ();
      cmat = hmat * cmat;
   }

   cmat.transform_vector (pvec);

   if (is_zero64 (pvec.get_y ())) { px = 0.0; }
   else {

      px = get_rotation_angle (Forward, pvec);
      pmat.from_axis_and_angle (Right, px);
      pmat.transpose_in_place ();
      cmat = pmat * cmat;
   }

   cmat.transform_vector (rvec);

   if (is_zero64 (rvec.get_x ())) { rz = 0.0; }
   else { rz = get_rotation_angle (Up, rvec); }
}

