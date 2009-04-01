#include <dmzRenderModuleCoreOSG.h>
#include "dmzRenderPluginStaticTerrainOSG.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <osg/Group>
#include <osgDB/ReadFile>

dmz::RenderPluginStaticTerrainOSG::RenderPluginStaticTerrainOSG (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      _log (Info),
      _rc (Info, &_log),
      _core (0),
      _modelList (0) {

   _init (local);
}


dmz::RenderPluginStaticTerrainOSG::~RenderPluginStaticTerrainOSG () {

   if (_modelList) { delete _modelList; _modelList = 0; }
}


// Plugin Interface
void
dmz::RenderPluginStaticTerrainOSG::update_plugin_state (
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
dmz::RenderPluginStaticTerrainOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) {

         _core = RenderModuleCoreOSG::cast (PluginPtr);

         if (_core) {

            const UInt32 IsectMask = _core->get_isect_mask ();

            osg::Group *root = _core->get_static_objects ();

            if (root) {

               ModelStruct *current (_modelList);

               while (current) {

                  if (current->model.valid ()) {

                     if (!current->Isect) {

                        UInt32 mask = current->model->getNodeMask ();
                        mask &= (~IsectMask);
                        current->model->setNodeMask (mask);
                     }

                     root->addChild (current->model.get ());
                  }

                  current = current->next;
               }
            }
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOSG::cast (PluginPtr))) {

         const UInt32 IsectMask = _core->get_isect_mask ();

         osg::Group *root = _core->get_static_objects ();

         if (root) {

            ModelStruct *current (_modelList);

            while (current) {

               if (current->model.valid ()) {

                  root->removeChild (current->model.get ());

                  if (!current->Isect) {

                     UInt32 mask = current->model->getNodeMask ();
                     mask |= IsectMask;
                     current->model->setNodeMask (mask);
                  }
               }

               current = current->next;
            }
         }

         _core = 0;
      }
   }
}


void
dmz::RenderPluginStaticTerrainOSG::_init (Config &local) {

   Config list;

   if (local.lookup_all_config ("model", list)) {

      ConfigIterator it;
      Config model;

      while (list.get_next_config (it, model)) {

         const String ResourceName = config_to_string ("resource", model);

         if (ResourceName) {

            const String FileName = _rc.find_file (ResourceName);
            const Boolean Isect = config_to_boolean ("isect", model, True);

            if (FileName) {

               ModelStruct *ms = new ModelStruct (Isect);

               if (ms) {

                  ms->model = osgDB::readNodeFile (FileName.get_buffer ());

                  if (ms->model.valid ()) { 

                     ms->next = _modelList;
                     _modelList = ms;

                     _log.info << "Loaded model: " << FileName
                       << " (" << ResourceName << ")" << endl;
                  }
                  else {

                     delete ms; ms = 0;
                     _log.error << "Failed loading model: " << FileName
                        << " (" << ResourceName << ")" << endl;
                  }
               }
            }
         }
         else {

            _log.error << "No resource name specified for static terrain." << endl;
         }
      }
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginStaticTerrainOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginStaticTerrainOSG (Info, local);
}

};
