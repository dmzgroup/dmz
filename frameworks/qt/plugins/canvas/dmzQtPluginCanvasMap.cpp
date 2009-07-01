#include <dmzQtModuleCanvas.h>
#include <dmzQtModuleMap.h>
#include "dmzQtPluginCanvasMap.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <QtGui/QtGui>
#include <QtCore/QtCore>

#include <QtCore/QDebug>


dmz::QtPluginCanvasMap::QtPluginCanvasMap (const PluginInfo &Info, Config &local) :
      QWidget (0),
      Plugin (Info),
      TimeSlice (Info),
      QtWidget (Info),
      _log (Info),
      _layout (0),
      _mapModule (0),
      _mapModuleName (),
      _canvasModule (0),
      _canvasModuleName (),
      _canvasWidget (0),
      _mapWidget (0) {

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

      // if (_canvasWidget && _mapWidget) {
      // 
      //   _canvasWidget->setParent (_mapWidget);
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
            
            QtWidget *w = QtWidget::cast (PluginPtr);
            
            if (w && !_canvasWidget) {
            
               _canvasWidget = w->get_qt_widget ();
            
               if (_canvasWidget) {
               
                  _log.info << "Found Canvas: " << _canvasModuleName << endl;
                  _canvasWidget->setParent (this);
               }
            }
         }
      }

      if (!_mapModule) {

         _mapModule = QtModuleMap::cast (PluginPtr, _mapModuleName);

         if (_mapModule) {

            QtWidget *w = QtWidget::cast (PluginPtr);
            
            if (w) {
            
               _mapWidget = w->get_qt_widget ();
            
               if (_mapWidget) {
               
                  _log.info << "Found Map: " << _mapModuleName << endl;
                  _mapWidget->setParent (this);
                  _layout->addWidget (_mapWidget);
               }
            }
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_canvasModule && (_canvasModule == QtModuleCanvas::cast (PluginPtr))) {

         _canvasWidget->setParent (0);
         _canvasWidget = 0;
         _canvasModule = 0;
      }
      
      if (_mapModule && (_mapModule == QtModuleMap::cast (PluginPtr))) {

         _mapWidget->setParent (0);
         _mapWidget = 0;
         _mapModule = 0;
      }
   }
}


// TimeSlice Interface
void
dmz::QtPluginCanvasMap::update_time_slice (const Float64 TimeDelta) {

   if (_canvasModule) {
      
   }

   if (_mapModule) {
      
   }
}


// QtWidget Interface
QWidget *
dmz::QtPluginCanvasMap::get_qt_widget () { return this; }


// void
// dmz::QtPluginCanvasMap::_slot_pan_changed (const QPoint &Value) {
// 
//    if (_map) {
// 
// //      _map->scroll (-Value);
//    }
// }
// 
// 
// void
// dmz::QtPluginCanvasMap::_slot_scale_changed (qreal value) {
// 
//    if (_map) {
//       
//       _map->setZoom (local_scale_to_level (value));
//    }
// }


void
dmz::QtPluginCanvasMap::resizeEvent (QResizeEvent *event) {

   if (event) {

//      if (_mapWidget) { _mapWidget->resize (event->size ()); }

      if (_canvasWidget) { _canvasWidget->resize (event->size ()); }

      event->ignore ();
   }
}


void
dmz::QtPluginCanvasMap::_init (Config &local) {

   _canvasModuleName = config_to_string ("canvas.name", local);
   _mapModuleName = config_to_string ("map.name", local);

   _layout = new QVBoxLayout (this);
   _layout->setSpacing (0);
   _layout->setContentsMargins (0, 0, 0, 0);
   
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
