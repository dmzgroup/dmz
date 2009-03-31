#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzRenderModuleCoreOSG.h>
#include "dmzRenderPluginHideObjectOSG.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <osg/Group>

dmz::RenderPluginHideObjectOSG::RenderPluginHideObjectOSG (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _core (0),
      _cullMask (0) {

   _init (local);
}


dmz::RenderPluginHideObjectOSG::~RenderPluginHideObjectOSG () {

}


// Plugin Interface
void
dmz::RenderPluginHideObjectOSG::update_plugin_state (
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
dmz::RenderPluginHideObjectOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) {

         _core = RenderModuleCoreOSG::cast (PluginPtr);

         if (_core) {

            _cullMask = _core->get_cull_mask ();
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOSG::cast (PluginPtr))) {

         _core = 0;
         _cullMask = 0;
      }
   }
}


// Object Observer Interface
void
dmz::RenderPluginHideObjectOSG::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (_core) {

      osg::Group *group = _core->lookup_dynamic_object (ObjectHandle);

      if (group) {

         UInt32 mask = group->getNodeMask ();

         if (Value) { mask &= (~_cullMask); }
         else { mask |= _cullMask; }

         group->setNodeMask (mask);
      }
   }
}


void
dmz::RenderPluginHideObjectOSG::_init (Config &local) {

   activate_object_attribute (
      config_to_string ("flag.name", local, ObjectAttributeHideName),
      ObjectFlagMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginHideObjectOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginHideObjectOSG (Info, local);
}

};
