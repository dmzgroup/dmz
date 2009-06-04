#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include "dmzQtPluginMapObjectPosition.h"
#include <dmzRenderModulePick.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesVector.h>


dmz::QtPluginMapObjectPosition::QtPluginMapObjectPosition (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _pickModule (0),
      _pickModuleName (),
      _defaultAttrHandle (0),
      _positionAttrHandle (0),
      _sourceCanvas (0),
      _sourceMap (0),
      _typeSet () {

   _init (local);
}


dmz::QtPluginMapObjectPosition::~QtPluginMapObjectPosition () {

}


// Plugin Interface
void
dmz::QtPluginMapObjectPosition::update_plugin_state (
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
dmz::QtPluginMapObjectPosition::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_pickModule) {

         _pickModule = RenderModulePick::cast (PluginPtr, _pickModuleName);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_pickModule && (_pickModule == RenderModulePick::cast (PluginPtr))) {

         _pickModule = 0;
      }
   }
}


// Object Observer Interface

void
dmz::QtPluginMapObjectPosition::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (AttributeHandle == _defaultAttrHandle) {
      
      ObjectModule *objMod (get_object_module ());
      
      if (objMod) {
         
         ObjectType type (objMod->lookup_object_type (ObjectHandle));
         
         if (_typeSet.contains_type (type)) {
            
_log.error << ObjectHandle << " - " << type.get_name () << endl;

            Vector mapPos (Value);
            Vector canvasPos (0.0, 0.0, 0.0);
            
            if (_map_to_canvas (mapPos, canvasPos)) {
               
               _log.warn << "   map: " << mapPos << endl;
               _log.warn << "canvas: " << canvasPos << endl;
               
               objMod->store_position (ObjectHandle, _positionAttrHandle, canvasPos);
            }
            
            // Int32 screenX, screenY;
            // 
            // Vector mPos (Value.get_x (), Value.get_z (), 0.0);
            // 
            // _pickModule->world_to_screen (_sourceMap, mPos, screenX, screenY);
            // 
            // _log.warn << "map: " << mPos << endl;
            // _log.warn << "map->screen: " << screenX << " - " << screenY << endl;
            // 
            // Vector cPos (0.0, 0.0, 0.0);
            // _pickModule->world_to_screen (_sourceCanvas, cPos, screenX, screenY);
            // _log.warn << "canvas: " << cPos << endl;
            // _log.warn << "canvas->screen: " << screenX << " - " << screenY << endl;
            // 
            // Vector cNormal;
            // Handle objHandle;

            //            _pickModule->screen_to_world (_sourceCanvas, screenX, screenY, pos, normal, objHandle);
            //_log.warn << pos << endl;
            //            _menu->popup (QPoint (screenX, screenY));
         }
      }
   }
}

dmz::Boolean
dmz::QtPluginMapObjectPosition::_map_to_canvas (const Vector &MapPos, Vector &canvasPos) {

   Boolean retVal (False);
   
   if (_pickModule && _sourceCanvas && _sourceMap) {
      
      Int32 screenX, screenY;

      Vector mapPos (MapPos.get_x (), MapPos.get_z (), 0.0);

      _pickModule->world_to_screen (_sourceMap, mapPos, screenX, screenY);

      Vector normal;
      Handle objHandle;

     _pickModule->screen_to_world (_sourceCanvas, screenX, screenY, canvasPos, normal, objHandle);
     
     retVal = True;
   }
   
   return retVal;
}


void
dmz::QtPluginMapObjectPosition::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());
   Definitions defs (context);
   
   _pickModuleName = config_to_string ("module.pick.name", local);

   _defaultAttrHandle = activate_default_object_attribute (ObjectPositionMask);

   _positionAttrHandle = activate_object_attribute (
      config_to_string ("attribute.position.name", local),
      ObjectPositionMask);

   _sourceCanvas = config_to_named_handle (
      "source.canvas.name", local, "dmzQtModuleCanvasBasic", context);

   _sourceMap = config_to_named_handle (
      "source.map.name", local, "dmzQtModuleMapBasic", context);
      
   _typeSet = config_to_object_type_set ("set", local, context);

   if (_typeSet.get_count () == 0) {

      _log.info << "No object types specified. Using root type." << endl;
      _typeSet.add_object_type (defs.get_root_object_type ());
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginMapObjectPosition (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginMapObjectPosition (Info, local);
}

};
