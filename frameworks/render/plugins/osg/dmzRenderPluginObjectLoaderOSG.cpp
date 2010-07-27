#include <dmzObjectAttributeMasks.h>
#include <dmzRenderModuleCoreOSG.h>
#include "dmzRenderPluginObjectLoaderOSG.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>


dmz::RenderPluginObjectLoaderOSG::RenderPluginObjectLoaderOSG (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _core (0),
      _modelAttrHandle (0),
      _objectTable () {

   _init (local);
}


dmz::RenderPluginObjectLoaderOSG::~RenderPluginObjectLoaderOSG () {

   _objectTable.empty ();
}


// Plugin Interface
void
dmz::RenderPluginObjectLoaderOSG::update_plugin_state (
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
dmz::RenderPluginObjectLoaderOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) { _core = RenderModuleCoreOSG::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOSG::cast (PluginPtr))) {
      
         HashTableHandleIterator it;
         ObjectStruct *os (0);

         while (_objectTable.get_next (it, os)) {

            osg::Group *group (_core->lookup_dynamic_object (it.get_hash_key ()));

            if (os->model.valid () && group) { group->removeChild (os->model.get ()); }
         }
         
         _core = 0;
      }
   }
}


// Object Observer Interface
void
dmz::RenderPluginObjectLoaderOSG::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   ObjectStruct *os (_objectTable.remove (ObjectHandle));

   if (os) {

      if (_core && os->model.valid ()) {

         osg::Group *group (_core->lookup_dynamic_object (ObjectHandle));

         if (group) { group->removeChild (os->model.get ()); }
      }

      delete os; os = 0;
   }
}


void
dmz::RenderPluginObjectLoaderOSG::remove_object_attribute (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &AttrMask) {

   if (AttributeHandle == _modelAttrHandle) {

      destroy_object (Identity, ObjectHandle);
   }
}


void
dmz::RenderPluginObjectLoaderOSG::update_object_text (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const String &Value,
      const String *PreviousValue) {

   if (AttributeHandle == _modelAttrHandle) {
      
      if (_core) {
      
         ObjectStruct *os = _objectTable.lookup (ObjectHandle);
         
         if (!os) {
            
            os = new ObjectStruct;
            os->model = osgDB::readNodeFile (Value.get_buffer ());
            
            if (os->model.valid ()) {

               osgUtil::Optimizer optimizer;
               optimizer.optimize (os->model.get (),
                  osgUtil::Optimizer::DEFAULT_OPTIMIZATIONS &
                  !osgUtil::Optimizer::OPTIMIZE_TEXTURE_SETTINGS);
               
               osg::Group *group (_core->create_dynamic_object (ObjectHandle));

               if (group) { group->addChild (os->model.get ()); }
               
               _objectTable.store (ObjectHandle, os);

               _log.info << "Loaded file: " << Value << endl;
            }
            else {
               
               os->model = 0; delete os; os = 0;
               _log.error << "Failed loading file: " << Value << endl;
            }
         }
      }
   }
}


void
dmz::RenderPluginObjectLoaderOSG::_init (Config &local) {

   activate_default_object_attribute (ObjectDestroyMask);

   _modelAttrHandle = activate_object_attribute (
      config_to_string ("attribute.model.name", local, "Object_Model_Attribute"),
      ObjectTextMask |
      ObjectRemoveAttributeMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginObjectLoaderOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginObjectLoaderOSG (Info, local);
}

};
