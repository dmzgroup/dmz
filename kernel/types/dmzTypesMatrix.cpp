#include <dmzTypesMatrix.h>
#include <dmzTypesQuaternion.h>

/*!   

\class dmz::Matrix
\brief 3x3 Matrix.
\ingroup Types
\details This is a row-major 3x3 Matrix. Each component is a 64 bit floating point number.
The indices are as follows:
- 0 1 2
- 3 4 5
- 6 7 8

*/


void
dmz::Matrix::from_quaternion (const Quaternion &Quat) {

   Float64 x (0.0), y (0.0), z (0.0), w (0.0);

   Quat.get_xyzw (x, y, z, w);

   const Float64 XX = x * x;
   const Float64 XY = x * y;
   const Float64 XZ = x * z;
   const Float64 XW = x * w;

   const Float64 YY = y * y;
   const Float64 YZ = y * z;
   const Float64 YW = y * w;

   const Float64 ZZ = z * z;
   const Float64 ZW = z * w;
   

   //Row 1
   _data[0] = 1.0 - 2.0 * (YY + ZZ);
   _data[1] = 2.0 * (XY - ZW);
   _data[2] = 2.0 * (XZ + YW);
   
   //Row 2
   _data[3] = 2.0 * (XY + ZW);
   _data[4] = 1.0 - 2.0 * (XX + ZZ);
   _data[5] = 2.0 * (YZ - XW);

   //Row 3
   _data[6] = 2.0 * (XZ - YW);
   _data[7] = 2.0 * (YZ + XW);
   _data[8] = 1.0 - 2.0 * (XX + YY);
}


void
dmz::Matrix::to_quaternion (Quaternion &quat) const {
   quat = Quaternion (*this);
}
