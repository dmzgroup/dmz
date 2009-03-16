#include <dmzRenderUtilOSG.h>
#include "dmzRenderPluginLoadModelOSG.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>

#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osgUtil/Optimizer>

dmz::RenderPluginLoadModelOSG::RenderPluginLoadModelOSG (
      const PluginInfo &Info,
      const Config &Local) :
      Plugin (Info),
      _core (0),
      _log (Info) {

   _init (Local);
}


dmz::RenderPluginLoadModelOSG::~RenderPluginLoadModelOSG () {

}


void
dmz::RenderPluginLoadModelOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) {

         _core = RenderModuleCoreOSG::cast (PluginPtr);
         if (_core) {

            HashTableStringIterator it;

            String *currentFileName;

            for (currentFileName = _fileNameTable.get_first (it);
                  currentFileName;
                  currentFileName = _fileNameTable.get_next (it)) {

               osg::ref_ptr<osg::Node> model =
                  osgDB::readNodeFile (currentFileName->get_buffer ());

               if (model.valid ()) {

                  osg::BoundingSphere bs = model->computeBound ();
                  osg::Vec3d c = bs.center ();
                  _core->get_static_objects ()->addChild (model.get ());
               }
               else {

                  _log.error << "Failed loading: " << *currentFileName << endl;
               }
            }
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && _core == RenderModuleCoreOSG::cast (PluginPtr)) {

         _core = 0;
      }
   }
}


void
dmz::RenderPluginLoadModelOSG::_init (const Config &Local) {

   Config models;

   if (Local.lookup_all_config ("model", models)) {

      ConfigIterator it;
      Config cd;

      Boolean found (models.get_first_config (it, cd));

      while (found)  {

         String modelFileName (config_to_string ("file", cd));
         String modelKeyName (config_to_string ("name", cd));

         String *namePtr (new String (modelFileName));

         if (namePtr) {

            if (_fileNameTable.store (modelKeyName, namePtr)) {

               _log.info << "Loading Model: "
               << config_to_string ("file", cd) << endl;
            }
            else { delete namePtr; namePtr = 0; }
         }

         found = models.get_next_config (it, cd);
      }
   }
   else { _log.info << "No models listed" << endl; }

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginLoadModelOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginLoadModelOSG (Info, local);
}

};
