#ifndef DMZ_TYPES_VECTOR_DOT_H
#define DMZ_TYPES_VECTOR_DOT_H

#include <dmzTypesBase.h>
#include <dmzTypesConsts.h>
#include <math.h>

namespace dmz {

//! \brief Vector component enumeration.
//! \ingroup Types
//! \sa dmz::Vector::set() \n dmz::Vector::get()
enum VectorComponentEnum {
   VectorComponentX, //!< The X component.
   VectorComponentY, //!< The Y component.
   VectorComponentZ, //!< The Z component.
};

/*!

\brief 3D Vector class.
\ingroup Types
\details Standard 3D Vector class. Each of the three components are a 64 bit floating
point number.
\htmlonly Lua bindings are <a href="dmzlua.html#dmz.vector">available</a>.
\endhtmlonly

*/
class Vector {

   public:
      Vector ();
      Vector (const Float64 X, const Float64 Y, const Float64 Z);
      Vector (const Vector &Vec);
      ~Vector () {;}

      Vector &operator= (const Vector &Vec);

      Vector &operator+= (const Vector &Vec);
      Vector &operator+= (const Float64 Value);
      Vector operator+ (const Vector &Vec) const;
      Vector operator+ (const Float64 Value) const;

      Vector operator- () const;

      Vector &operator-= (const Vector &Vec);
      Vector &operator-= (const Float64 Value);
      Vector operator- (const Vector &Vec) const;
      Vector operator- (const Float64 Value) const;

      Vector &operator*= (const Vector &Vec);
      Vector &operator*= (const Float64 Value);
      Vector operator* (const Vector &Vec) const;
      Vector operator* (const Float64 Value) const;

      Boolean operator== (const Vector &Vec) const;
      Boolean operator!= (const Vector &Vec) const;

      void set_x (const Float64 X);
      void set_y (const Float64 Y);
      void set_z (const Float64 Z);
      void set_xyz (const Float64 X, const Float64 Y, const Float64 Z);
      void set (const VectorComponentEnum Component, const Float64 Value);

      Float64 get_x () const;
      Float64 get_y () const;
      Float64 get_z () const;
      void get_xyz (Float64 &x, Float64 &y, Float64 &z) const;
      Float64 get (const VectorComponentEnum Component) const;

      Float64 magnitude_squared () const;
      Float64 magnitude () const;
      Vector &normalize_in_place ();
      Vector normalize () const;
      Float64 dot (const Vector &Vec) const;
      Vector &cross_in_place (const Vector &Vec);
      Vector cross (const Vector &Vec) const;

      Float64 get_angle (const Vector &Vec) const;
      Boolean is_zero () const;
      Boolean is_zero (const Float64 Epsilon) const;

   protected:
      //! \cond
      Float64 _x;
      Float64 _y;
      Float64 _z;
      //! \endcond
}; // class Vector

}; // namespace dmz


/*!

\brief Default constructor.
\details All components are initialized to zero.

*/
inline
dmz::Vector::Vector () : _x (0.0), _y (0.0), _z (0.0) {;}


/*!

\brief Component constructor.
\param[in] X component.
\param[in] Y component.
\param[in] Z component.

*/
inline
dmz::Vector::Vector (const Float64 X, const Float64 Y, const Float64 Z) :
      _x (X),
      _y (Y),
      _z (Z) {;}


//! Copy constructor.
inline
dmz::Vector::Vector (const Vector &Vec) : _x (Vec._x), _y (Vec._y), _z (Vec._z) {;}


//! Assignment operator
inline dmz::Vector &
dmz::Vector::operator= (const Vector &Vec) {

   _x = Vec._x;
   _y = Vec._y;
   _z = Vec._z;

   return *this;
}


//! Assignment by vector sum operator
inline dmz::Vector &
dmz::Vector::operator+= (const Vector &Vec) {

   _x += Vec._x;
   _y += Vec._y;
   _z += Vec._z;

   return *this;
}


//! Assignment by scalar sum operator
inline dmz::Vector &
dmz::Vector::operator+= (const Float64 Value) {

   _x += Value;
   _y += Value;
   _z += Value;

   return *this;
}


//! Vector addition operator.
inline dmz::Vector
dmz::Vector::operator+ (const Vector &Vec) const {

   Vector result (*this);
   return result += Vec;
}


//! Scalar addition operator.
inline dmz::Vector
dmz::Vector::operator+ (const Float64 Value) const {

   Vector result (*this);
   return result += Value;
}


/*!

\brief Unary minus operator.
\details Negates the Vector.

*/
inline dmz::Vector
dmz::Vector::operator- () const {

   Vector result (-_x, -_y, -_z);
   return result;
}


//! Assignment by vector difference operator.
inline dmz::Vector &
dmz::Vector::operator-= (const Vector &Vec) {

   _x -= Vec._x;
   _y -= Vec._y;
   _z -= Vec._z;

   return *this;
}


//! Assignment by scalar difference operator.
inline dmz::Vector &
dmz::Vector::operator-= (const Float64 Value) {

   _x -= Value;
   _y -= Value;
   _z -= Value;

   return *this;
}


//! Subtraction Vector operator.
inline dmz::Vector
dmz::Vector::operator- (const Vector &Vec) const {

   Vector result (*this);
   return result -= Vec;
}


//! Subtraction scalar operator.
inline dmz::Vector
dmz::Vector::operator- (const Float64 Value) const {

   Vector result (*this);
   return result -= Value;
}


//! Assignment by vector multiplication operator.
inline dmz::Vector &
dmz::Vector::operator*= (const Vector &Vec) {

   _x *= Vec._x;
   _y *= Vec._y;
   _z *= Vec._z;

   return *this;
}


//! Assignment by scalar multiplication operator.
inline dmz::Vector &
dmz::Vector::operator*= (const Float64 Value) {

   _x *= Value;
   _y *= Value;
   _z *= Value;

   return *this;
}


//! Vector multiplication operator.
inline dmz::Vector
dmz::Vector::operator* (const Vector &Vec) const {

   Vector result (*this);
   return result *= Vec;
}


//! Scalar multiplication operator.
inline dmz::Vector
dmz::Vector::operator* (const Float64 Value) const {

   Vector result (*this);
   return result *= Value;
}


/*!

\brief Relational "equal to" operator.
\details This function subtracts the left hand vector from the right hand vector and then
tests each resulting value to see if it near enough to zero to be considered zero.
\param[in] Vec Right hand value.
\return Returns dmz::True if the two vectors are close enough in value to be considered
the same Vector.
\sa dmz::is_zero64

*/
inline dmz::Boolean
dmz::Vector::operator== (const Vector &Vec) const {

   return is_zero64 (Vec._x - _x) && is_zero64 (Vec._y - _y) && is_zero64 (Vec._z - _z);
}


/*!

\brief Relational "not equal to" operator.
\details This function applies the logical not operator to the result of the relational
"equal to" operator.
\param[in] Vec Right hand value.
\return Returns dmz::True if the two vectors are \b not close enough in value to be
considered the same Vector.
\sa dmz::Vector::operator==()

*/
inline dmz::Boolean
dmz::Vector::operator!= (const Vector &Vec) const { return !(*this == Vec); }


//! Set the X component.
inline void
dmz::Vector::set_x (const Float64 X) { _x = X; }


//! Set the Y component.
inline void
dmz::Vector::set_y (const Float64 Y) { _y = Y; }


//! Set the Z component.
inline void
dmz::Vector::set_z (const Float64 Z) { _z = Z; }


/*!

\brief Sets  the X, Y, and Z components of the Vector.
\param[in] X component.
\param[in] Y component.
\param[in] Z component.

*/
inline void
dmz::Vector::set_xyz (const Float64 X, const Float64 Y, const Float64 Z) {

   _x = X;
   _y = Y;
   _z = Z;
}


/*!

\brief Sets the specified Vector component.
\param[in] Component VectorComponentEnum specifying the component to set.
\param[in] Value Float64 containing the new value of the component.

*/
inline void
dmz::Vector::set (const VectorComponentEnum Component, const Float64 Value) {

   if (Component == VectorComponentX) { _x = Value; }
   else if (Component == VectorComponentY) { _y = Value; }
   else if (Component == VectorComponentZ) { _z = Value; }
}


//! Returns the X component.
inline dmz::Float64
dmz::Vector::get_x () const { return _x; }


//! Returns the Y component.
inline dmz::Float64
dmz::Vector::get_y () const { return _y; }


//! Returns the Z component.
inline dmz::Float64
dmz::Vector::get_z () const { return _z; }


/*!

\brief Returns the X, Y, and Z components of the Vector.
\param[out] x component.
\param[out] y component.
\param[out] z component.

*/
inline void
dmz::Vector::get_xyz (Float64 &x, Float64 &y, Float64 &z) const {

   x = _x;
   y = _y;
   z = _z;
}


/*!

\brief Return the specified Vector component.
\param[in] Component VectorComponentEnum specifying the component to return.
\return Returns the component as a Float64.

*/
inline dmz::Float64
dmz::Vector::get (const VectorComponentEnum Component) const {

   return
      ((Component == VectorComponentX) ? _x : (Component == VectorComponentY) ? _y : _z);
}

//! Returns the squared magnitude of the Vector.
inline dmz::Float64
dmz::Vector::magnitude_squared () const {

   return (_x * _x) + (_y * _y) + (_z * _z);
}


//! Returns the magnitude of the Vector.
inline dmz::Float64
dmz::Vector::magnitude () const {

   const Float64 Result (sqrt (magnitude_squared ()));
   return (Result > Epsilon64) ? Result : 0.0;
}


/*!

\brief Normalizes the vector.
\return Returns reference to self.

*/
inline dmz::Vector &
dmz::Vector::normalize_in_place () {

   const Float64 Magnitude (magnitude ());

   if (Magnitude > 0.0) { *this *= 1.0 / Magnitude; }
   else { set_xyz (0.0, 0.0, 0.0); }

   return *this;
}


/*!

\brief Returns a normalized vector.
\return Returns a Vector containing the normalized value.

*/
inline dmz::Vector
dmz::Vector::normalize () const {

   Vector result (*this);
   return result.normalize_in_place ();
}

//! Returns the dot product of the two vectors.
inline dmz::Float64
dmz::Vector::dot (const Vector &Vec) const {

   return (_x * Vec._x) + (_y * Vec._y) + (_z * Vec._z);
}


/*!

\brief Performs the cross product on the Vector.
\return Returns reference to self.

*/
inline dmz::Vector &
dmz::Vector::cross_in_place (const Vector &Vec) {

   const Float64 NewX = (_y * Vec._z) - (_z * Vec._y);
   const Float64 NewY = (_z * Vec._x) - (_x * Vec._z);
   const Float64 NewZ = (_x * Vec._y) - (_y * Vec._x);

   _x = NewX;
   _y = NewY;
   _z = NewZ;

   return *this;
}


//! Returns a vector that contains the cross product of the two vectors.
inline dmz::Vector
dmz::Vector::cross (const Vector &Vec) const {

   Vector result (*this);
   return result.cross_in_place (Vec);
}


/*!

\brief Returns the angle between the two given vectors.
\details The function calculates the smallest angle between two Vectors. The result
is always positive and therefor does not indicate direction of rotation of the angle.
\returns Returns the angle in radians.

*/
inline dmz::Float64
dmz::Vector::get_angle (const Vector &Vec) const {

   return atan2 (cross (Vec).magnitude (), dot (Vec));
}


/*!

\brief Tests if Vector is zero.
\details The function dmz::is_zero64 is used to determine if each component is near
enough to zero to be considered zero.
\return Returns dmz::True if the Vector is sufficiently close to zero.

*/
inline dmz::Boolean
dmz::Vector::is_zero () const {

   return is_zero64 (_x) && is_zero64 (_y) && is_zero64 (_z);
}


/*!

\brief Tests if Vector is zero.
\details The function dmz::is_zero64 and \a Epslion are used to determine magnitude
of the Vector is near enough to zero to be considered zero.
\param[in] Epsilon
\return Returns dmz::True if the Vector is sufficiently close to zero.

*/
inline dmz::Boolean
dmz::Vector::is_zero (const Float64 Epsilon) const {

   return is_zero64 (magnitude (), fabs (Epsilon));
}

#endif // DMZ_TYPES_VECTOR_DOT_H
