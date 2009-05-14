#include <dmzInputEventKey.h>
#include <dmzInputEventMouse.h>
#include <dmzInputEventMasks.h>
#include <dmzQtConfigRead.h>
#include <dmzQtModuleCanvas.h>
#include <dmzQtModuleMainWindow.h>
#include "dmzQtPluginCanvasZoomPanMap.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
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
      _zoomStep (1),
      _zoomDefault (0) {

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

               connect (
                  view, SIGNAL (scale_changed (qreal)),
                  this, SLOT (slot_scale_changed (qreal)));

               view->installEventFilter (this);
               setParent (view);
            }

            _canvasModule->set_zoom_min_value (local_level_to_scale (_zoomMin));
            _canvasModule->set_zoom_max_value (local_level_to_scale (_zoomMax));
            _canvasModule->set_zoom_step_value (_zoomStep);
            _canvasModule->set_zoom (local_level_to_scale (_zoomDefault));
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
            Value.get_mouse_delta_x (),
            Value.get_mouse_delta_y ());
      }
      else if (Value.get_scroll_delta_y ()) {

view->centerOn (0.0, 0.0);
         // if (Value.get_scroll_delta_y () >= 1) {
         // 
         //    _ui.zoomInButton->click ();
         // }
         // else {
         // 
         //    _ui.zoomOutButton->click ();
         // }

//          QGraphicsView *view (_canvasModule->get_view ());
//          QSize vpSize (view->viewport ()->size ());
//          QPoint center (vpSize.width () / 2, vpSize.height () / 2);
// qWarning () << "world center: " << center;
// qWarning () << "scene center: " << view->mapToScene (center);

//_log.warn << "center: " << center.x () << " " << center.y () << endl;
//         result.add_config (qpointf_to_config ("center", Value.mapToScene (center)));
         
//         _map->screenToWorldCoordinate (const QPoint &Screen);
      }
   }
}


void
dmz::QtPluginCanvasZoomPanMap::on_panLeftButton_clicked () {

   if (_canvasModule) {

      _canvasModule->pan_direction (_scrollDelta, 0);
   }
}


void
dmz::QtPluginCanvasZoomPanMap::on_panRightButton_clicked () {

   if (_canvasModule) {

      _canvasModule->pan_direction (-(Int32)_scrollDelta, 0);
   }
}

void
dmz::QtPluginCanvasZoomPanMap::on_panUpButton_clicked () {

   if (_canvasModule) {

      _canvasModule->pan_direction (0, _scrollDelta);
   }
}


void
dmz::QtPluginCanvasZoomPanMap::on_panDownButton_clicked () {

   if (_canvasModule) {

      _canvasModule->pan_direction (0, -(Int32)_scrollDelta);
   }
}


void
dmz::QtPluginCanvasZoomPanMap::on_zoomAllButton_clicked () {

   if (_canvasModule) {

//      _canvasModule->zoom_extents ();
      _canvasModule->set_zoom (local_level_to_scale (_zoomMin));
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
      
      Float64 l2s (local_level_to_scale (value));
      Int32 s2l (local_scale_to_level (l2s));
   
      _log.error << "--------------- on_zoomSlider_valueChanged ----------------------------" << endl;
      _log.warn << (Int32)value << " --> " << l2s << " --> " << s2l << endl;
      _log.error << "-------------------------------------------" << endl;
      
      _ignoreScaleChange = True;
      _canvasModule->set_zoom (local_level_to_scale (value));
      _ignoreScaleChange = False;
   }
}


void
dmz::QtPluginCanvasZoomPanMap::slot_scale_changed (qreal value) {

   if (_canvasModule && !_ignoreScaleChange) {

// FIXME
      _log.error << "--------------- slot_scale_changed ----------------------------" << endl;
      Int32 s2l (local_scale_to_level (value));
      _log.warn << value << " --> " << s2l << endl;
      _log.error << "-------------------------------------------" << endl;
      
      _ui.zoomSlider->setValue (local_scale_to_level (value));
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

            myRect.moveTopLeft (viewRect.topLeft ());
            myRect.moveTop (myRect.top () + 5);
            myRect.moveLeft (myRect.left () + 5);
            setGeometry (myRect);
         }
      }
   }

   // pass the event on to the parent class
   return QWidget::eventFilter (obj, event);
}


void
dmz::QtPluginCanvasZoomPanMap::_location_coord (Float64 lat, Float64 lon, Int32 zoom) { 
   
  if (abs (lat) > 85.0511287798066) 
    return;
    
  Float64 sin_phi = sin (to_radians (lat)); 
  Float64 norm_x = lon / 180; 
  Float64 norm_y = (0.5 * log ((1 + sin_phi) / (1 - sin_phi))) / Pi64; 
  Float64 tileRow = pow (2, zoom) * ((1 - norm_y) / 2); 
  Float64 tileColumn = pow (2, zoom) * ((norm_x + 1) / 2); 
  
_log.error << "tileRow: " << (Int32)tileRow << endl;
_log.error << "tileCol: " << (Int32)tileColumn << endl;
_log.error << "   zoom: " << zoom << endl;
}


#if 0
public static Point getTileXY(double lat, double lon, int zoom) {
   
   // Per se earth coordinates are -90?<= lat <= +90?and -180?<= lon <= 180?
   // Mercator is NOT applicable for abs(lat) > 85.0511287798066 degrees
   if (System.Math.Abs(lat) > 85.0511287798066)
       return new Point(0, 0);
       
   // latitude (deg) -> latitude (rad)
   double sin_phi = System.Math.Sin(lat * System.Math.PI / 180);
   
   // Use Mercator to calculate x and normalize this to -1 <= x <= +1 by division by 180.
   // The center of the map is lambda_0 = 0, hence x = lat. 
   double norm_x = lon / 180;

   // Use Mercator to calculate y and normalize this to -1 <= y <= +1 by division by PI. 
   // According to Mercator y is defined in the range of -PI <= y <= PI only 
   double norm_y = (0.5 * System.Math.Log((1 + sin_phi) / (1 - sin_phi))) / System.Math.PI;

   // Apply the normalized point coordinations to any application, here the coordinates of 
   // a 256*256 image holding that point in a thought grid with origin 0,0 top/left
   double tileRow = System.Math.Pow(2, 17 - zoom) * ((1 - norm_y) / 2);
   double tileColumn = System.Math.Pow(2, 17 - zoom) * ((norm_x + 1) / 2);
   
   return new Point((int)tileColumn, (int)tileRow);
}

public static Point getXYPixelFromTile(double lat, double lon, int zoom) {
   
   // Per se earth coordinates are -90?<= lat <= +90?and -180?<= lon <= 180?
   // Mercator is NOT applicable for abs(lat) > 85.0511287798066 degrees
   if (System.Math.Abs(lat) > 85.0511287798066)
       return new Point(0, 0);
   
   // latitude (deg) -> latitude (rad)
   double sin_phi = System.Math.Sin(lat * System.Math.PI / 180);
   
   // Use Mercator to calculate x and normalize this to -1 <= x <= +1 by division by 180.
   // The center of the map is lambda_0 = 0, hence x = lat. 
   double norm_x = lon / 180;

   // Use Mercator to calculate y and normalize this to -1 <= y <= +1 by division by PI. 
   // According to Mercator y is defined in the range of -PI <= y <= PI only 
   double norm_y = (0.5 * System.Math.Log((1 + sin_phi) / (1 - sin_phi))) / System.Math.PI;
   
   // Apply the normalized point coordinations to any application, here the coordinates of 
   // a 256*256 image holding that point in a thought grid with origin 0,0 top/left
   double tileRow = System.Math.Pow(2, 17 - zoom) * ((1 - norm_y) / 2);
   double tileColumn = System.Math.Pow(2, 17 - zoom) * ((norm_x + 1) / 2);
   
   int y = (int)((tileColumn - (int)tileColumn) * 256);
   int x = (int)((tileRow - (int)tileRow) * 256);
   
   Console.Out.WriteLine(x +" " + y);
   
   return new Point(y,x);
}
#endif


void
dmz::QtPluginCanvasZoomPanMap::_init (Config &local) {

   _canvasModuleName = config_to_string ("module.canvas.name", local);
   _mainWindowModuleName = config_to_string ("module.mainWindow.name", local);

   init_input_channels (
      local,
      InputEventKeyMask | InputEventMouseMask | InputEventChannelStateMask,
      &_log);

   _mouseButton = config_to_uint32 ("mouseButton.value", local, _mouseButton);

   _scrollDelta = config_to_uint32 ("scrollDelta.value", local, _scrollDelta);

   Int32 slidderMin = config_to_int32 ("slidder.min", local, 0);
   Int32 slidderMax = config_to_int32 ("slidder.max", local, 17);
   
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
   
//_location_coord (52.5211, 13.4122, 10);
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
