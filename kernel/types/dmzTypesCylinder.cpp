#include <dmzTypesCylinder.h>
#include <dmzTypesVector.h>

/*!

\class dmz::Cylinder
\ingroup Types
\brief Provides a cylinder implementation of the Volume base class.
\details The Cylinder origin is either at the bottom or top of the Volume depending
on if the height is positive or negative. If the height is positive, the origin is
at the base of the cylinder. If the height is negative, the origin is at the top of the
cylinder. A cylinder may be aligned with the x, y, or z axis. A cylinder with a
height of zero is a special case where the cylinder is unbounded  along its
axis of alignment. The radius must be greater than zero.

*/

struct dmz::Cylinder::State {

   Vector origin;
   Float64 radius;
   Float64 radiusSquared;
   Float64 height;
   VectorComponentEnum axis;

   State &operator= (const State &Value) {

      origin = Value.origin;
      radius = Value.radius;
      radiusSquared = Value.radiusSquared;
      height = Value.height;
      axis = Value.axis;

      return *this;
   }

   Boolean operator== (const State &Value) const {

      return (origin == Value.origin) &&
         is_zero64 (radius - Value.radius) &&
         is_zero64 (height - Value.height) &&
         (axis == Value.axis);
   }

   State () : radius (1.0), radiusSquared (1.0), height (1.0), axis (VectorComponentY) {;}

   State (const State &Value) :
         radius (1.0),
         radiusSquared (1.0),
         height (1.0),
         axis (VectorComponentY) { *this = Value; }
};


/*! 

\brief Base constructor.
\details The radius and height default to 1.0 and the axis defaults to
dmz::VectorComponentY.

*/
dmz::Cylinder::Cylinder () : _state (*(new State)) {;}


//! Copy constructor.
dmz::Cylinder::Cylinder (const Cylinder &Value) : _state (*(new State (Value._state))) {;}


/*!

\brief Constructor
\param[in] Origin Vector containing the origin of the cylinder.
\param[in] Radius Scalar value containing the radius of the cylinder.
\param[in] Height Scalar value containing the height of the cylinder.
\param[in] Axis VectorComponentEnum specifying the cylinders axis of alignment.

*/
dmz::Cylinder::Cylinder (
      const Vector &Origin,
      const Float64 Radius,
      const Float64 Height,
      const VectorComponentEnum Axis) :
      _state (*(new State)) {

   _state.origin = Origin;
   _state.radius = (Radius > 0.0 ? Radius : 1.0);
   _state.radiusSquared = (Radius > 0.0 ? Radius * Radius : 1.0);
   _state.height = Height;
   _state.axis = Axis;
}


//! Destructor.
dmz::Cylinder::~Cylinder () { delete &_state; }


void
dmz::Cylinder::set_origin (const Vector &Origin) { _state.origin = Origin; }


dmz::Vector
dmz::Cylinder::get_origin () const { return _state.origin; }


dmz::Boolean
dmz::Cylinder::contains_point (const Vector &Point) const {

   Boolean result (True);

   if (_state.height != 0.0) {

      Float64 min (0.0);
      Float64 max (0.0);

      if (_state.height > 0.0 ) {

         min = _state.origin.get (_state.axis);
         max = _state.height;
      }
      else {

         min = _state.height;
         max = _state.origin.get (_state.axis);
      }

      const Float64 Height = Point.get (_state.axis);

      if ((Height > max) || (Height < min)) { result = False; }
   }

   if (result) {

      Vector p (Point);
      Vector o (_state.origin);

      p.set (_state.axis, 0.0);
      o.set (_state.axis, 0.0);

      if ((p - o).magnitude_squared () > _state.radiusSquared) { result = False; }
   }

   return result;
}


void
dmz::Cylinder::get_extents (Vector &origin, Vector &min, Vector &max) const {

   origin = _state.origin;
   min = _state.origin - _state.radius;
   max = _state.origin + _state.radius;

   if (_state.height > 0.0) {

      min.set (_state.axis, _state.origin.get (_state.axis));
      max.set (_state.axis, _state.origin.get (_state.axis) + _state.height);
   }
   else if (_state.height < 0.0) {

      min.set (_state.axis, _state.origin.get (_state.axis) + _state.height);
      max.set (_state.axis, _state.origin.get (_state.axis));
   }
   else { min.set (_state.axis, 0.0); max.set (_state.axis, 0.0); }
}


//! Assignment operator.
dmz::Cylinder &
dmz::Cylinder::operator= (const Cylinder &Value) { _state = Value._state; return *this; }


//! Relational "equal to" operator.
dmz::Boolean
dmz::Cylinder::operator== (const Cylinder &Value) const { return _state == Value._state; }


/*!

\brief Sets the radius.
\note The radius must be greater than zero.
\param[in] Radius Scalar value containing the new radius.

*/
void
dmz::Cylinder::set_radius (const Float64 Radius) {

   if (Radius > 0.0) {

      _state.radius = Radius;
      _state.radiusSquared = Radius * Radius;
   }
}


//! Gets the current radius value.
dmz::Float64
dmz::Cylinder::get_radius () const { return _state.radius; }


/*!

\brief Sets the height.
\note If the height is set to zero, the cylinder is defined to be unbounded
along its axis of alignment.
\param[in] Height Scalar value containing the new height.

*/
void
dmz::Cylinder::set_height (const Float64 Height) { _state.height = Height; }


//! Gets the current height value.
dmz::Float64
dmz::Cylinder::get_height () const { return _state.height; }


//! Sets the axis of alignment for the cylinder.
void
dmz::Cylinder::set_axis (const VectorComponentEnum Axis) { _state.axis = Axis; }


//! Gets the axis of alignment for the cylinder.
dmz::VectorComponentEnum
dmz::Cylinder::get_axis () const { return _state.axis; }

