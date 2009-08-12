#include <dmzInputEventKey.h>
#include <dmzInputEventMouse.h>
#include <dmzInputEventMasks.h>
#include <dmzQtConfigRead.h>
#include <dmzQtModuleCanvas.h>
#include <dmzQtModuleMap.h>
#include "dmzQtPluginCanvasZoomPanMap.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <qmapcontrol.h>
#include <QtGui/QtGui>


dmz::QtPluginCanvasZoomPanMap::QtPluginCanvasZoomPanMap (
      const PluginInfo &Info,
      Config &local) :
      QWidget (0),
      Plugin (Info),
      InputObserverUtil (Info, local),
      _log (Info),
      _rti (Info, &_log),
      _canvasModule (0),
      _canvasModuleName (),
      _mapModule (0),
      _mapModuleName (),
      _source (0),
      _active (0),
      _mouseButton (2),
      _ignoreScaleChange (False),
      _handScrolling (False),
      _scrollDelta (10),
      _zoomMin (0),
      _zoomMax (17),
      _zoomDefault (_zoomMin) {

   setObjectName (get_plugin_name ().get_buffer ());

   _ui.setupUi (this);

   _init (local);
}


dmz::QtPluginCanvasZoomPanMap::~QtPluginCanvasZoomPanMap () {

}


// Plugin Interface
void
dmz::QtPluginCanvasZoomPanMap::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
}


void
dmz::QtPluginCanvasZoomPanMap::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_canvasModule) {

         _canvasModule = QtModuleCanvas::cast (PluginPtr, _canvasModuleName);

         if (_canvasModule) {

            _source = (PluginPtr ? PluginPtr->get_plugin_handle () : 0);

            QGraphicsView *view = _canvasModule->get_view ();

            if (view) {

               view->installEventFilter (this);
               setParent (view);
            }

            _canvasModule->set_zoom_min_value (1.0);
            _canvasModule->set_zoom_max_value (1.0);
            _canvasModule->set_zoom (1.0);
         }
      }

      if (!_mapModule) {

         _mapModule = QtModuleMap::cast (PluginPtr, _mapModuleName);

         if (0 && _mapModule) {

            qmapcontrol::MapControl *map (_mapModule->get_map_control ());
            
            if (map) {
               
               connect (
                  map, SIGNAL (zoomChanged (int)),
                  this, SLOT (slot_zoom_changed (int)));
            }
            
            _mapModule->set_zoom_min_value (_zoomMin);
            _mapModule->set_zoom_max_value (_zoomMax);
            _mapModule->set_zoom (_zoomDefault);
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_canvasModule && (_canvasModule == QtModuleCanvas::cast (PluginPtr))) {

         QGraphicsView *view = _canvasModule->get_view ();

         if (view) {

            view->removeEventFilter (this);
            hide ();
            setParent (0);
         }

         _canvasModule = 0;
      }
      
      if (_mapModule && (_mapModule == QtModuleMap::cast (PluginPtr))) {

         qmapcontrol::MapControl *map (_mapModule->get_map_control ());
            
         if (map) {
         
            map->disconnect (this);
         }
         
         _mapModule = 0;
      }
   }
}


// Input Observer Interface
void
dmz::QtPluginCanvasZoomPanMap::update_channel_state (
      const UInt32 Channel,
      const Boolean State) {

   _active += (State ? 1 : -1);
}


void
dmz::QtPluginCanvasZoomPanMap::receive_key_event (
      const UInt32 Channel,
      const InputEventKey &Value) {

//   _log.out << "receive_key_event" << endl;
}


void
dmz::QtPluginCanvasZoomPanMap::receive_mouse_event (
      const UInt32 Channel,
      const InputEventMouse &Value) {

   if (_active && _canvasModule && (Value.get_source_handle () == _source)) {

      QGraphicsView *view (_canvasModule->get_view ());

      if (Value.is_button_changed (_mouseButton)) {

         if (Value.is_button_pressed (_mouseButton)) {

            _handScrolling = True;

            if (view) { view->viewport ()->setCursor (Qt::ClosedHandCursor); }
         }
         else {

            _handScrolling = False;

            if (view) { view->viewport ()->setCursor (Qt::ArrowCursor); }
         }
      }
      else if (_handScrolling) {

         _canvasModule->pan_direction (
            Value.get_mouse_delta_x (), Value.get_mouse_delta_y ());
            
         if (_mapModule) {
            
            _mapModule->pan_direction (
               -Value.get_mouse_delta_x (), -Value.get_mouse_delta_y ());
         }
      }
      else if (Value.get_scroll_delta_y ()) {

         if (Value.get_scroll_delta_y () >= 1) {

            _ui.zoomInButton->click ();
         }
         else {

            _ui.zoomOutButton->click ();
         }
      }
   }
}


void
dmz::QtPluginCanvasZoomPanMap::on_panLeftButton_clicked () {

   if (_canvasModule) {

      _canvasModule->pan_direction (_scrollDelta, 0);
   }
   
   if (_mapModule) {

      _mapModule->pan_direction (-(Int32)_scrollDelta, 0);
   }
}


void
dmz::QtPluginCanvasZoomPanMap::on_panRightButton_clicked () {

   if (_canvasModule) {

      _canvasModule->pan_direction (-(Int32)_scrollDelta, 0);
   }
   
   if (_mapModule) {

      _mapModule->pan_direction (_scrollDelta, 0);
   }
}


void
dmz::QtPluginCanvasZoomPanMap::on_panUpButton_clicked () {

   if (_canvasModule) {

      _canvasModule->pan_direction (0, _scrollDelta);
   }
   
   if (_mapModule) {

      _mapModule->pan_direction (0, -(Int32)_scrollDelta);
   }
}


void
dmz::QtPluginCanvasZoomPanMap::on_panDownButton_clicked () {

   if (_canvasModule) {

      _canvasModule->pan_direction (0, -(Int32)_scrollDelta);
   }
   
   if (_mapModule) {
   
      _mapModule->pan_direction (0, _scrollDelta);
   }
}


void
dmz::QtPluginCanvasZoomPanMap::on_zoomAllButton_clicked () {

   if (_mapModule) {

      qmapcontrol::MapControl *map (_mapModule->get_map_control ());
      
      if (map) {
         
         map->setView (QPointF (0.0, 0.0));
      }
      
      _mapModule->set_zoom (_mapModule->get_zoom_min_value ());
   }
}


void
dmz::QtPluginCanvasZoomPanMap::on_zoomInButton_clicked () {

   _ui.zoomSlider->triggerAction (QAbstractSlider::SliderSingleStepAdd);
}


void
dmz::QtPluginCanvasZoomPanMap::on_zoomOutButton_clicked () {

   _ui.zoomSlider->triggerAction (QAbstractSlider::SliderSingleStepSub);
}


void
dmz::QtPluginCanvasZoomPanMap::on_zoomSlider_valueChanged (int value) {

   if (_canvasModule) {
      
      _canvasModule->center_on (QPointF (0.0, 0.0));
   }
   
   if (_mapModule) {

      const Float32 ZoomMin (_mapModule->get_zoom_min_value ());
      const Float32 ZoomMax (_mapModule->get_zoom_max_value ());
      const Float32 ZoomRange (ZoomMax - ZoomMin);
      const Float32 SliderRange (_ui.zoomSlider->maximum () - _ui.zoomSlider->minimum ());
      const Float32 SliderValue (value / SliderRange);

      _ignoreScaleChange = True;
      _mapModule->set_zoom (ZoomMin + (ZoomRange * SliderValue));
      _ignoreScaleChange = False;
   }
}


void
dmz::QtPluginCanvasZoomPanMap::slot_zoom_changed (int value) {

   if (_mapModule && !_ignoreScaleChange) {

      _ui.zoomSlider->setValue (value);
   }
}


bool
dmz::QtPluginCanvasZoomPanMap::eventFilter (QObject *obj, QEvent *event) {

   bool retVal (False);

   if (_canvasModule) {

      QGraphicsView *view (_canvasModule->get_view ());

      if (view && (obj == view)) {

         if (event->type() == QEvent::Resize) {

            QRect viewRect (view->geometry ());
            QRect myRect (geometry ());

            myRect.moveTopLeft  (viewRect.topLeft ());
            myRect.moveTop (myRect.top () + 5);
            myRect.moveLeft (myRect.left ());
            setGeometry (myRect);
         }
      }
   }

   // pass the event on to the parent class
   return QWidget::eventFilter (obj, event);
}


void
dmz::QtPluginCanvasZoomPanMap::_init (Config &local) {

   _canvasModuleName = config_to_string ("module.canvas.name", local);
   _mapModuleName = config_to_string ("module.map.name", local);

   init_input_channels (
      local,
      InputEventKeyMask | InputEventMouseMask | InputEventChannelStateMask,
      &_log);

   _mouseButton = config_to_uint32 ("mouseButton.value", local, _mouseButton);

   _scrollDelta = config_to_uint32 ("scrollDelta.value", local, _scrollDelta);

   Int32 slidderMin = config_to_int32 ("slidder.min", local, _ui.zoomSlider->minimum ());
   Int32 slidderMax = config_to_int32 ("slidder.max", local, _ui.zoomSlider->maximum ());
   
   if (slidderMin < slidderMax) {
      
      _ui.zoomSlider->setRange (slidderMin, slidderMax);
   }

   _zoomMin = config_to_float32 ("zoom.min", local, _zoomMin);
   _zoomMax = config_to_float32 ("zoom.max", local, _zoomMax);
   _zoomStep = config_to_float32 ("zoom.step", local, _zoomStep);
   _zoomDefault = config_to_float32 ("zoom.default", local, _zoomDefault);

   qwidget_config_read ("widget", local, this);
   qtoolbutton_config_read ("panLeft", local, _ui.panLeftButton);
   qtoolbutton_config_read ("panRight", local, _ui.panRightButton);
   qtoolbutton_config_read ("panUp", local, _ui.panUpButton);
   qtoolbutton_config_read ("panDown", local, _ui.panDownButton);
   qtoolbutton_config_read ("zoomAll", local, _ui.zoomAllButton);
   qtoolbutton_config_read ("zoomIn", local, _ui.zoomInButton);
   qtoolbutton_config_read ("zoomOut", local, _ui.zoomOutButton);

   if (config_to_boolean ("hide.value", local, False)) { hide (); }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginCanvasZoomPanMap (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginCanvasZoomPanMap (Info, local);
}

};
