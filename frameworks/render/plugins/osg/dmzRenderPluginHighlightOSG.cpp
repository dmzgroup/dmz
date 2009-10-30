#include <dmzObjectAttributeMasks.h>
#include <dmzRenderModuleCoreOSG.h>
#include "dmzRenderPluginHighlightOSG.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <osg/Group>
#include <osg/Material>


dmz::RenderPluginHighlightOSG::RenderPluginHighlightOSG (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _core (0) {

   _init (local);
}


dmz::RenderPluginHighlightOSG::~RenderPluginHighlightOSG () {

   _yellow = 0;
}


// Plugin Interface
void
dmz::RenderPluginHighlightOSG::update_plugin_state (
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
dmz::RenderPluginHighlightOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) { _core = RenderModuleCoreOSG::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOSG::cast (PluginPtr))) { _core = 0; }
   }
}


// Object Observer Interface
void
dmz::RenderPluginHighlightOSG::remove_object_attribute (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &AttrMask) {

}


void
dmz::RenderPluginHighlightOSG::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (_core) {

      osg::Group *group = _core->lookup_dynamic_object (ObjectHandle);

      if (group) {

         if (Value) { group->setStateSet (_yellow.get ()); }
         else { group->setStateSet (0); } 
      }
   }
}


void
dmz::RenderPluginHighlightOSG::_init (Config &local) {

   activate_object_attribute ("Highlight", ObjectRemoveAttributeMask | ObjectFlagMask);

   _yellow = new osg::StateSet;
   osg::ref_ptr <osg::Material> material = new osg::Material;
   material->setColorMode (osg::Material::OFF);
   osg::Vec4 none (0.0f, 0.0f, 0.0f, 1.0f);
   material->setAmbient (osg::Material::FRONT_AND_BACK, none);
   material->setDiffuse (osg::Material::FRONT_AND_BACK, none);
   material->setSpecular (osg::Material::FRONT_AND_BACK, none);

   material->setEmission (osg::Material::FRONT_AND_BACK, osg::Vec4 (1.0,1.0f,0.0f,1.0f));

   _yellow->setAttributeAndModes (
      material.get (),
      osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

   _yellow->setMode (
      GL_LIGHTING,
      osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

   _yellow->setTextureMode (
      0,
      GL_TEXTURE_2D, 
      osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginHighlightOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginHighlightOSG (Info, local);
}

};
