#include <dmzQtModuleCanvas.h>
#include "dmzQtPluginCanvasMap.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <qmapcontrol.h>
#include <QtGui/QtGui>


dmz::QtPluginCanvasMap::QtPluginCanvasMap (const PluginInfo &Info, Config &local) :
      QWidget (0),
      Plugin (Info),
      QtWidget (Info),
      _log (Info),
      _layout (0),
      _canvasModule (0),
      _canvasModuleName (),
      _map (0),
      _mapAdapter (0),
      _baseLayer (0),
      _canvasWidget (0) {

   _init (local);
}


dmz::QtPluginCanvasMap::~QtPluginCanvasMap () {

}


// Plugin Interface
void
dmz::QtPluginCanvasMap::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

      // if (_canvasWidget) {
      //    
      //    _canvasWidget->setParent (this);
      // }
   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::QtPluginCanvasMap::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_canvasModule) {

         _canvasModule = QtModuleCanvas::cast (PluginPtr, _canvasModuleName);

         if (_canvasModule) {

            // QGraphicsView *view (_canvasModule->get_view ());
            // 
            // if (view) {
            //  
            //    _canvasWidget = view;
            // }
            
            QtWidget *w = QtWidget::cast (PluginPtr);
            
            if (w) {
            
               _canvasWidget = w->get_qt_widget ();
            
               if (_canvasWidget) {
               
                  _canvasWidget->setParent (this);
               }
            }
         }
      }
      
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_canvasModule && (_canvasModule == QtModuleCanvas::cast (PluginPtr))) {

         _canvasWidget = 0;
         _canvasModule = 0;
      }
   }
}


// QtWidget Interface
QWidget *
dmz::QtPluginCanvasMap::get_qt_widget () { return this; }


void
dmz::QtPluginCanvasMap::resizeEvent (QResizeEvent *event) {

   if (event) {
      
      // if (_map) { _map->resize (event->size () - QSize (1, 1)); }
      // if (_canvasWidget) { _canvasWidget->resize (event->size ()); }
      // 
      // if (_canvasModule) {
      // 
      //    QGraphicsView *view (_canvasModule->get_view ());
      //    if (view) { view->resize (event->size ()); } 
      // }
      
      event->ignore ();
   }
}


void
dmz::QtPluginCanvasMap::_init (Config &local) {

   _canvasModuleName = config_to_string ("module.canvas.name", local);

   Int32 zoomMin = config_to_int32 ("zoom.min", local, 0);
   Int32 zoomMax = config_to_int32 ("zoom.max", local, 17);
   
   String mapUrl (config_to_string ("tileMapAdapter.url", local, "tile.openstreetmap.org"));
   String mapPath (config_to_string ("tileMapAdapter.path", local, "/%1/%2/%3.png"));
   Int32 tileSize (config_to_int32 ("tileMapAdapter.tileSize", local, 256));
   
   _map = new qmapcontrol::MapControl (frameSize (), qmapcontrol::MapControl::None, this);
   
   _map->showScale (config_to_boolean ("map.scale", local, True));
   
   _mapAdapter = new qmapcontrol::TileMapAdapter (
      mapUrl.get_buffer (),
      mapPath.get_buffer (),
      tileSize,
      zoomMin,
      zoomMax);

   _baseLayer = new qmapcontrol::MapLayer ("base", _mapAdapter);
   _map->addLayer (_baseLayer);

   _layout = new QVBoxLayout (this);
   _layout->setSpacing (0);
   _layout->setContentsMargins (0, 0, 0, 0);
   _layout->addWidget (_map);
   
   setLayout (_layout);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginCanvasMap (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginCanvasMap (Info, local);
}

};
