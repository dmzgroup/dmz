#include <dmzTypesCylinder.h>
#include <dmzTypesVector.h>

#include <math.h>

struct dmz::Cylinder::State {

   Vector origin;
   Float64 radiusSquared;
};

dmz::Cylinder::Cylinder () : _state (*(new State)) {;}


dmz::Cylinder::Cylinder (const Vector &Origin, const Float64 Radius) :
      _state (*(new State)) {

   _state.origin = Origin;
   _state.radiusSquared = (Radius > 0.0 ? Radius * Radius : 1.0);
}


dmz::Cylinder::~Cylinder () { delete &_state; }


void
dmz::Cylinder::set_origin (const Vector &Origin) { _state.origin = Origin; }


dmz::Vector
dmz::Cylinder::get_origin () { return _state.origin; }


dmz::Boolean
dmz::Cylinder::contains_point (const Vector &Point) {

   return (Point - _state.origin).magnitude_squared () <= _state.radiusSquared;
}


void
dmz::Cylinder::set_radius (const Float64 Radius) {

   if (Radius > 0.0) { _state.radiusSquared = Radius * Radius; }
}


dmz::Float64
dmz::Cylinder::get_radius () { return sqrt (_state.radiusSquared); }

