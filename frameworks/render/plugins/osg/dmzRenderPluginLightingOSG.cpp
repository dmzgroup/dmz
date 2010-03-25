#include <dmzRenderConfigToOSG.h>
#include <dmzRenderModuleCoreOSG.h>
#include "dmzRenderPluginLightingOSG.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <osg/Light>
#include <osg/LightSource>

dmz::RenderPluginLightingOSG::RenderPluginLightingOSG (
      const PluginInfo &Info, 
      Config &local) :
      Plugin (Info),
      _log (Info),
      _core (0) {

   _init (local);
}

 
dmz::RenderPluginLightingOSG::~RenderPluginLightingOSG () {

   _lightTable.empty ();
}


// Plugin Interface
void
dmz::RenderPluginLightingOSG::update_plugin_state (
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
dmz::RenderPluginLightingOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) { 
         
         _core = RenderModuleCoreOSG::cast (PluginPtr); 

         if (_core) { _add_lights (); }         
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOSG::cast (PluginPtr))) {

         _remove_lights ();
         _core = 0;
      }
   }
}


void
dmz::RenderPluginLightingOSG::_add_lights () {

   osg::ref_ptr<osg::Group> scene (_core ? _core->get_scene () : 0);

   if (scene.valid ()) {

      if (_lightTable.get_count () == 0) {

         LightStruct *ls = new LightStruct;

         if (ls) {

            ls->light = new osg::Light;

            ls->light->setLightNum (0);
            ls->light->setPosition (osg::Vec4 (0.0, 1000.0, 0.0, 1.0));
            ls->light->setDirection (osg::Vec3 (0.0, -1.0, 0.0));

            ls->source = new osg::LightSource;
         
            ls->source->setLight (ls->light.get ());
            ls->source->setLocalStateSetModes (osg::StateAttribute::ON);

            if (_lightTable.store (0, ls)) {

               scene->addChild (ls->source.get ());

               _log.info << "No valid lights found in config. Created Default Light."
                  << endl;
            }
            else {

               delete ls; ls = 0;
               _log.error << "Failed to add default light source." << endl;
            }
         }
      }
      else {

         HashTableUInt32Iterator it;
         LightStruct *ls (0);

         while (_lightTable.get_next (it, ls)) {

            scene->addChild (ls->source.get ());
         }

         _log.info << "Created " << _lightTable.get_size () << " Light"
            << (_lightTable.get_size () == 1 ? "." : "s.") << endl;
      }
   }
}


void
dmz::RenderPluginLightingOSG::_remove_lights () {

   osg::ref_ptr<osg::Group> scene (_core ? _core->get_scene () : 0);

   if (scene.valid ()) {

      HashTableUInt32Iterator it;
      LightStruct *ls (0);

      while (_lightTable.get_next (it, ls)) { scene->removeChild (ls->source.get ()); }
   }
}
 

void 
dmz::RenderPluginLightingOSG::_init_light (const Int32 MaxLights, Config &light) {

   const Int32 LightValue (config_to_int32 ("value", light, -1));

   if ((LightValue >= 0) && (LightValue < MaxLights)) {

      LightStruct *ls (0);
      
      ls = _lightTable.lookup (LightValue);

      if (!ls) {

         ls = new LightStruct ();

         if (ls && _lightTable.store (LightValue, ls)) {

            ls->light = new osg::Light;
            ls->light->setLightNum (LightValue);
            ls->source = new osg::LightSource;
            ls->source->setLight (ls->light.get ());
         }
         else if (ls) { delete ls; ls = 0; }
      }
      
      if (ls && ls->light.valid ()) {

         Config attr;
         
         if (light.lookup_config ("position", attr)) {

            const Vector Value = config_to_vector (attr);

            ls->light->setPosition (osg::Vec4 (
               Value.get_x (), 
               Value.get_y (), 
               Value.get_z (), 
               1.0));
         }

         if (light.lookup_config ("direction", attr)) {

            const Vector Value = config_to_vector (attr).normalize ();

            ls->light->setDirection (osg::Vec3 (
               Value.get_x (), 
               Value.get_y (), 
               Value.get_z ()));
         }

         if (light.lookup_config ("ambient", attr)) {

            const osg::Vec4 Value = config_to_osg_vec4_color (
               attr,
               osg::Vec4 (0.0, 0.0, 0.0, 1.0));

            ls->light->setAmbient (Value);
         }

         if (light.lookup_config ("specular", attr)) {

            const osg::Vec4 Value = config_to_osg_vec4_color (
               attr,
               osg::Vec4 (0.0, 0.0, 0.0, 1.0));

            ls->light->setSpecular (Value);
         }

         if (light.lookup_config ("diffuse", attr)) {

            const osg::Vec4 Value = config_to_osg_vec4_color (
               attr,
               osg::Vec4 (0.0, 0.0, 0.0, 1.0));

            ls->light->setDiffuse (Value);
         }
      }
   }
}


void
dmz::RenderPluginLightingOSG::_init (Config &local) {

   const Int32 MaxLights = (config_to_int32 ("max-lights.value", local, 8));

   Config lights;

   if (local.lookup_all_config ("light", lights)) {

      ConfigIterator it;
      Config light;

      while (lights.get_next_config (it, light)) {

         _init_light (MaxLights, light);
      }
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginLightingOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginLightingOSG (Info, local);
}

};
