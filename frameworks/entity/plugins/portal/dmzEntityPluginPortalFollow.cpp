#include "dmzEntityPluginPortalFollow.h"
#include <dmzInputEventMasks.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::EntityPluginPortalFollow
\ingroup Entity
\brief Follows human-in-th-loop object with the render and audio portals.

*/

//! \cond
dmz::EntityPluginPortalFollow::EntityPluginPortalFollow (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      InputObserverUtil (Info, local),
      _handle (0),
      _defaultHandle (0),
      _hilHandle (0),
      _offset (0.0, 2.5, 10.0),
      _renderPortal (0),
      _audioPortal (0),
      _lastHeadVec (0.0, 0.0, -1.0),
      _lastPitchVec (0.0, 0.0, -1.0),
      _active (0),
      _log (Info) {

   _init (local);
}


dmz::EntityPluginPortalFollow::~EntityPluginPortalFollow () {

}


// Plugin Interface
void
dmz::EntityPluginPortalFollow::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_renderPortal) { _renderPortal = RenderModulePortal::cast (PluginPtr); }
      if (!_audioPortal) { _audioPortal = AudioModulePortal::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_renderPortal && (_renderPortal == RenderModulePortal::cast (PluginPtr))) { 
      
         _renderPortal = 0; 
      }
      if (_audioPortal && (_audioPortal == AudioModulePortal::cast (PluginPtr))) { 
      
         _audioPortal = 0; 
      }
   }
}


// TimeSlice Interface
void
dmz::EntityPluginPortalFollow::update_time_slice (const Float64 TimeDelta) {

   ObjectModule *objMod (get_object_module ());

   if ((_active > 0) && objMod && _handle && _defaultHandle) {

      Vector pos, vel;
      Matrix ori; ori.set_identity ();

      objMod->lookup_position (_handle, _defaultHandle, pos);
      objMod->lookup_orientation (_handle, _defaultHandle, ori);
      objMod->lookup_velocity (_handle, _defaultHandle, vel);

      const Vector Velocity (vel);
      const Vector SideX (1.0, 0.0, 0.0);
      const Vector UpY (0.0, 1.0, 0.0);
      const Vector ForwardZ (0.0, 0.0, -1.0);
      const Matrix OrigOri (ori);

      if (vel.is_zero ()) {

         vel = ForwardZ;

         ori.transform_vector (vel);
      }

      ori.set_identity ();

      Vector pitchVec (vel);
      const Float64 ValX (pitchVec.get_x ());
      const Float64 ValZ (pitchVec.get_z ());
      pitchVec.set_x (0.0);
      pitchVec.set_z (-(sqrt ((ValX * ValX) + (ValZ * ValZ))));

      const Float64 MaxTurn ((HalfPi64 - 0.1) * TimeDelta);

      Float64 pitch (0.0);

      if (!pitchVec.is_zero ()) {

         pitchVec.normalize_in_place ();

         pitch = ForwardZ.get_angle (_lastPitchVec);
         //if (ForwardZ.cross (_lastPitchVec).get_x () < 0.0) { pitch = -pitch; }
         if (_lastPitchVec.get_y () < 0.0) { pitch = -pitch; }

         Float64 pitchDiff (_lastPitchVec.get_angle (pitchVec));

         if (pitchDiff > MaxTurn) { pitchDiff = MaxTurn; }

         if (_lastPitchVec.cross (pitchVec).get_x () < 0.0) { pitch -= pitchDiff; }
         else { pitch += pitchDiff; }

         Matrix pitchMat (SideX, pitch);

         _lastPitchVec = ForwardZ;

         pitchMat.transform_vector (_lastPitchVec);

         ori = ori * pitchMat;
      }

      Vector headVec (vel);

      headVec.set_y (0.0);

      if (headVec.is_zero ()) {

         Vector vup (UpY);
         headVec = ForwardZ;

         OrigOri.transform_vector (vup);
         OrigOri.transform_vector (headVec);

         Matrix headMat (vup, UpY);

         headMat.transform_vector (headVec);
      }

      headVec.normalize_in_place ();

      Float64 head (ForwardZ.get_angle (_lastHeadVec));
      if (ForwardZ.cross (_lastHeadVec).get_y () < 0.0) { head = TwoPi64 - head; }

      Float64 headDiff (_lastHeadVec.get_angle (headVec));

      if (headDiff > MaxTurn) { headDiff = MaxTurn; }

      if (_lastHeadVec.cross (headVec).get_y () < 0.0) { head -= headDiff; }
      else { head += headDiff; }

      Matrix headMat (UpY, head);

      _lastHeadVec = ForwardZ;

      headMat.transform_vector (_lastHeadVec);

      ori = headMat * ori;

      Vector forward (_offset);
      ori.transform_vector (forward);

      pos += forward;

      if (_renderPortal) { _renderPortal->set_view (pos, ori); }
      if (_audioPortal) { _audioPortal->set_view (pos, ori, Velocity); }
   }
}


void
dmz::EntityPluginPortalFollow::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if ((AttributeHandle == _hilHandle) && Value) { _handle = ObjectHandle; }
}


// Input Observer Interface
void
dmz::EntityPluginPortalFollow::update_channel_state (
      const Handle Channel,
      const Boolean State) {

   if (State) { _active++; }
   else { _active--; }
}


void
dmz::EntityPluginPortalFollow::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _defaultHandle = defs.create_named_handle (ObjectAttributeDefaultName);

   _hilHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);

   Boolean foundOffset (False);

   Config cameraParams;

   if (local.lookup_all_config ("offset", cameraParams)) {

      ConfigIterator it;
      Config cd;

      Boolean found (cameraParams.get_first_config (it, cd));

      if (found)  {

         _offset = config_to_vector (cd, _offset);
         foundOffset = True;
      }
   }

   init_input_channels (local, InputEventChannelStateMask, &_log);

   _log.info << "Using " << (foundOffset ? "" : "default ") << "camera offset: "
      << _offset << endl;
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginPortalFollow (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginPortalFollow (Info, local);
}

};
