#include <dmzInputEventKey.h>
#include <dmzInputEventMouse.h>
#include <dmzInputEventMasks.h>
#include <dmzQtConfigRead.h>
#include <dmzQtModuleMap.h>
#include <dmzQtModuleMainWindow.h>
#include "dmzQtPluginMapZoomPan.h"
#include <dmzQtWidget.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <qmapcontrol.h>
#include <QtGui/QtGui>


dmz::QtPluginMapZoomPan::QtPluginMapZoomPan (
      const PluginInfo &Info,
      Config &local) :
      QWidget (0),
      Plugin (Info),
      InputObserverUtil (Info, local),
      _log (Info),
      _rti (Info, &_log),
      _mapModule (0),
      _mapModuleName (),
      _mainWindowModule (0),
      _mainWindowModuleName (),
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


dmz::QtPluginMapZoomPan::~QtPluginMapZoomPan () {

}


// Plugin Interface
void
dmz::QtPluginMapZoomPan::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateStart) {

      _load_session ();
   }
   else if (State == PluginStateStop) {

      _save_session ();
   }
}


void
dmz::QtPluginMapZoomPan::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_mapModule) {

         _mapModule = QtModuleMap::cast (PluginPtr, _mapModuleName);

         if (_mapModule) {

            _source = (PluginPtr ? PluginPtr->get_plugin_handle () : 0);

            qmapcontrol::MapControl *map (_mapModule->get_map_control ());

            if (map) {
               
               // connect (
               //    view, SIGNAL (scale_changed (qreal)),
               //    this, SLOT (slot_scale_changed (qreal)));
               
               map->installEventFilter (this);
               setParent (map);
            }

           _mapModule->set_zoom_min_value (_zoomMin);
           _mapModule->set_zoom_max_value (_zoomMax);
           _mapModule->set_zoom (_zoomDefault);
         }
      }

      if (!_mainWindowModule) {

         _mainWindowModule = QtModuleMainWindow::cast (PluginPtr, _mainWindowModuleName);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_mapModule && (_mapModule == QtModuleMap::cast (PluginPtr))) {

         qmapcontrol::MapControl *map (_mapModule->get_map_control ());

         if (map) {

            map->removeEventFilter (this);
            
            hide ();
            setParent (0);
         }

         _mapModule = 0;
      }
   }
}


// Input Observer Interface
void
dmz::QtPluginMapZoomPan::update_channel_state (
      const UInt32 Channel,
      const Boolean State) {

   _active += (State ? 1 : -1);
}


void
dmz::QtPluginMapZoomPan::receive_key_event (
      const UInt32 Channel,
      const InputEventKey &Value) {

//   _log.out << "receive_key_event" << endl;
}


void
dmz::QtPluginMapZoomPan::receive_mouse_event (
      const UInt32 Channel,
      const InputEventMouse &Value) {

   if (_active && _mapModule && (Value.get_source_handle () == _source)) {

      qmapcontrol::MapControl *map (_mapModule->get_map_control ());
      
      if (Value.is_button_changed (_mouseButton)) {

         if (Value.is_button_pressed (_mouseButton)) {

            _handScrolling = True;

            if (map) { map->setCursor (Qt::ClosedHandCursor); }
         }
         else {

            _handScrolling = False;

            if (map) { map->setCursor (Qt::ArrowCursor); }
         }
      }
      else if (_handScrolling) {

         _mapModule->pan_direction (
            -Value.get_mouse_delta_x (),
            -Value.get_mouse_delta_y ());
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
dmz::QtPluginMapZoomPan::on_panLeftButton_clicked () {

   if (_mapModule) {

      _mapModule->pan_direction (-(Int32)_scrollDelta, 0);
   }
}


void
dmz::QtPluginMapZoomPan::on_panRightButton_clicked () {

   if (_mapModule) {

      _mapModule->pan_direction (_scrollDelta, 0);
   }
}

void
dmz::QtPluginMapZoomPan::on_panUpButton_clicked () {

   if (_mapModule) {

      _mapModule->pan_direction (0, -(Int32)_scrollDelta);
   }
}


void
dmz::QtPluginMapZoomPan::on_panDownButton_clicked () {

   if (_mapModule) {

      _mapModule->pan_direction (0, _scrollDelta);
   }
}


void
dmz::QtPluginMapZoomPan::on_zoomAllButton_clicked () {

   if (_mapModule) {
   
      _mapModule->set_zoom (_mapModule->get_zoom_min_value ());
   }
}


void
dmz::QtPluginMapZoomPan::on_zoomInButton_clicked () {

   _ui.zoomSlider->triggerAction (QAbstractSlider::SliderSingleStepAdd);
}


void
dmz::QtPluginMapZoomPan::on_zoomOutButton_clicked () {

   _ui.zoomSlider->triggerAction (QAbstractSlider::SliderSingleStepSub);
}


void
dmz::QtPluginMapZoomPan::on_zoomSlider_valueChanged (int value) {

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
dmz::QtPluginMapZoomPan::slot_scale_changed (qreal value) {

   if (_mapModule && !_ignoreScaleChange) {

      const Float32 ZoomMin (_mapModule->get_zoom_min_value ());
      const Float32 ZoomMax (_mapModule->get_zoom_max_value ());
      const Float32 ZoomRange (ZoomMax - ZoomMin);
      const Float32 SliderRange (_ui.zoomSlider->maximum () - _ui.zoomSlider->minimum ());
      const Float32 SliderValue ((value - ZoomMin) / ZoomRange);

      _ui.zoomSlider->setValue (SliderValue * SliderRange);
   }
}


bool
dmz::QtPluginMapZoomPan::eventFilter (QObject *obj, QEvent *event) {

   bool retVal (False);

   if (_mapModule) {

      qmapcontrol::MapControl *map (_mapModule->get_map_control ());
      
      if (map && (obj == map)) {

         if (event->type() == QEvent::Resize) {

            QRect viewRect (map->geometry ());
            QRect myRect (geometry ());

            myRect.moveTopRight (viewRect.topRight ());
            myRect.moveTop (myRect.top () + 5);
            myRect.moveRight (myRect.right () - 20);
            setGeometry (myRect);
         }
      }
   }

   // pass the event on to the parent class
   return QWidget::eventFilter (obj, event);
}


void
dmz::QtPluginMapZoomPan::_save_session () {

//   String data;

//   Config session (get_plugin_name ());

//   Config window ("window");

//   session.add_config (window);

//   set_session_config (get_plugin_runtime_context (), session);
}


void
dmz::QtPluginMapZoomPan::_load_session () {

//   Config session (get_session_config (get_plugin_name (), get_plugin_runtime_context ()));
}


void
dmz::QtPluginMapZoomPan::_init (Config &local) {

   _mapModuleName = config_to_string ("module.canvas.name", local);
   _mainWindowModuleName = config_to_string ("module.mainWindow.name", local);

   init_input_channels (
      local,
      InputEventKeyMask | InputEventMouseMask | InputEventChannelStateMask,
      &_log);

   _mouseButton = config_to_uint32 ("mouseButton.value", local, _mouseButton);

   _scrollDelta = config_to_uint32 ("scrollDelta.value", local, _scrollDelta);

   _zoomMin = config_to_float32 ("zoom.min", local, _zoomMin);
   _zoomMax = config_to_float32 ("zoom.max", local, _zoomMax);
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
create_dmzQtPluginMapZoomPan (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginMapZoomPan (Info, local);
}

};
