#include "dmzEntityPluginRandomizePosition.h"
#include <dmzObjectModule.h>
#include <dmzRenderIsectUtil.h>
#include <dmzRenderModuleIsect.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMask.h>
#include <dmzTypesMath.h>
#include <dmzTypesMatrix.h>

#include <stdlib.h>

dmz::EntityPluginRandomizePosition::EntityPluginRandomizePosition (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _isect (0),
      _defaultAttrHandle (0),
      _radius (100.0),
      _heading (-(8.0 * TwoPi64)) {

   _init (local);
}


dmz::EntityPluginRandomizePosition::~EntityPluginRandomizePosition () {

}


// Plugin Interface
void
dmz::EntityPluginRandomizePosition::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::EntityPluginRandomizePosition::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_isect) { _isect = RenderModuleIsect::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_isect && (_isect == RenderModuleIsect::cast (PluginPtr))) { _isect = 0; }
   }
}


// Object Observer Interface
void
dmz::EntityPluginRandomizePosition::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   ObjectModule *objMod (get_object_module ());

   if (objMod && Value) {

      Vector tmp;

      if (!objMod->lookup_position (ObjectHandle, _defaultAttrHandle, tmp)) {

         const Vector Up (0.0, 1.0, 0.0);
         const Float64 Angle = (Float64 (rand ()) / Float64 (RAND_MAX)) * TwoPi64;
         const Matrix Rot (Up, Angle);
         Vector offset (0.0, 0.0, -(Float64 (rand ()) / Float64 (RAND_MAX)) * _radius);
         Rot.transform_vector (offset);
         Vector pos (_center + offset);
         Matrix dir (Up, _heading);
         Matrix tilt;

         if (_isect) {

            Vector normal;

            if (isect_clamp_point (pos, *_isect, pos, normal)) {

               tilt.from_two_vectors (Up, normal);
            }
         }

         objMod->store_position (ObjectHandle, _defaultAttrHandle, pos);
         objMod->store_orientation (ObjectHandle, _defaultAttrHandle, dir);
      }
   }
}


// Protected Functions
void
dmz::EntityPluginRandomizePosition::_init (Config &local) {

   Mask EmptyMask;
   _defaultAttrHandle = activate_default_object_attribute (EmptyMask);

   activate_hil_object_attribute ();

   _center = config_to_vector ("center", local, _center);
   _radius = config_to_float64 ("radius.value", local, _radius);
   _heading = config_to_float64 ("heading.value", local, _heading);

   if (_heading < -(TwoPi64 * 7.0)) {

      _heading = to_radians (config_to_float64 ("heading.degrees", local));
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginRandomizePosition (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginRandomizePosition (Info, local);
}

};
