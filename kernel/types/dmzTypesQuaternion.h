#ifndef DMZ_TYPES_QUATERNION_DOT_H
#define DMZ_TYPES_QUATERNION_DOT_H

#include <dmzTypesBase.h>
#include <dmzTypesConsts.h>
#include <dmzTypesVector.h>

#include <math.h>

namespace dmz {

   class Matrix;

   class Quaternion {

      public:
         Quaternion ();
         Quaternion (const Matrix &Mat);
         Quaternion (const Float64 Rx, const Float64 Ry, const Float64 Rz);              
         // Euler Angles
         Quaternion (const Vector Axis, const Float64 Theta);
         Quaternion (
            const Float64 X, 
            const Float64 Y, 
            const Float64 Z, 
            const Float64 W);
         Quaternion (const Quaternion &Quat);
         ~Quaternion () {;}

         Quaternion &operator= (const Quaternion &Quat);
         Quaternion &operator= (const Matrix &Mat);

         Quaternion &operator*= (const Quaternion &Quat);
         Quaternion operator* (const Quaternion &Quat) const;

         Boolean operator== (const Quaternion &Quat) const;
         Boolean operator!= (const Quaternion &Quat) const;

         DMZ_KERNEL_LINK_SYMBOL Quaternion &from_matrix (const Matrix &Mat);
         DMZ_KERNEL_LINK_SYMBOL void to_matrix (Matrix &Mat);

         void set_x (const Float64 X);
         Float64 get_x () const;
         void set_y (const Float64 Y);
         Float64 get_y () const;
         void set_z (const Float64 Z);
         Float64 get_z () const;
         void set_w (const Float64 W);
         Float64 get_w () const;

         void set_xyzw (
            const Float64 X, 
            const Float64 Y, 
            const Float64 Z, 
            const Float64 W);
         void get_xyzw (Float64 &x, Float64 &y, Float64 &z, Float64 &w) const;

         void multiply_angle (const Float64 Factor);
         Quaternion invert () const;
         Quaternion &invert_in_place ();

         void transform_vector (Vector &Vec) const; 
         // test this against the matrix a lot.  I might have done it wrong.

         Quaternion &from_axis_and_angle_radians (
            const Vector &Axis, 
            const Float64 &Theta);
         void to_axis_and_angle_radians (Vector &axis, Float64 &theta) const; 
         // this might not work at all.  Test it with vigor.

         DMZ_KERNEL_LINK_SYMBOL void from_matrix_array (const Float64 data[9]);
         DMZ_KERNEL_LINK_SYMBOL void to_matrix_array (Float64 data[9]) const;
         DMZ_KERNEL_LINK_SYMBOL void from_matrix_array32 (const Float32 data[9]);
         DMZ_KERNEL_LINK_SYMBOL void to_matrix_array32 (Float32 data[9]) const;

      protected:
         void _normalize_in_place ();

         Float64 _x, _y, _z, _w;
   };
};


inline
dmz::Quaternion::Quaternion () : _x (1.0), _y (1.0), _z (1.0), _w (0.0) { 
   
   _normalize_in_place (); 
}


inline
dmz::Quaternion::Quaternion (
      const Float64 Rx, 
      const Float64 Ry, 
      const Float64 Rz) :
      _x (1.0), _y (1.0), _z (1.0), _w (0.0) {

   Quaternion pitch (Vector (1.0, 0.0, 0.0), Rx); 
   Quaternion yaw   (Vector (0.0, 1.0, 0.0), Ry); 
   Quaternion roll  (Vector (0.0, 0.0, 1.0), Rz);

   *this = (pitch * yaw) * roll;

   _normalize_in_place (); 
}


inline
dmz::Quaternion::Quaternion (const Matrix &Mat) {

   from_matrix (Mat);
}


inline
dmz::Quaternion::Quaternion (const Vector Axis, const Float64 Theta) {

   from_axis_and_angle_radians (Axis, Theta);
}


inline
dmz::Quaternion::Quaternion (
      const Float64 X, 
      const Float64 Y, 
      const Float64 Z, 
      const Float64 W) : 
      _x (X), _y (Y), _z (Z), _w (W) {

   _normalize_in_place (); 
}


inline
dmz::Quaternion::Quaternion (const Quaternion &Quat) : 
      _x (Quat._x), _y (Quat._y), _z (Quat._z), _w (Quat._w) {;}


inline dmz::Quaternion &
dmz::Quaternion::operator= (const Quaternion &Quat) {

   _x = Quat._x;
   _y = Quat._y;
   _z = Quat._z;
   _w = Quat._w;

   return *this;
}


inline dmz::Quaternion &
dmz::Quaternion::operator= (const Matrix &Mat) {

   return from_matrix (Mat);
}


inline dmz::Quaternion &
dmz::Quaternion::operator*= (const Quaternion &Quat) {

   Quaternion result;

   result._w = (_w * Quat._w) - (_x * Quat._x) - (_y * Quat._y) - (_z * Quat._z);
   result._x = (_w * Quat._x) + (_x * Quat._w) + (_y * Quat._z) - (_z * Quat._y);
   result._y = (_w * Quat._y) + (_y * Quat._w) + (_z * Quat._x) - (_x * Quat._z);
   result._z = (_w * Quat._z) + (_z * Quat._w) + (_x * Quat._y) - (_y * Quat._x);

   _w = result._w;
   _x = result._x;
   _y = result._y;
   _z = result._z;

   return *this;
}

inline dmz::Quaternion 
dmz::Quaternion::operator* (const Quaternion &Quat) const {

   Quaternion result (*this);

   return (result *= Quat);
}

inline dmz::Boolean 
dmz::Quaternion::operator== (const Quaternion &Quat) const {

   Boolean result (False);

   const Float64 DiffX (Quat._x - _x);
   const Float64 DiffY (Quat._y - _y);
   const Float64 DiffZ (Quat._z - _z);
   const Float64 DiffW (Quat._w - _w);

   if (
         (DiffX < Epsilon64) && (DiffX > -Epsilon64) &&
         (DiffY < Epsilon64) && (DiffY > -Epsilon64) &&
         (DiffZ < Epsilon64) && (DiffZ > -Epsilon64) &&
         (DiffW < Epsilon64) && (DiffW > -Epsilon64)) { result = True; }

   return result;
}

inline dmz::Boolean 
dmz::Quaternion::operator!= (const Quaternion &Quat) const { return !(*this == Quat); }

inline void 
dmz::Quaternion::set_x (const Float64 X) { _x = X; }

inline dmz::Float64 
dmz::Quaternion::get_x () const { return _x; }

inline void 
dmz::Quaternion::set_y (const Float64 Y) { _y = Y; }

inline dmz::Float64 
dmz::Quaternion::get_y () const { return _y; }

inline void 
dmz::Quaternion::set_z (const Float64 Z) { _z = Z; }

inline dmz::Float64 
dmz::Quaternion::get_z () const { return _z; }

inline void 
dmz::Quaternion::set_w (const Float64 W) { _w = W; }

inline dmz::Float64 
dmz::Quaternion::get_w () const { return _w; }

inline void 
dmz::Quaternion::set_xyzw (const Float64 X, 
                           const Float64 Y, 
                           const Float64 Z, 
                           const Float64 W) {

   _x = X;
   _y = Y;
   _z = Z;
   _w = W;
}

inline void 
dmz::Quaternion::get_xyzw (
      Float64 &x, 
      Float64 &y, 
      Float64 &z, 
      Float64 &w) const {

   x = _x;
   y = _y;
   z = _z;
   w = _w;
}

inline void
dmz::Quaternion::multiply_angle (const Float64 Factor) {

   Vector axis;
   Float64 angle (0);

   to_axis_and_angle_radians (axis, angle);

   angle *= Factor;

   from_axis_and_angle_radians (axis, angle);
}

inline dmz::Quaternion 
dmz::Quaternion::invert () const {

   Quaternion result;

   result._w = _w;
   result._x = -_x;
   result._y = -_y;
   result._z = -_z;

   return result;
}

inline dmz::Quaternion &
dmz::Quaternion::invert_in_place () {

   _x = -_x;
   _y = -_y;
   _z = -_z;
   return *this;
}

inline void 
dmz::Quaternion::transform_vector (Vector &Vec) const {

   Float64 mag (Vec.magnitude ());
   Quaternion targetVector (Vec.get_x (), Vec.get_y (), Vec.get_z (), 0.0);
   
   Quaternion trans (*this);
   Quaternion inv (invert ());

   Quaternion result = trans * targetVector * inv;

   Vec.set_xyz (result._x, result._y, result._z);
   Vec *= mag;

//   Vector trans (_x, _y, _z);
//   Vector transInv (inv._x, inv._y, inv._z);
//   Vector result = trans * Vec * transInv;
//   Vec = result;
}

inline dmz::Quaternion &
dmz::Quaternion::from_axis_and_angle_radians (
      const Vector &Axis, 
      const Float64 &Theta) {

   Vector axisNormalized (Axis);

   if ( fabs (axisNormalized.magnitude_squared () - 1.0) > Epsilon64) {
      
      axisNormalized.normalize_in_place ();
   }

   _w = cos (0.5 * Theta);

   const Float64 SinPoint5Theta (sin (0.5 * Theta));

   _x = SinPoint5Theta * axisNormalized.get_x ();
   _y = SinPoint5Theta * axisNormalized.get_y ();
   _z = SinPoint5Theta * axisNormalized.get_z ();
   
   _normalize_in_place ();

   return *this;
}

inline void 
dmz::Quaternion::to_axis_and_angle_radians (Vector &axis, Float64 &theta) const {

   Quaternion temp (*this);
   temp._normalize_in_place ();

   theta = acos ( temp._w ) * 2.0;

   Float64 sin_a = sqrt (1.0 - temp._w * temp._w);

   if (fabs (sin_a) < Epsilon64) sin_a = 1.0;

   axis.set_xyz (
         temp._x / sin_a,
         temp._y / sin_a, 
         temp._z / sin_a);
}

inline void 
dmz::Quaternion::_normalize_in_place () {

   const Float64 Magnitude = sqrt ((_x * _x) + (_y * _y) + (_z * _z) + (_w * _w));

   _x /= Magnitude;
   _y /= Magnitude;
   _z /= Magnitude;
   _w /= Magnitude;
}

#endif // DMZ_TYPES_VECTOR_DOT_H
