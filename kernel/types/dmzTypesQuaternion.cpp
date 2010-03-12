#include <dmzTypesMatrix.h>
#include <dmzTypesQuaternion.h>

dmz::Quaternion &
dmz::Quaternion::from_matrix (const Matrix &Mat) {

   Float64 M[9]; Mat.to_array (M);

   const Float64 T (1.0 + M[0] + M[4] + M[8]);

   if (T > Epsilon64) {
      
      Float64 s (sqrt (T) * 2.0);
      _x = (M[7] - M[5]) / s;
      _y = (M[2] - M[6]) / s;
      _z = (M[3] - M[1]) / s;
      _w = 0.25 * s;
   } 
   else if ((M[0] > M[4]) && (M[0] > M[8])) {

      Float64 s (sqrt (1.0 + M[0] - M[4] - M[8]) * 2.0);
      _x = 0.25 * s;
      _y = (M[3] + M[1]) / s;
      _z = (M[2] + M[6]) / s;
      _w = (M[7] - M[5]) / s;
   } 
   else if (M[4] > M[8]) {

      Float64 s (sqrt (1.0 + M[4] - M[0] - M[8]) * 2.0);
      _x = (M[3] + M[1]) / s;
      _y = 0.25 * s;
      _z = (M[7] + M[5]) / s;
      _w = (M[2] - M[6]) / s;
   } 
   else {

      Float64 s = sqrt (1.0 + M[8] - M[0] - M[4]) * 2.0;
      _x = (M[2] + M[6]) / s;
      _y = (M[7] + M[5]) / s;
      _z = 0.25 * s;
      _w = (M[3] - M[1]) / s;
   }
   return *this;
}


void 
dmz::Quaternion::to_matrix (Matrix &Mat) {

   Mat = Matrix (*this);
}


void 
dmz::Quaternion::from_matrix_array (const Float64 data[9]) {

   Matrix tempMatrix (data);
   from_matrix (tempMatrix);
}


void 
dmz::Quaternion::to_matrix_array (Float64 data[9]) const {

   Matrix tempMatrix (*this);
   tempMatrix.to_array (data);
}


void 
dmz::Quaternion::from_matrix_array32 (const Float32 data[9]) {

   Matrix tempMatrix (data);
   from_matrix (tempMatrix);
}


void 
dmz::Quaternion::to_matrix_array32 (Float32 data[9]) const {

   Matrix tempMatrix (*this);
   tempMatrix.to_array32 (data);
}
