#include <dmzInputConsts.h>
#include <dmzInputModule.h>
#include <dmzQtCanvasView.h>
#include "dmzQtModuleCanvasBasic.h"
#include <dmzQtModuleMainWindow.h>
#include <dmzQtUtil.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>


dmz::QtModuleCanvasBasic::QtModuleCanvasBasic (const PluginInfo &Info, Config &local) :
      QWidget (0),
      QtModuleCanvas (Info),
      Plugin (Info),
      _log (Info),
      _inputModule (0),
      _inputModuleName (),
      _mainWindowModule (0),
      _mainWindowModuleName (),
      _channel (0),
      _scene (),
      _canvas (0),
      _keyEvent (),
      _mouseEvent (),
      _itemTable (),
      _zoomMin (0.01f),
      _zoomMax (10.0f),
      _zoomStep (1.5f),
      _zoomDefault (1.0f) {

   _scene.setSceneRect (QRectF (-50000, -50000, 100000, 100000));
   //_scene.setItemIndexMethod (QGraphicsScene::NoIndex);

   _canvas = new QtCanvasView (this);
   _canvas->setTransformationAnchor (QGraphicsView::AnchorViewCenter);
   _canvas->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
   _canvas->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
   _canvas->setMouseTracking (true);
   //_canvas->setDragMode (QGraphicsView::ScrollHandDrag);

   setObjectName (get_plugin_name ().get_buffer ());
   _scene.setObjectName (objectName () + "Scene");
   _canvas->setObjectName (objectName () + "View");

   _canvas->setScene (&_scene);

   //_canvas->setBackgroundBrush (QPixmap ("/assets/images/background1.png"));

   QVBoxLayout *layout (new QVBoxLayout ());
   layout->addWidget (_canvas);

   setLayout (layout);
   setMouseTracking (true);

   _init (local);
}


dmz::QtModuleCanvasBasic::~QtModuleCanvasBasic () {

   _itemTable.clear ();
}


// Plugin Interface
void
dmz::QtModuleCanvasBasic::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateStart) {

      _load_session ();
      setFocus (Qt::ActiveWindowFocusReason);
   }
   else if (State == PluginStateStop) {

      _save_session ();
   }
}

void
dmz::QtModuleCanvasBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_mainWindowModule) {

         _mainWindowModule = QtModuleMainWindow::cast (PluginPtr, _mainWindowModuleName);

         if (_mainWindowModule) {

            _mainWindowModule->add_central_widget (_channel, this);
         }
      }

      if (!_inputModule) {

         _inputModule = InputModule::cast (PluginPtr, _inputModuleName);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_mainWindowModule) {

         if (_mainWindowModule == QtModuleMainWindow::cast (PluginPtr)) {

            _mainWindowModule->remove_central_widget (_channel);
            _mainWindowModule = 0;
         }
      }

      if (_inputModule && (_inputModule == InputModule::cast (PluginPtr))) {

         _inputModule = 0;
      }
   }
}


// QtModuleCanvas Interface
QGraphicsScene *
dmz::QtModuleCanvasBasic::get_scene () { return &_scene; }


QGraphicsView *
dmz::QtModuleCanvasBasic::get_view () { return _canvas; }


dmz::Boolean
dmz::QtModuleCanvasBasic::add_item (const Handle ObjectHandle, QGraphicsItem *item) {

   Boolean retVal (False);

   if (item) {

      retVal = _itemTable.store (ObjectHandle, item);

      if (retVal) {

         _scene.addItem (item);
      }
   }

   return retVal;
}


QGraphicsItem *
dmz::QtModuleCanvasBasic::lookup_item (const Handle ObjectHandle) {

   QGraphicsItem *item (_itemTable.lookup (ObjectHandle));
   return item;
}


QGraphicsItem *
dmz::QtModuleCanvasBasic::remove_item (const Handle ObjectHandle) {

   QGraphicsItem *item (_itemTable.remove (ObjectHandle));

   if (item) {

      _scene.removeItem (item);
   }

   return item;
}


void
dmz::QtModuleCanvasBasic::set_zoom_min_value (const Float32 Value) {

   _zoomMin = Value;
}


dmz::Float32
dmz::QtModuleCanvasBasic::get_zoom_min_value () const {

   return _zoomMin;
}


void
dmz::QtModuleCanvasBasic::set_zoom_max_value (const Float32 Value) {

   _zoomMax = Value;
}


dmz::Float32
dmz::QtModuleCanvasBasic::get_zoom_max_value () const {

   return _zoomMax;
}


void
dmz::QtModuleCanvasBasic::set_zoom_step_value (const Float32 Value) {

   _zoomStep = Value;
}


dmz::Float32
dmz::QtModuleCanvasBasic::get_zoom_step_value () const {

   return _zoomStep;
}


void
dmz::QtModuleCanvasBasic::set_zoom (const Float32 Value) {

   if (_canvas) {

      Float32 zoom (Value);
      if (Value > _zoomMax) { zoom = _zoomMax; }
      else if (Value < _zoomMin) { zoom = _zoomMin; }

      _canvas->set_scale (zoom);
   }
}


dmz::Float32
dmz::QtModuleCanvasBasic::get_zoom () const {

   Float32 retVal (1.0f);

   if (_canvas) { retVal = _canvas->get_scale (); }

   return retVal;
}


void
dmz::QtModuleCanvasBasic::zoom_in () {

   set_zoom (get_zoom () * _zoomStep);
}


void
dmz::QtModuleCanvasBasic::zoom_out () {

   set_zoom (get_zoom () / _zoomStep);
}


void
dmz::QtModuleCanvasBasic::zoom_extents () {

   if (_canvas) {

      _canvas->fitInView (_scene.itemsBoundingRect (), Qt::KeepAspectRatio);
      _canvas->set_scale (_canvas->get_scale ());
   }
}


void
dmz::QtModuleCanvasBasic::pan_direction (const Int32 Dx, const Int32 Dy) {

   if (_canvas) {

      _canvas->pan_direction (Dx, Dy);
   }
}


void
dmz::QtModuleCanvasBasic::center_on (const Handle ObjectHandle) {

   if (_canvas && ObjectHandle) {

      _canvas->centerOn (_itemTable.lookup (ObjectHandle));
   }
}


void
dmz::QtModuleCanvasBasic::resizeEvent (QResizeEvent *event) {

   _handle_mouse_event (0, 0);
}


void
dmz::QtModuleCanvasBasic::keyPressEvent (QKeyEvent *event) {

   if (event) { _handle_key_event (*event, True); }
}


void
dmz::QtModuleCanvasBasic::keyReleaseEvent (QKeyEvent *event) {

   if (event) { _handle_key_event (*event, False); }
}


void
dmz::QtModuleCanvasBasic::mousePressEvent (QMouseEvent *event) {

   _handle_mouse_event (event, 0);
}


void
dmz::QtModuleCanvasBasic::mouseReleaseEvent (QMouseEvent *event) {

   _handle_mouse_event (event, 0);
}


void
dmz::QtModuleCanvasBasic::mouseMoveEvent (QMouseEvent *event) {

   _handle_mouse_event (event, 0);
}


void
dmz::QtModuleCanvasBasic::wheelEvent (QWheelEvent *event) {

   _handle_mouse_event (0, event);
}


void
dmz::QtModuleCanvasBasic::_handle_key_event (
      const QKeyEvent &Event,
      const Boolean KeyState) {

   if (!Event.isAutoRepeat ()) {

      UInt32 theKey (0);

      switch (Event.key ()) {

         case Qt::Key_F1: theKey = KeyF1; break;
         case Qt::Key_F2: theKey = KeyF2; break;
         case Qt::Key_F3: theKey = KeyF3; break;
         case Qt::Key_F4: theKey = KeyF4; break;
         case Qt::Key_F5: theKey = KeyF5; break;
         case Qt::Key_F6: theKey = KeyF6; break;
         case Qt::Key_F7: theKey = KeyF7; break;
         case Qt::Key_F8: theKey = KeyF8; break;
         case Qt::Key_F9: theKey = KeyF9; break;
         case Qt::Key_F10: theKey = KeyF10; break;
         case Qt::Key_F11: theKey = KeyF11; break;
         case Qt::Key_F12: theKey = KeyF12; break;
         case Qt::Key_Left: theKey = KeyLeftArrow; break;
         case Qt::Key_Up: theKey = KeyUpArrow; break;
         case Qt::Key_Right: theKey = KeyRightArrow; break;
         case Qt::Key_Down: theKey = KeyDownArrow; break;
         case Qt::Key_PageUp: theKey = KeyPageUp; break;
         case Qt::Key_PageDown: theKey = KeyPageDown; break;
         case Qt::Key_Home: theKey = KeyHome; break;
         case Qt::Key_End: theKey = KeyEnd; break;
         case Qt::Key_Insert: theKey = KeyInsert; break;
         case Qt::Key_Space: theKey = KeySpace; break;
         case Qt::Key_Escape: theKey = KeyEsc; break;
         case Qt::Key_Tab: theKey = KeyTab; break;
         case Qt::Key_Backspace: theKey = KeyBackspace; break;
         case Qt::Key_Enter: theKey = KeyEnter; break;
         case Qt::Key_Return: theKey = KeyEnter; break;
         case Qt::Key_Delete: theKey = KeyDelete; break;
         case Qt::Key_Shift : theKey = KeyShift; break;
         case Qt::Key_Control : theKey = KeyControl; break;
         case Qt::Key_Meta : theKey = KeyMeta; break;
         case Qt::Key_Alt : theKey = KeyAlt; break;

         default:

            if (!(Event.text ().isEmpty ())) {

               theKey = Event.text ().at (0).toAscii ();
            }
      }

      _keyEvent.set_key (theKey);
      _keyEvent.set_key_state (KeyState);

      if (_inputModule) { _inputModule->send_key_event (_keyEvent); }
   }
}


void
dmz::QtModuleCanvasBasic::_handle_mouse_event (QMouseEvent *me, QWheelEvent *we) {

   if (_inputModule && _canvas) {

      InputEventMouse event (_mouseEvent);

      QPoint pointOnCanvas (event.get_mouse_x (), event.get_mouse_y ());
      QPoint pointOnScreen (event.get_mouse_screen_x (), event.get_mouse_screen_y ());

      Qt::MouseButtons buttons;

      if (me) {

         pointOnCanvas = _canvas->mapFrom (this, me->pos ());
         pointOnScreen = me->globalPos ();

         buttons = me->buttons ();
      }
      else if (we) {

         pointOnCanvas = _canvas->mapFrom (this, we->pos ());
         pointOnScreen = we->globalPos ();

         buttons = we->buttons ();
      }

      event.set_mouse_position (pointOnCanvas.x (), pointOnCanvas.y ());
      event.set_mouse_screen_position (pointOnScreen.x (), pointOnScreen.y ());

      UInt32 mask (0);
      if (buttons & Qt::LeftButton) { mask |= 0x01 << 0; }
      if (buttons & Qt::RightButton) { mask |= 0x01 << 1; }
      if (buttons & Qt::MidButton) { mask |= 0x01 << 2; }

      event.set_button_mask (mask);

      Int32 deltaX (0), deltaY (0);

      if (we) {

         if (we->orientation () == Qt::Vertical) { deltaY = we->delta (); }
         else if (we->orientation () == Qt::Horizontal) { deltaX = we->delta (); }
      }

      event.set_scroll_delta (deltaX, deltaY);

      event.set_window_size (width (), height ());

      if (_mouseEvent.update (event)) {

         _inputModule->send_mouse_event (_mouseEvent);
      }
   }
}


void
dmz::QtModuleCanvasBasic::_save_session () {

   // String data;
   //
   // Config session (get_plugin_name ());
   //
   // if (_canvas) {
   //
   //    session.add_config (qgraphicsview_to_config ("canvasView", *_canvas));
   // }
   //
   // set_session_config (get_plugin_runtime_context (), session);
}


void
dmz::QtModuleCanvasBasic::_load_session () {

   // Config session (get_session_config (get_plugin_name (), get_plugin_runtime_context ()));
   //
   // if (_canvas) {
   //
   //    qgraphicsview_config_read ("canvasView", session, *_canvas);
   //    _canvas->set_scale (_canvas->get_scale ());
   // }
}


void
dmz::QtModuleCanvasBasic::_init (Config &local) {

   _inputModuleName = config_to_string ("module.input.name", local);
   _mainWindowModuleName = config_to_string ("module.mainWindow.namne", local);

   _channel = config_to_named_handle (
      "channel.name",
      local,
      InputChannelDefaultName,
      get_plugin_runtime_context ());

   if (_canvas) {

      _keyEvent.set_source_handle (get_plugin_handle ());
      _mouseEvent.set_source_handle (get_plugin_handle ());

      String value;

      Boolean enableOpenGL (False);

      if (local.lookup_attribute ("canvas.opengl", value)) {

         if (value.to_lower () == "true") { enableOpenGL = True; }
      }

      Boolean enableAntialiasing (False);
      if (local.lookup_attribute ("canvas.antialiasing", value)) {

         if (value.to_lower () == "true") { enableAntialiasing = True; }
      }

      if (enableOpenGL) {

         _canvas->setViewport (new QGLWidget (QGLFormat  (QGL::SampleBuffers)));
         _log.info << "OpenGL enabled" << endl;
      }

      if (enableAntialiasing) {

         if (enableOpenGL) {

            _canvas->setRenderHint (QPainter::HighQualityAntialiasing, true);
            _log.info << "High Quality Antialiasing enabled" << endl;
         }
         else {

            _canvas->setRenderHint (QPainter::Antialiasing, true);
            _log.info << "Antialiasing enabled" << endl;
         }
      }

      if (local.lookup_attribute ("canvas.textantialiasing", value)) {

         if (value.to_lower () == "true") {

            _canvas->setRenderHint (QPainter::TextAntialiasing, true);
            _log.info << "Test Antialiasing enabled" << endl;
         }
      }

      _canvas->setRenderHint (QPainter::SmoothPixmapTransform, true);

      set_zoom_min_value (config_to_float32 ("canvas.zoom.min", local, _zoomMin));
      set_zoom_max_value (config_to_float32 ("canvas.zoom.max", local, _zoomMax));
      set_zoom_step_value (config_to_float32 ("canvas.zoom.step", local, _zoomStep));
      set_zoom (config_to_float32 ("canvas.zoom.default", local, _zoomDefault));
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtModuleCanvasBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtModuleCanvasBasic (Info, local);
}

};
