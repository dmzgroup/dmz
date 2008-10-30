#include <dmzInputEventKey.h>
#include <dmzInputEventMouse.h>
#include <dmzInputEventMasks.h>
#include <dmzQtConfigRead.h>
#include <dmzQtModuleCanvas.h>
#include <dmzQtModuleMainWindow.h>
#include "dmzQtPluginCanvasZoomPan.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <QtGui/QtGui>


dmz::QtPluginCanvasZoomPan::QtPluginCanvasZoomPan (
      const PluginInfo &Info,
      Config &local) :
      QWidget (0),
      Plugin (Info),
      InputObserverUtil (Info, local),
      _log (Info),
      _rti (Info, &_log),
      _canvasModule (0),
      _canvasModuleName (),
      _mainWindowModule (0),
      _mainWindowModuleName (),
      _source (0),
      _active (0),
      _mouseButton (2),
      _ignoreScaleChange (False),
      _handScrolling (False),
      _scrollDelta (10),
      _zoomMin (0.5f),
      _zoomMax (2.0f),
      _zoomStep (1.1f),
      _zoomDefault (1.0f) {

   setObjectName (get_plugin_name ().get_buffer ());

   _ui.setupUi (this);

   _init (local);

   hide ();
}


dmz::QtPluginCanvasZoomPan::~QtPluginCanvasZoomPan () {

}


// Plugin Interface
void
dmz::QtPluginCanvasZoomPan::update_plugin_state (
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
dmz::QtPluginCanvasZoomPan::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_canvasModule) {

         _canvasModule = QtModuleCanvas::cast (PluginPtr, _canvasModuleName);

         if (_canvasModule) {

            _source = (PluginPtr ? PluginPtr->get_plugin_handle () : 0);

            QGraphicsView *view = _canvasModule->get_view ();

            if (view) {

               connect (
                  view, SIGNAL (scale_changed (qreal)),
                  this, SLOT (slot_scale_changed (qreal)));

               view->installEventFilter (this);
               setParent (view);
            }

            _canvasModule->set_zoom_min_value (_zoomMin);
            _canvasModule->set_zoom_max_value (_zoomMax);
            _canvasModule->set_zoom_step_value (_zoomStep);
            _canvasModule->set_zoom (_zoomDefault);
         }
      }

      if (!_mainWindowModule) {

         _mainWindowModule = QtModuleMainWindow::cast (PluginPtr, _mainWindowModuleName);
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
   }
}


// Input Observer Interface
void
dmz::QtPluginCanvasZoomPan::update_channel_state (
      const UInt32 Channel,
      const Boolean State) {

   _active += (State ? 1 : -1);
}


void
dmz::QtPluginCanvasZoomPan::receive_key_event (
      const UInt32 Channel,
      const InputEventKey &Value) {

//   _log.out << "receive_key_event" << endl;
}


void
dmz::QtPluginCanvasZoomPan::receive_mouse_event (
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
            Value.get_mouse_delta_x (),
            Value.get_mouse_delta_y ());
      }
      else if (Value.get_scroll_delta_y ()) {

         if (view) {

            QGraphicsView::ViewportAnchor previousAnchor (view->transformationAnchor ());
            view->setTransformationAnchor (QGraphicsView::AnchorUnderMouse);

            if (Value.get_scroll_delta_y () >= 1) {

               _ui.zoomInButton->click ();
            }
            else {

               _ui.zoomOutButton->click ();
            }

            view->setTransformationAnchor (previousAnchor);
         }
      }
   }
}


void
dmz::QtPluginCanvasZoomPan::on_panLeftButton_clicked () {

   if (_canvasModule) {

      _canvasModule->pan_direction (_scrollDelta, 0);
   }
}


void
dmz::QtPluginCanvasZoomPan::on_panRightButton_clicked () {

   if (_canvasModule) {

      _canvasModule->pan_direction (-(Int32)_scrollDelta, 0);
   }
}

void
dmz::QtPluginCanvasZoomPan::on_panUpButton_clicked () {

   if (_canvasModule) {

      _canvasModule->pan_direction (0, _scrollDelta);
   }
}


void
dmz::QtPluginCanvasZoomPan::on_panDownButton_clicked () {

   if (_canvasModule) {

      _canvasModule->pan_direction (0, -(Int32)_scrollDelta);
   }
}


void
dmz::QtPluginCanvasZoomPan::on_zoomAllButton_clicked () {

   if (_canvasModule) {

      _canvasModule->zoom_extents ();
   }
}


void
dmz::QtPluginCanvasZoomPan::on_zoomInButton_clicked () {

   _ui.zoomSlider->triggerAction (QAbstractSlider::SliderSingleStepAdd);
}


void
dmz::QtPluginCanvasZoomPan::on_zoomOutButton_clicked () {

   _ui.zoomSlider->triggerAction (QAbstractSlider::SliderSingleStepSub);
}


void
dmz::QtPluginCanvasZoomPan::on_zoomSlider_valueChanged (int value) {

   if (_canvasModule) {

      const Float32 ZoomMin (_canvasModule->get_zoom_min_value ());
      const Float32 ZoomMax (_canvasModule->get_zoom_max_value ());
      const Float32 ZoomRange (ZoomMax - ZoomMin);
      const Float32 SliderRange (_ui.zoomSlider->maximum () - _ui.zoomSlider->minimum ());
      const Float32 SliderValue (value / SliderRange);

      _ignoreScaleChange = True;
      _canvasModule->set_zoom (ZoomMin + (ZoomRange * SliderValue));
      _ignoreScaleChange = False;
   }
}


void
dmz::QtPluginCanvasZoomPan::slot_scale_changed (qreal value) {

   if (_canvasModule && !_ignoreScaleChange) {

      const Float32 ZoomMin (_canvasModule->get_zoom_min_value ());
      const Float32 ZoomMax (_canvasModule->get_zoom_max_value ());
      const Float32 ZoomRange (ZoomMax - ZoomMin);
      const Float32 SliderRange (_ui.zoomSlider->maximum () - _ui.zoomSlider->minimum ());
      const Float32 SliderValue ((value - ZoomMin) / ZoomRange);

      _ui.zoomSlider->setValue (SliderValue * SliderRange);
   }
}


bool
dmz::QtPluginCanvasZoomPan::eventFilter (QObject *obj, QEvent *event) {

   bool retVal (False);

   if (_canvasModule) {

      QGraphicsView *view (_canvasModule->get_view ());

      if (view && (obj == view)) {

         if (event->type() == QEvent::Resize) {

            QRect viewRect (view->geometry ());
            QRect myRect (geometry ());

            myRect.moveTopRight (viewRect.topRight ());
            myRect.moveRight (myRect.right () - 20);
            setGeometry (myRect);
         }
      }
   }

   // pass the event on to the parent class
   return QWidget::eventFilter (obj, event);
}


void
dmz::QtPluginCanvasZoomPan::_save_session () {

//   String data;

//   Config session (get_plugin_name ());

//   Config window ("window");

//   session.add_config (window);

//   set_session_config (get_plugin_runtime_context (), session);
}


void
dmz::QtPluginCanvasZoomPan::_load_session () {

//   Config session (get_session_config (get_plugin_name (), get_plugin_runtime_context ()));
}


void
dmz::QtPluginCanvasZoomPan::_init (Config &local) {

   _canvasModuleName = config_to_string ("module.canvas.name", local);
   _mainWindowModuleName = config_to_string ("module.mainWindow.name", local);

   init_input_channels (
      local,
      InputEventKeyMask | InputEventMouseMask | InputEventChannelStateMask,
      &_log);

   _mouseButton = config_to_uint32 ("mouseButton.value", local, _mouseButton);

   _scrollDelta = config_to_uint32 ("scrollDelta.value", local, _scrollDelta);

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
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginCanvasZoomPan (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginCanvasZoomPan (Info, local);
}

};
