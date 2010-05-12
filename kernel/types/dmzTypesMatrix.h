#ifndef DMZ_TYPES_MATRIX_DOT_H
#define DMZ_TYPES_MATRIX_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesConsts.h>
#include <dmzTypesVector.h>

#include <math.h>


namespace dmz {

class Matrix {

   public:
      Matrix ();
      Matrix (
         const Float64 V0, const Float64 V1, const Float64 V2,
         const Float64 V3, const Float64 V4, const Float64 V5,
         const Float64 V6, const Float64 V7, const Float64 V8);
      Matrix (const Float64 Array[9]);
      Matrix (const Float32 Array[9]);
      Matrix (const Float64 RadiansX, const Float64 RadiansY, const Float64 RadiansZ);
      Matrix (const Vector &Direction);
      Matrix (const Vector &Axis, const Float64 AngleRadians);
      Matrix (const Vector &FromValue, const Vector &ToValue);
      Matrix (const Matrix &Mat);
      ~Matrix () {;}

      Matrix &operator= (const Matrix &Mat);

      Matrix &operator+= (const Matrix &Mat);
      Matrix operator+ (const Matrix &Mat) const;

      Matrix operator- () const;

      Matrix &operator-= (const Matrix &Mat);
      Matrix operator- (const Matrix &Mat) const;

      Matrix &operator*= (const Matrix &Mat);
      Matrix &operator*= (const Float64 Scalar);

      Matrix operator* (const Matrix &Mat) const;
      Matrix operator* (const Float64 Scalar) const;

      Boolean operator== (const Matrix &Mat) const;
      Boolean operator!= (const Matrix &Mat) const;

      Matrix transpose () const;
      Matrix &transpose_in_place ();
      Matrix negate () const;
      Matrix &negate_in_place ();
      Boolean invert (Matrix &value) const;
      Boolean invert_in_place ();

      Matrix &yaw_in_place (const Float64 Angle);
      Matrix &pitch_in_place (const Float64 Angle);
      Matrix &roll_in_place (const Float64 Angle);

      Vector &transform_vector (Vector &Vec) const;

      Boolean set_element (const Int32 Row, const Int32 Col, const Float64 Data);
      Boolean get_element (const Int32 Row, const Int32 Col, Float64 &data) const;

      void from_array (const Float64 Array[9]);
      void to_array (Float64 array[9]) const;
      void from_array32 (const Float32 Array[9]);
      void to_array32 (Float32 array[9]) const;

      DMZ_KERNEL_LINK_SYMBOL void from_euler_angles (
         const Float64 Hy,
         const Float64 Px,
         const Float64 Rz);

      DMZ_KERNEL_LINK_SYMBOL void to_euler_angles (
         Float64 &hy,
         Float64 &px,
         Float64 &rz) const;

      void from_axis_and_angle (
         const Vector &Axis,
         const Float64 AngleRadians);

      void to_axis_and_angle (Vector &axis, Float64 &angleRadians) const;

      DMZ_KERNEL_LINK_SYMBOL void from_vector (const Vector &Direction);
      void from_two_vectors (const Vector &FromValue, const Vector &ToValue);

      void set_identity ();
      Boolean is_identity () const;

   protected:
      Float64 _data[9]; //!< Matrix elements.
};

};


/*!

\brief Default constructor.
\details Initializes the Matrix as an identity Matrix.

*/
inline
dmz::Matrix::Matrix () {

   //Identity Matrix
   _data[1] = _data[2] = _data[3] = 0.0;
   _data[5] = _data[6] = _data[7] = 0.0;
   _data[0] = _data[4] = _data[8] = 1.0;
}


/*!

\brief Element Constructor.
\details Assumes row major order.

*/
inline
dmz::Matrix::Matrix (
      const Float64 V0, const Float64 V1, const Float64 V2,
      const Float64 V3, const Float64 V4, const Float64 V5,
      const Float64 V6, const Float64 V7, const Float64 V8) {

   _data[0] = V0;
   _data[1] = V1;
   _data[2] = V2;
   _data[3] = V3;
   _data[4] = V4;
   _data[5] = V5;
   _data[6] = V6;
   _data[7] = V7;
   _data[8] = V8;
}


/*!

\brief 64-bit array constructor.
\param[in] Array contains a row-major 3x3 matrix stored in a dmz::Float64[9] array.

*/
inline
dmz::Matrix::Matrix (const Float64 Array[9]) {

   for (Int32 count = 0; count < 9; count++) {

      _data[count] = Array[count];
   }
}


/*!

\brief 32-bit array constructor.
\param[in] Array contains a row-major 3x3 matrix stored in a dmz::Float32[9] array.

*/
inline
dmz::Matrix::Matrix (const Float32 Array[9]) {

   for (Int32 count = 0; count < 9; count++) {

      _data[count] = Array[count];
   }
}


/*!

\brief \b Not currently working.

*/
inline
dmz::Matrix::Matrix (
      const Float64 RadiansX,
      const Float64 RadiansY,
      const Float64 RadiansZ) {

   from_euler_angles (RadiansX, RadiansY, RadiansZ);
}


inline
dmz::Matrix::Matrix (const Vector &Direction) { from_vector (Direction); }


/*!

\brief Rotation around an arbitrary axis constructor.
\param[in] Axis Arbitrary axis described by a Vector.
\param[in] AngleRadians Rotation about arbitrary axis in radians.

*/
inline
dmz::Matrix::Matrix (const Vector &Axis, const Float64 AngleRadians) {

   from_axis_and_angle (Axis, AngleRadians);
}


/*!

\brief Creates Matrix that will transform one FromValue vector to ToValue vector.
\param[in] FromValue Vector to transform from.
\param[in] ToValue Vector to transform to.

*/
inline
dmz::Matrix::Matrix (const Vector &FromValue, const Vector &ToValue) {

   from_two_vectors (FromValue, ToValue);
}


//! Copy constructor.
inline
dmz::Matrix::Matrix (const Matrix &Mat) { Mat.to_array (_data); }


//! Assignment operator.
inline dmz::Matrix &
dmz::Matrix::operator= (const Matrix &Mat) {

   for (Int32 count = 0; count < 9; count++) {

      _data[count] = Mat._data[count];
   }

   return *this;
}


//! Assignment by sum operator.
inline dmz::Matrix &
dmz::Matrix::operator+= (const Matrix &Mat) {

   for (Int32 count = 0; count < 9; count++) {

      _data[count] += Mat._data[count];
   }

   return *this;
}


//! Addition operator.
inline dmz::Matrix
dmz::Matrix::operator+ (const Matrix &Mat) const {

   Matrix result (*this);
   return result += Mat;
}


//! Subtraction operator.
inline dmz::Matrix
dmz::Matrix::operator- () const {

   Matrix result (*this);
   result.negate_in_place ();
   return result;
}


//! Assignment by subtraction operator.
inline dmz::Matrix &
dmz::Matrix::operator-= (const Matrix &Mat) {

   for (Int32 count = 0; count < 9; count++) {

      _data[count] -= Mat._data[count];
   }

   return *this;
}


//! Subtraction operator.
inline dmz::Matrix
dmz::Matrix::operator- (const Matrix &Mat) const {

   Matrix result (*this);
   return result -= Mat;
}


//! Assignment by Matrix multiplication operator.
inline dmz::Matrix &
dmz::Matrix::operator*= (const Matrix &Mat) {

   Float64 sum (0.0);
   Int32 offset0 (0), offset1 (0), offset2 (0);

   Float64 resultMatrix[9];

   for (Int32 row = 0; row < 3; row++) {

      for (Int32 col = 0; col < 3; col++) {

         offset0 = (row * 3) + col;
         sum = 0.0;

         for (Int32 count = 0; count < 3; count++) {

            offset1 = (row * 3) + count;
            offset2 = (count * 3) + col;
            sum += _data[offset1] * Mat._data[offset2];
         }

         resultMatrix[offset0] = sum;
      }
   }

   from_array (resultMatrix);

   return *this;
}


//! Assignment by scalar multiplication operator.
inline dmz::Matrix &
dmz::Matrix::operator*= (const Float64 Scalar) {

   for(Int32 count = 0; count < 9; count++) {

      _data[count] *= Scalar;
   }

   return *this;
}


//! Matrix multiplication operator.
inline dmz::Matrix
dmz::Matrix::operator* (const Matrix &Mat) const {

   Matrix result (*this);
   return result *= Mat;
}


//! Scalar multiplication operator.
inline dmz::Matrix
dmz::Matrix::operator* (const Float64 Scalar) const {

   Matrix result (*this);
   return result *= Scalar;
}


//! Relational "equal to" operator.
inline dmz::Boolean
dmz::Matrix::operator== (const Matrix &Mat) const {

   Boolean result (True);

   for(Int32 count = 0; (count < 9) && result; count++) {

      Float64 diff = fabs (_data[count] - Mat._data[count]);
      result = result && (diff < Epsilon64);
   }

   return result;
}


//! Relational "not equal to" operator.
inline dmz::Boolean
dmz::Matrix::operator!= (const Matrix &Mat) const {

   return !(*this == Mat);
}


/*!

\brief Matrix transpose function.
\return Returns a dmz::Matrix with a transpose of the Matrix.

*/
inline dmz::Matrix
dmz::Matrix::transpose () const {

   Matrix result (*this);
   return result.transpose_in_place ();
}


/*!

\brief Matrix transpose in place function.
\details Transposes the matrix in place.
\return Returns reference to self.

*/
inline dmz::Matrix &
dmz::Matrix::transpose_in_place () {

   //0 1 2    0 3 6
   //3 4 5 => 1 4 7
   //6 7 8    2 5 8

   Float64 tempVal (0.0);

   tempVal = _data[3];
   _data[3] = _data[1];
   _data[1] = tempVal;

   tempVal = _data[6];
   _data[6] = _data[2];
   _data[2] = tempVal;

   tempVal = _data[7];
   _data[7] = _data[5];
   _data[5] = tempVal;

   return *this;
}


/*!

\brief Matrix negate function.
\return Returns a dmz::Matrix with the negation of the Matrix.

*/
inline dmz::Matrix
dmz::Matrix::negate () const {

   Matrix result (*this);
   result.negate_in_place ();
   return result;
}


/*!

\brief Matrix negation in place function.
\details Negates the matrix in place.
\return Returns reference to self.

*/
inline dmz::Matrix &
dmz::Matrix::negate_in_place () {

   for (Int32 count = 0; count < 9; count++) { _data[count] *= -1.0; }
   return *this;
}


/*!

\brief Creates an inverse of the Matrix.
\param[out] value The inverted Matrix.
\return Returns dmz::True if the matrix was successfully inverted

*/
inline dmz::Boolean
dmz::Matrix::invert (Matrix &value) const {

   Boolean result (False);

   Float64 determinant =
      _data[0] * (_data[4] * _data[8] - _data[7] * _data[5]) -
      _data[1] * (_data[3] * _data[8] - _data[6] * _data[5]) +
      _data[2] * (_data[3] * _data[7] - _data[6] * _data[4]);

   result = (fabs (determinant) >= Epsilon64);

   if (result) {

      Float64 inverseData[9];
      inverseData[0] =  ((_data[4] * _data[8]) - (_data[5] * _data[7])) / determinant;
      inverseData[1] = -((_data[1] * _data[8]) - (_data[7] * _data[2])) / determinant;
      inverseData[2] =  ((_data[1] * _data[5]) - (_data[4] * _data[2])) / determinant;
      inverseData[3] = -((_data[3] * _data[8]) - (_data[5] * _data[6])) / determinant;
      inverseData[4] =  ((_data[0] * _data[8]) - (_data[6] * _data[2])) / determinant;
      inverseData[5] = -((_data[0] * _data[5]) - (_data[3] * _data[2])) / determinant;
      inverseData[6] =  ((_data[3] * _data[7]) - (_data[6] * _data[4])) / determinant;
      inverseData[7] = -((_data[0] * _data[7]) - (_data[6] * _data[1])) / determinant;
      inverseData[8] =  ((_data[0] * _data[4]) - (_data[1] * _data[3])) / determinant;
      value.from_array (inverseData);
   }

   return result;
}


//! \brief Inverts the current Matrix in place.
inline dmz::Boolean
dmz::Matrix::invert_in_place () { return invert (*this); }


//! Applies yaw to the Matrix in radians.
inline dmz::Matrix &
dmz::Matrix::yaw_in_place (const Float64 Angle) {

   const Float64 AngleCos (cos (Angle));
   const Float64 AngleSin (sin (Angle));

	Float64 tmp1 = (AngleCos * _data[0]) + (AngleSin * _data[6]);
	Float64 tmp2 = (AngleCos * _data[6]) - (AngleSin * _data[0]);

	_data[0] = tmp1; _data[6] = tmp2;

	tmp1 = (AngleCos * _data[1]) + (AngleSin * _data[7]);
	tmp2 = (AngleCos * _data[7]) - (AngleSin * _data[1]);

	_data[1] = tmp1; _data[7] = tmp2;

	tmp1 = (AngleCos * _data[2]) + (AngleSin * _data[8]);
	tmp2 = (AngleCos * _data[8]) - (AngleSin * _data[2]);

	_data[2] = tmp1; _data[8] = tmp2;

	return *this;
}


//! Applies pitch to the Matrix in radians.
inline dmz::Matrix &
dmz::Matrix::pitch_in_place (const Float64 Angle) {

   const Float64 AngleCos (cos (Angle));
   const Float64 AngleSin (sin (Angle));

	Float64 tmp1 = (AngleCos * _data[3]) - (AngleSin * _data[6]);
	Float64 tmp2 = (AngleSin * _data[3]) + (AngleCos * _data[6]);

	_data[3] = tmp1; _data[6] = tmp2;

	tmp1 = (AngleCos * _data[4]) - (AngleSin * _data[7]);
	tmp2 = (AngleSin * _data[4]) + (AngleCos * _data[7]);

	_data[4] = tmp1; _data[7] = tmp2;

	tmp1 = (AngleCos * _data[5]) - (AngleSin * _data[8]);
	tmp2 = (AngleSin * _data[5]) + (AngleCos * _data[8]);

	_data[5] = tmp1; _data[8] = tmp2;

	return *this;
}


//! Applies roll to the Matrix in radians.
inline dmz::Matrix &
dmz::Matrix::roll_in_place (const Float64 Angle) {

   const Float64 AngleCos (cos (Angle));
   const Float64 AngleSin (sin (Angle));

	Float64 tmp1 = (AngleCos * _data[0]) - (AngleSin * _data[3]);
	Float64 tmp2 = (AngleSin * _data[0]) + (AngleCos * _data[3]);

	_data[0] = tmp1; _data[3] = tmp2;

	tmp1 = (AngleCos * _data[1]) - (AngleSin * _data[4]);
	tmp2 = (AngleSin * _data[1]) + (AngleCos * _data[4]);

	_data[1] = tmp1; _data[4] = tmp2;

	tmp1 = (AngleCos * _data[2]) - (AngleSin * _data[5]);
	tmp2 = (AngleSin * _data[2]) + (AngleCos * _data[5]);

	_data[2] = tmp1; _data[5] = tmp2;

	return *this;
}


//! Applies transform to Vector.
inline dmz::Vector &
dmz::Matrix::transform_vector (Vector &vec) const {

   Float64 vx (0.0), vy (0.0), vz (0.0);

   vec.get_xyz (vx, vy, vz);

   vec.set_xyz (
      (_data[0] * vx) + (_data[1] * vy) + (_data[2] * vz),
      (_data[3] * vx) + (_data[4] * vy) + (_data[5] * vz),
      (_data[6] * vx) + (_data[7] * vy) + (_data[8] * vz));

   return vec;
}


/*!

\brief Sets Matrix element for a given Row and Column.
\param[in] Row Element row.
\param[in] Col Element colum.
\param[in] Data Element value.
\return Returns dmz::True if element was successfully set.

*/
inline dmz::Boolean
dmz::Matrix::set_element (const Int32 Row, const Int32 Col, const Float64 Data) {

   Boolean result (False);
   Int32 offset = (Row * 3) + Col;
   result = ((Row < 3) && (Row > -1) && (Col < 3) && (Col > -1));

   if (result) { _data[offset] = Data; }

   return result;
}


/*!

\brief Gets Matrix element for a given Row and Column.
\param[in] Row Element row.
\param[in] Col Element colum.
\param[out] data Found element value.
\return Returns dmz::True if element was successfully found.

*/
inline dmz::Boolean
dmz::Matrix::get_element (const Int32 Row, const Int32 Col, Float64 &data) const {

   Boolean result (False);
   Int32 offset = (Row * 3) + Col;
   result = ((Row < 3) && (Row > -1) && (Col < 3) && (Col > -1));

   if (result) { data = _data[offset]; }

   return result;
}


/*!

\brief Sets matrix from an array of 9 dmz::Float64 values.
\param[in] Array contains a row-major 3x3 matrix stored in a dmz::Float64[9] array.

*/
inline void
dmz::Matrix::from_array (const Float64 Array[9]) {

   for (Int32 count = 0; count < 9; count++) {

      _data[count] = Array[count];
   }
}


/*!

\brief Converts the Matrix to an array of 9 dmz::Float64 values.
\param[out] array will contains a row-major 3x3 matrix.

*/
inline void
dmz::Matrix::to_array (Float64 array[9]) const {

   for (Int32 count = 0; count < 9; count++) {

      array[count] = _data[count];
   }
}


/*!

\brief Sets matrix from an array of 9 dmz::Float32.
\param[in] Array contains a row-major 3x3 matrix stored in a dmz::Float32[9] array.

*/
inline void
dmz::Matrix::from_array32 (const Float32 Array[9]) {

   for (Int32 count = 0; count < 9; count++) {

      _data[count] = Float64 (Array[count]);
   }
}


/*!

\brief Converts the Matrix to an array of 9 dmz::Float32 values.
\param[out] array will contains a row-major 3x3 matrix.

*/
inline void
dmz::Matrix::to_array32 (Float32 array[9]) const {

   for (Int32 count = 0; count < 9; count++) {

      array[count] =  Float32 (_data[count]);
   }
}


/*
//! \b Not currently working.
inline void
dmz::Matrix::from_euler_angles (
      const Float64 Rx,
      const Float64 Ry,
      const Float64 Rz) {

   const Float64 A  = cos (Rx);
   const Float64 B  = sin (Rx);
   const Float64 C  = cos (Ry);
   const Float64 D  = sin (Ry);
   const Float64 E  = cos (Rz);
   const Float64 F  = sin (Rz);
   const Float64 AD = A * D;
   const Float64 BD = B * D;

   //Row 1
   _data[0] =  C * E;
   _data[1] = -C * F;
   _data[2] =  D;

   //Row 2
   _data[3] =  (BD * E) + (A * F);
   _data[4] = (-BD * F) + (A * E);
   _data[5] =  -B * C;

   //Row 3
   _data[6] = (-AD * E) + (B * F);
   _data[7] =  (AD * F) + (B * E);
   _data[8] =   A * C;
}


//! \b Not currently working.
inline void
dmz::Matrix::to_euler_angles (Float64 &rx, Float64 &ry, Float64 &rz)  const {

   // Calculate Y-axis angle
   ry = asin (_data[2]);

   const Float64 C (cos (ry));

   // Gimbal lock?
   if (fabs (C) > Epsilon64) {

      // No, so get X-axis angle
      rx = atan2 (-_data[5] / C, _data[8] / C);
      // Get Z-axis angle
      rz = atan2 (-_data[1] / C, _data[0] / C);
   }
   else {
      // Gimbal lock has occurred

      // Set X-axis angle to zero and calculate Z-axis angle
      rx  = 0;
      rz = atan2 (_data[3], _data[4]);
   }

   // return only positive angles in [0,2*PI]
   if (rx < 0.0) { rx += TwoPi64; }
   if (ry < 0.0) { ry += TwoPi64; }
   if (rz < 0.0) { rz += TwoPi64; }
}
*/


//! Sets Matrix from arbitrary axis and rotation angle in radians.
inline void
dmz::Matrix::from_axis_and_angle (
      const Vector &Axis,
      const Float64 AngleRadians) {

   Float64 x (0.0), y (0.0), z (0.0);

   Vector axisNorm (Axis);

   if (fabs (Axis.magnitude_squared () - 1.0) > Epsilon64) {

      axisNorm.normalize_in_place ();
   }

   axisNorm.get_xyz (x, y, z);

   const Float64 AngleSin = sin (AngleRadians);
   const Float64 AngleCos = cos (AngleRadians);
   const Float64 OneMinusAngleCos (1.0 - AngleCos);

   _data[0] =      (AngleCos) + (x * x * OneMinusAngleCos);
   _data[1] = (-z * AngleSin) + (x * y * OneMinusAngleCos);
   _data[2] =  (y * AngleSin) + (x * z * OneMinusAngleCos);
   _data[3] =  (z * AngleSin) + (y * x * OneMinusAngleCos);
   _data[4] =      (AngleCos) + (y * y * OneMinusAngleCos);
   _data[5] = (-x * AngleSin) + (y * z * OneMinusAngleCos);
   _data[6] = (-y * AngleSin) + (z * x * OneMinusAngleCos);
   _data[7] =  (x * AngleSin) + (z * y * OneMinusAngleCos);
   _data[8] =      (AngleCos) + (z * z * OneMinusAngleCos);
}


//! Converts matrix to arbitrary axis and an angle of rotation in radians.
inline void
dmz::Matrix::to_axis_and_angle (Vector &axis, Float64 &angleRadians) const {

   Float64 resultAngle (0.0), resultX (0.0), resultY (0.0), resultZ (0.0);

   if ((fabs(_data[1]-_data[3]) < Epsilon64) && //Check for symmetric matrix
      (fabs(_data[2]-_data[6]) < Epsilon64) &&
      (fabs(_data[5]-_data[7]) < Epsilon64)) {

      // singularity found
      // first check for identity matrix which must have +1 for all
      // terms in leading diagonal and zero in other terms

      if (is_identity ()) {
         // Singularity: angle = 0

         // Zero angle, Arbitrary axis
         resultX = 1.0;
         resultY = 0.0;
         resultZ = 0.0;
         resultAngle = 0.0;
      }
      else {
         // Singularity: angle = 180
         resultAngle = Pi64;

         resultX = (_data[0] + 1.0) / 2.0;

         if (resultX > 0.0) { resultX = sqrt(resultX); }  // Should always be true for
                                                          // orthogonal matrix
         else { resultX = 0.0; }// in case matrix has become de-orthogonalized

         resultY = (_data[4] + 1.0) / 2.0;
         if (resultY > 0) { resultY = sqrt(resultY); }
         else { resultY = 0.0; } // in case matrix has become de-orthogonalized

         resultZ = (_data[8] + 1.0) / 2.0;
         if (resultZ > 0.0) { resultZ = sqrt(resultZ); }
         else { resultZ = 0.0; } // in case matrix has become de-orthogonalized

         Boolean xZero = (fabs(resultX) < Epsilon64);
         Boolean yZero = (fabs(resultY) < Epsilon64);
         Boolean zZero = (fabs(resultZ) < Epsilon64);
         Boolean xyPositive = (_data[1] > 0.0);
         Boolean xzPositive = (_data[2] > 0.0);
         Boolean yzPositive = (_data[5] > 0.0);

         if (xZero && !yZero && !zZero && !yzPositive) { resultY = -resultY; }
         else if (yZero && !zZero && !xzPositive) { resultZ = -resultZ; }
         else if (zZero && !xyPositive) { resultX = -resultX; }
      }
   }
   else {

      Float64 s = sqrt (
         ((_data[7] - _data[5]) * (_data[7] - _data[5])) +
         ((_data[2] - _data[6]) * (_data[2] - _data[6])) +
         ((_data[3] - _data[1]) * (_data[3] - _data[1]))); // Used to Normalize

      // This prevents a divide by zero, should not happen if matrix is orthogonal
      // and should be caught by singularity test above, but I've left it in just in case
      if (fabs(s) < Epsilon64) { s = 1.0; }

      resultAngle = acos ((_data[0] + _data[4] + _data[8] - 1.0) / 2.0);
      resultX = (_data[7] - _data[5]) / s;
      resultY = (_data[2] - _data[6]) / s;
      resultZ = (_data[3] - _data[1]) / s;
   }

   axis.set_xyz (resultX, resultY, resultZ);
   angleRadians = resultAngle;
}


/*!

\brief Creates Matrix that will transform one Vector to another.
\param[in] FromValue The Vector to transform from.
\param[in] ToValue The Vector to transform to.

*/
inline void
dmz::Matrix::from_two_vectors (const Vector &FromValue, const Vector &ToValue) {

   const Vector Cross (FromValue.cross (ToValue).normalize ());
   const Float64 Angle (ToValue.get_angle (FromValue));

   from_axis_and_angle (Cross, Angle);
}


//! Returns dmz::True if Matrix is an identity Matrix.
inline dmz::Boolean
dmz::Matrix::is_identity () const {

   return
      is_zero64 (_data[0] - 1.0) &&
      is_zero64 (_data[1]) &&
      is_zero64 (_data[2]) &&
      is_zero64 (_data[3]) &&
      is_zero64 (_data[4] - 1.0) &&
      is_zero64 (_data[5]) &&
      is_zero64 (_data[6]) &&
      is_zero64 (_data[7]) &&
      is_zero64 (_data[8] - 1.0);
}


//! Sets the Matrix to be an identity Matrix.
inline void
dmz::Matrix::set_identity () {

   _data[0] = 1.0;
   _data[1] = 0.0;
   _data[2] = 0.0;
   _data[3] = 0.0;
   _data[4] = 1.0;
   _data[5] = 0.0;
   _data[6] = 0.0;
   _data[7] = 0.0;
   _data[8] = 1.0;
}

#endif // DMZ_TYPES_MATRIX_DOT_H
