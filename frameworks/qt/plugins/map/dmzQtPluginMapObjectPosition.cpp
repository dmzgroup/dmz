#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzQtModuleCanvas.h>
#include "dmzQtPluginMapObjectPosition.h"
#include <dmzRenderModulePick.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesVector.h>
#include <QtGui/QtGui>


dmz::QtPluginMapObjectPosition::QtPluginMapObjectPosition (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _canvasModule (0),
      _canvasModuleName (),
      _pickModule (0),
      _pickModuleName (),
      _defaultAttrHandle (0),
      _positionAttrHandle (0),
      _sourceCanvas (0),
      _typeSet () {

   // Initialize array
   _vectorOrder[0] = VectorComponentX;
   _vectorOrder[1] = VectorComponentY;
   _vectorOrder[2] = VectorComponentZ;
   
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

      if (!_canvasModule) {

         _canvasModule = QtModuleCanvas::cast (PluginPtr, _canvasModuleName);
      }

      if (!_pickModule) {

         _pickModule = RenderModulePick::cast (PluginPtr, _pickModuleName);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_canvasModule && (_canvasModule == QtModuleCanvas::cast (PluginPtr))) {

         _canvasModule = 0;
      }

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
            
_log.error << ObjectHandle << " - " << type.get_name () << " - " << Value << endl;

            Vector canvasPos;
            
            if (_world_to_canvas (Value, canvasPos)) {
               
               _log.warn << "   map: " << Value << endl;
               _log.warn << "canvas: " << canvasPos << endl;
               
               objMod->store_position (ObjectHandle, _positionAttrHandle, canvasPos);
            }
         }
      }
   }
}


dmz::Boolean
dmz::QtPluginMapObjectPosition::_world_to_canvas (const Vector &WorldPos, Vector &canvasPos) {

   Boolean retVal (False);
   
   if (_canvasModule && _pickModule && _sourceCanvas) {

      QGraphicsView *view (_canvasModule->get_view ());

      if (view) {
         
         Int32 sourceX, sourceY;
         
         if (_pickModule->world_to_source (_sourceCanvas, WorldPos, sourceX, sourceY)) {
            
_log.warn << "source pos: " << sourceX << " - " << sourceY << endl;

            QPoint sourcePoint (sourceX, sourceY);

            QPointF worldPoint (view->mapToScene (sourcePoint));

            canvasPos.set (_vectorOrder[0], worldPoint.x ());
            canvasPos.set (_vectorOrder[1], worldPoint.y ());
            canvasPos.set (_vectorOrder[2], 0.0);

            retVal = True;
         }
      }
   }
   
   return retVal;
}


void
dmz::QtPluginMapObjectPosition::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());
   Definitions defs (context);
   
   _canvasModuleName = config_to_string ("module.canvas.name", local);
   _pickModuleName = config_to_string ("module.pick.name", local);

   _defaultAttrHandle = activate_default_object_attribute (ObjectPositionMask);

   _positionAttrHandle = activate_object_attribute (
      config_to_string ("attribute.position.name", local),
      ObjectPositionMask);

   _sourceCanvas = config_to_named_handle (
      "source.canvas.name", local, "dmzQtModuleCanvasBasic", context);

   _typeSet = config_to_object_type_set ("set", local, context);

   if (_typeSet.get_count () == 0) {

      _log.info << "No object types specified. Using root type." << endl;
      _typeSet.add_object_type (defs.get_root_object_type ());
   }
   
   
   _vectorOrder[0] = config_to_vector_component ("order.x", local, _vectorOrder [0]);
   _vectorOrder[1] = config_to_vector_component ("order.y", local, _vectorOrder [1]);
   _vectorOrder[2] = config_to_vector_component ("order.z", local, _vectorOrder [2]);
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
