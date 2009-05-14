#include <dmzQtMapUtil.h>
#include <dmzQtModuleCanvas.h>
#include <dmzQtModuleMap.h>
#include "dmzQtPluginCanvasMap.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <QtGui/QtGui>

#include <QtCore/QDebug>


static const dmz::Int32 BaseZoom (15);

   
dmz::Float64
local_level_to_scale (const dmz::Int32 Value) {

   return (1.0 / pow (2, BaseZoom - Value));
}


dmz::Int32
local_scale_to_level (const dmz::Float64 Value) {

   return (BaseZoom - log2 (1.0 / Value));
}


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

      // if (!_canvasModule) {
      // 
      //    _canvasModule = QtModuleCanvas::cast (PluginPtr, _canvasModuleName);
      // 
      //    if (_canvasModule) {
      // 
      //       QtWidget *w = QtWidget::cast (PluginPtr);
      //       
      //       if (w) {
      //       
      //          _canvasWidget = w->get_qt_widget ();
      //       
      //          if (_canvasWidget) {
      //          
      //             _canvasWidget->setParent (this);
      //             _layout->addWidget (_canvasWidget);
      //          }
      //       }
      //    }
      // }

      if (!_mapModule) {

         _mapModule = QtModuleMap::cast (PluginPtr, _mapModuleName);

         if (_mapModule) {

            QtWidget *w = QtWidget::cast (PluginPtr);
            
            if (w) {
            
               _mapWidget = w->get_qt_widget ();
            
               if (_mapWidget) {
               
                  _mapWidget->setParent (this);
                  _layout->addWidget (_mapWidget);
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
      
      if (_mapModule && (_mapModule == QtModuleMap::cast (PluginPtr))) {

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

      if (_mapWidget) { _mapWidget->resize (event->size ()); }
      
//      if (_canvasWidget) { _canvasWidget->resize (event->size ()); }

      event->ignore ();
   }
}


void
dmz::QtPluginCanvasMap::_init (Config &local) {

   _canvasModuleName = config_to_string ("module.canvas.name", local);
   _mapModuleName = config_to_string ("module.map.name", local);

   _layout = new QVBoxLayout (this);
   _layout->setSpacing (0);
   _layout->setContentsMargins (0, 0, 0, 0);
//   _layout->addWidget ();
   
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
