#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzRenderConfigToOSG.h>
#include <dmzRenderModuleCoreOSG.h>
#include "dmzRenderPluginHighlightOSG.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToNamedHandle.h>
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
      _core (0),
      _highlightList (0) {

   _init (local);
}


dmz::RenderPluginHighlightOSG::~RenderPluginHighlightOSG () {

   if (_highlightList) { delete _highlightList; _highlightList = 0; }
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

   ObjectModule *objMod = get_object_module ();

   if (_core && objMod) {

      osg::Group *group = _core->lookup_dynamic_object (ObjectHandle);

      if (!group) {

         HandleContainer links;

         if (objMod->lookup_attribute_object_links (ObjectHandle, links)) {

            group = _core->lookup_dynamic_object (links.get_first ());
         }
      }

      if (group) {

         HighlightStruct *current = _highlightList;
         Boolean done (False);

         while (!done && current) {

            if (objMod->lookup_flag (ObjectHandle, current->Attribute)) { done = True; }
            else { current = current->next; }
         }

         if (current) { group->setStateSet (current->color.get ()); }
         else { group->setStateSet (0); } 
      }
   }
}


void
dmz::RenderPluginHighlightOSG::_init (Config &local) {

   Config highlightList;

   if (local.lookup_all_config ("highlight", highlightList)) {

      const osg::Vec4 None (0.0f, 0.0f, 0.0f, 1.0f);

      ConfigIterator it;
      Config highlight;

      while (highlightList.get_prev_config (it, highlight)) {

         const String AttrName = config_to_string ("attribute", highlight);

         const Handle Attr = activate_object_attribute (
            AttrName,
            ObjectRemoveAttributeMask | ObjectFlagMask);

         if (Attr) {

            HighlightStruct *hs = new HighlightStruct (Attr);

            if (hs) {

               hs->color = new osg::StateSet;
               osg::ref_ptr <osg::Material> material = new osg::Material;
               material->setColorMode (osg::Material::OFF);
               osg::Vec4 color (None);
               color = config_to_osg_vec4_color ("ambient", highlight, None);
               material->setAmbient (osg::Material::FRONT_AND_BACK, color);
               color = config_to_osg_vec4_color ("diffuse", highlight, None);
               material->setDiffuse (osg::Material::FRONT_AND_BACK, color);
               color = config_to_osg_vec4_color ("specular", highlight, None);
               material->setSpecular (osg::Material::FRONT_AND_BACK, color);
               color = config_to_osg_vec4_color ("emission", highlight, None);
               material->setEmission (osg::Material::FRONT_AND_BACK, color);

               hs->color->setAttributeAndModes (
                  material.get (),
                  osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

               hs->color->setMode (
                  GL_LIGHTING,
                  osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

               hs->color->setTextureMode (
                  0,
                  GL_TEXTURE_2D, 
                  osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
            }

            hs->next = _highlightList;
            _highlightList = hs;
         }
      }
   }
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
