#include <dmzQtMapUtil.h>
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
      _mapZoomName ("dmzQtPluginMapZoomPan"),
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

      if (_canvasWidget && _mapWidget) {

         _canvasWidget->setParent (_mapWidget);
         
         QWidget *widget = _mapWidget->findChild<QWidget *>(_mapZoomName.get_buffer ());
         
         if (widget) {
            
            _log.debug << "Found Map Zoom Widget: " << _mapZoomName << endl;
            
            widget->setParent (_canvasWidget);
         }
      }
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
            
            // QGraphicsView *view = _canvasModule->get_view ();
            // 
            // if (view) {
            // 
            //    view->setAttribute (Qt::WA_TransparentForMouseEvents);
            // }
            
      
            QtWidget *w = QtWidget::cast (PluginPtr);
            
            if (w && !_canvasWidget) {
            
               _canvasWidget = w->get_qt_widget ();
            
               if (_canvasWidget) {
               
                  _log.debug << "Found Canvas: " << _canvasModuleName << endl;
               
                  // if (_mapWidget) {
                  //    
                  //    _canvasWidget->setParent (_mapWidget);
                  // }
                  // else {
                  //    
                  //    _log.warn << "Map not discovered yet: " << _mapModuleName << endl;
                  // }
                  
                  // _canvasWidget->setAttribute (Qt::WA_TransparentForMouseEvents);
                  // _canvasWidget->setAutoFillBackground (False);
//                  _canvasWidget->hide ();
                  
//                  _canvasWidget->installEventFilter (this);
                  
//                  _layout->addWidget (_canvasWidget);
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
               
                  _log.debug << "Found Map: " << _mapModuleName << endl;
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


#if 0
bool
dmz::QtPluginCanvasMap::eventFilter (QObject *o, QEvent *e) {

   bool retVal (False);
   
   if (0 && (o == _canvasWidget) && _mapWidget) {
      
     QApplication::sendEvent (_mapWidget, e);
     retVal = True;

//      QMouseEvent event(QEvent::MouseButtonPress, pos, 0, 0, 0);

#if 0
      switch (e->type ()) {
         
qDebug () <<  e;

         // case QEvent::Resize: {
         //     
         //    const QResizeEvent *re = (QResizeEvent *)e;
         // }
         
         case QEvent::MouseButtonPress:
         
            QApplication::sendEvent (_mapWidget, e);
//            _mapWidget->mousePressEvent ((QMouseEvent *)e);
            break;
              
         case QEvent::MouseButtonRelease:
            _mapWidget->mouseReleaseEvent ((QMouseEvent *)e);
            break;
              
         case QEvent::MouseButtonDblClick:
            _mapWidget->mouseDoubleClickEvent ((QMouseEvent *)e);
            break;
              
         case QEvent::MouseMove:
            _mapWidget->mouseMoveEvent ((QMouseEvent *)e);
            break;
              
         case QEvent::KeyPress:
            _mapWidget->keyPressEvent ((QKeyEvent *)e);
            break;
         
         case QEvent::KeyRelease:
            _mapWidget->keyReleaseEvent ((QKeyEvent *)e);
            break;
         
         case QEvent::Wheel:
            _mapWidget->wheelEvent ((QWheelEvent *)e);
            break;
         
         default:
            retVal = QWidget::eventFilter (o, e);
            break;
      }
#endif

//      _mapWidget->eventFilter (o, e);
//      retVal = True;
   }
   else {
      
      retVal = QWidget::eventFilter (o, e);
   }
   
   return retVal;

   // if (_mapModule) {
   // 
   //    qmapcontrol::MapControl *map (_mapModule->get_map_control ());
   //    
   //    if (map && (obj == map)) {
   // 
   //       if (event->type() == QEvent::Resize) {
   // 
   //          QRect viewRect (map->geometry ());
   //          QRect myRect (geometry ());
   // 
   //          myRect.moveTopRight (viewRect.topRight ());
   //          myRect.moveTop (myRect.top () + 5);
   //          myRect.moveRight (myRect.right () - 5);
   //          setGeometry (myRect);
   //       }
   //    }
   // }

   // pass the event on to the parent class
//   return QWidget::eventFilter (obj, event);
}
#endif


// void
// dmz::QtPluginCanvasMap::mousePressEvent (QMouseEvent *event) {
// 
// _log.warn << "mousePressEvent: " << endl;
// //   _handle_mouse_event (event, 0);
// event->ignore ();
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
   _mapZoomName = config_to_string ("zoom.name", local, _mapZoomName);

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
