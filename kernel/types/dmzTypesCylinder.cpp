#include <dmzTypesCylinder.h>
#include <dmzTypesVector.h>

#include <math.h>

struct dmz::Cylinder::State {

   Vector origin;
   Float64 radius;
   Float64 radiusSquared;
   Float64 height;
   VectorComponentEnum axis;

   State () : radius (1.0), radiusSquared (1.0), height (1.0), axis (VectorComponentY) {;}
};

dmz::Cylinder::Cylinder () : _state (*(new State)) {;}


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
dmz::Cylinder::get_extents (Vector &min, Vector &max) const {

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


void
dmz::Cylinder::set_radius (const Float64 Radius) {

   if (Radius > 0.0) {

      _state.radius = Radius;
      _state.radiusSquared = Radius * Radius;
   }
}


dmz::Float64
dmz::Cylinder::get_radius () const { return _state.radius; }


void
dmz::Cylinder::set_height (const Float64 Height) { _state.height = Height; }


dmz::Float64
dmz::Cylinder::get_height () const { return _state.height; }


void
dmz::Cylinder::set_axis (const VectorComponentEnum Axis) { _state.axis = Axis; }


dmz::VectorComponentEnum
dmz::Cylinder::get_axis () const { return _state.axis; }

