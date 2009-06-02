#include "dmzEntityPluginOverlayDead.h"
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzRenderModuleOverlay.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::EntityPluginOverlayDead
\ingroup Entity
\brief Toggles dead state overlay.
\details
\code
<dmz>
<dmzEntityPluginOverlayDead>
   <overlay>
      <switch name="Name of overlay switch"/>
      <scale name="Name of overlay transform"/>
   </overlay>
</dmzEntityPluginOverlayDead>
</dmz>
\endcode

*/

//! \cond
dmz::EntityPluginOverlayDead::EntityPluginOverlayDead (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      PortalSizeObserver (Info, local),
      _log (Info),
      _overlay (0),
      _hil (0),
      _hilAttrHandle (0),
      _overlaySwitchName ("dead overlay switch"),
      _overlaySwitch (0),
      _overlayScaleName ("dead overlay transform"),
      _overlayScale (0) {

   _init (local);
}


dmz::EntityPluginOverlayDead::~EntityPluginOverlayDead () {

}


// Plugin Interface
void
dmz::EntityPluginOverlayDead::update_plugin_state (
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
dmz::EntityPluginOverlayDead::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_overlay) {

         _overlay = RenderModuleOverlay::cast (PluginPtr);

         if (_overlay) {

            _overlayScale = _overlay->lookup_node_handle (_overlayScaleName);
            _overlaySwitch = _overlay->lookup_node_handle (_overlaySwitchName);
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_overlay && (_overlay == RenderModuleOverlay::cast (PluginPtr))) {

         _overlay = 0;
         _overlayScale = 0;
         _overlaySwitch = 0;
      }
   }
}


// Object Observer Interface
void
dmz::EntityPluginOverlayDead::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   if (_overlay && (ObjectHandle == _hil)) {

      const Boolean IsDead (Value.contains (_deadState));
      const Boolean WasDead (
         PreviousValue ? PreviousValue->contains (_deadState) : False);

      if (IsDead && !WasDead) {

         if (_overlaySwitch) {

            _overlay->enable_switch_state_single (_overlaySwitch, 1);
         }

         if (_overlayScale) {

            _overlay->store_transform_scale (
               _overlayScale,
               Float64 (get_portal_x ()),
               Float64 (get_portal_y ()));
         }
      }
      else if (!IsDead && WasDead) {

         if (_overlaySwitch) {

            _overlay->enable_switch_state_single (_overlaySwitch, 0);
         }
      }
   }
}


void
dmz::EntityPluginOverlayDead::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (Value) { _hil = ObjectHandle; }
   else if (_hil == ObjectHandle) { _hil = 0; }
}


// Render Portal Observer Interface
void
dmz::EntityPluginOverlayDead::update_portal_size (
      const Handle PortalHandle,
      const Int32 ValueX,
      const Int32 ValueY) {

   if (_overlay && _overlayScale) {

      _overlay->store_transform_scale (
         _overlayScale,
         Float64 (ValueX),
         Float64 (ValueY));
   }
}


void
dmz::EntityPluginOverlayDead::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   defs.lookup_state (DefaultStateNameDead, _deadState);

   activate_default_object_attribute (ObjectStateMask);

   _hilAttrHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);

   _overlaySwitchName =
      config_to_string ("overlay.switch.name", local, _overlaySwitchName);

   _overlayScaleName = config_to_string ("overlay.scale.name", local, _overlayScaleName);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginOverlayDead (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginOverlayDead (Info, local);
}

};
