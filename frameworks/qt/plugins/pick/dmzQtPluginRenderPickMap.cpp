#include <dmzObjectModule.h>
#include <dmzQtModuleMap.h>
#include "dmzQtPluginRenderPickMap.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesVector.h>
#include <qmapcontrol.h>
#include <QtGui/QtGui>


dmz::QtPluginRenderPickMap::QtPluginRenderPickMap (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      RenderPickUtil (Info, local),
      _log (Info),
      _mapModule (0),
      _mapModuleName (),
      _objectModule (0),
      _objectModuleName () {

   // Initialize array
   _vectorOrder[0] = VectorComponentX;
   _vectorOrder[1] = VectorComponentY;
   _vectorOrder[2] = VectorComponentZ;

   _init (local);
}


dmz::QtPluginRenderPickMap::~QtPluginRenderPickMap () {

}


// Plugin Interface
void
dmz::QtPluginRenderPickMap::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_mapModule) {

         _mapModule = QtModuleMap::cast (PluginPtr, _mapModuleName);
      }

      if (!_objectModule) {

         _objectModule = ObjectModule::cast (PluginPtr, _objectModuleName);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_mapModule && (_mapModule == QtModuleMap::cast (PluginPtr))) {

         _mapModule = 0;
      }

      if (_objectModule && (_objectModule == ObjectModule::cast (PluginPtr))) {

         _objectModule = 0;
      }
   }
}


// RenderPick Interface
dmz::Boolean
dmz::QtPluginRenderPickMap::screen_to_world (
      const Int32 ScreenPosX,
      const Int32 ScreenPosY,
      Vector &worldPosition,
      Vector &normal,
      Handle &objectHandle) {

   Boolean retVal (False);

   if (_mapModule) {

      qmapcontrol::MapControl *map (_mapModule->get_map_control ());

      if (map) {

         QPoint sourcePoint (map->mapFromGlobal (QPoint (ScreenPosX, ScreenPosY)));
          
         retVal = source_to_world (
            sourcePoint.x (),
            sourcePoint.y (),
            worldPosition,
            normal,
            objectHandle);
      }
   }

   return retVal;
}


dmz::Boolean
dmz::QtPluginRenderPickMap::world_to_screen (
      const Vector &WorldPosition,
      Int32 &screenPosX,
      Int32 &screenPosY) {

   Boolean retVal (False);

   if (_mapModule) {

      Int32 sourceX;
      Int32 sourceY;

      if (world_to_source (WorldPosition, sourceX, sourceY)) {

         qmapcontrol::MapControl *map (_mapModule->get_map_control ());

         if (map) {

            QPoint screenPoint (map->mapToGlobal (QPoint (sourceX, sourceY)));
            
            screenPosX = screenPoint.x ();
            screenPosY = screenPoint.y ();

            retVal = True;
         }
      }
   }

   return retVal;
}


dmz::Boolean
dmz::QtPluginRenderPickMap::source_to_world (
      const Int32 SourcePosX,
      const Int32 SourcePosY,
      Vector &worldPosition,
      Vector &normal,
      Handle &objectHandle) {

   Boolean retVal (False);

   if (_mapModule) {

      QPoint sourcePoint (SourcePosX, SourcePosY);
      QPointF worldPoint (_mapModule->screen_to_world (sourcePoint));
      
      worldPosition.set (_vectorOrder[0], worldPoint.x ());
      worldPosition.set (_vectorOrder[1], worldPoint.y ());
      worldPosition.set (_vectorOrder[2], 0.0);
      normal.set_xyz (0.0, 0.0, 0.0);
      normal.set (_vectorOrder[2], 1.0);

      objectHandle = _get_object_handle (sourcePoint);

      retVal = True;
   }

   return retVal;
}


dmz::Boolean
dmz::QtPluginRenderPickMap::world_to_source (
      const Vector &WorldPosition,
      Int32 &sourcePosX,
      Int32 &sourcePosY) {

   Boolean retVal (False);

   if (_mapModule) {

      QPointF worldPoint (
         WorldPosition.get (_vectorOrder[0]),
         WorldPosition.get (_vectorOrder[1]));
         
      QPoint sourcePoint (_mapModule->world_to_screen (worldPoint));
      
      sourcePosX = sourcePoint.x ();
      sourcePosY = sourcePoint.y ();

      retVal = True;
   }

   return retVal;
}


dmz::Handle
dmz::QtPluginRenderPickMap::_get_object_handle (const QPoint &ScreenPos) {

   Handle objectHandle (0);

   if (_mapModule) {

      qmapcontrol::MapControl *map (_mapModule->get_map_control ());

      if (map) {

         // QList<QGraphicsItem *>itemList (view->items (ScreenPos));
         // 
         // if (itemList.count ()) {
         // 
         //    foreach (QGraphicsItem *item, itemList) {
         // 
         //       if (item->isVisible ()) {
         // 
         //          bool ok (false);
         // 
         //          qlonglong data (
         //             item->data (QtCanvasObjectHandleIndex).toULongLong (&ok));
         // 
         //          if (ok && !objectHandle && data) {
         // 
         //             objectHandle = data;
         //          }
         // 
         //          if (objectHandle) { break; }
         //       }
         //    }
         // }
      }
   }

   return objectHandle;
}


void
dmz::QtPluginRenderPickMap::_init (Config &local) {

   _mapModuleName = config_to_string ("module.canvas.name", local);
   _objectModuleName = config_to_string ("module.object.name", local);

   _vectorOrder[0] = config_to_vector_component ("order.x", local, _vectorOrder [0]);
   _vectorOrder[1] = config_to_vector_component ("order.y", local, _vectorOrder [1]);
   _vectorOrder[2] = config_to_vector_component ("order.z", local, _vectorOrder [2]);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginRenderPickMap (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginRenderPickMap (Info, local);
}

};
