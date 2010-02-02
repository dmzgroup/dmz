#include <dmzInputConsts.h>
#include <dmzInputModule.h>
#include <dmzQtCanvasView.h>
#include "dmzQtModuleCanvasBasic.h"
#include <dmzQtUtil.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>

#include <QtCore/QDebug>


dmz::QtModuleCanvasBasic::QtModuleCanvasBasic (const PluginInfo &Info, Config &local) :
      QFrame (0),
      QtModuleCanvas (Info),
      Plugin (Info),
      QtWidget (Info),
      _log (Info),
      _inputModule (0),
      _inputModuleName (),
      _scene (),
      _canvas (0),
      _keyEvent (),
      _mouseEvent (),
      _ignoreEvents (False),
      _itemTable (),
      _zoomMin (0.01f),
      _zoomMax (10.0f),
      _zoomStep (1.5f),
      _zoomDefault (1.0f),
      _drawGrid (True) {

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

      setFocus (Qt::ActiveWindowFocusReason);
   }
   else if (State == PluginStateStop) {

   }
}

void
dmz::QtModuleCanvasBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_inputModule) {

         _inputModule = InputModule::cast (PluginPtr, _inputModuleName);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

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


void
dmz::QtModuleCanvasBasic::set_background_transparent (const Boolean Value) {

   if (Value) {

      _scene.enableGrid (False);

      QPalette palette = _canvas->palette ();
      palette.setBrush (QPalette::Base, Qt::transparent);
      _canvas->setPalette (palette);
      _canvas->setAttribute (Qt::WA_OpaquePaintEvent, false);
   }
   else {
      
      _scene.enableGrid (_drawGrid);

      QPalette palette = _canvas->palette ();
      palette.setBrush (QPalette::Base, Qt::white);
      _canvas->setPalette (palette);
      _canvas->setAttribute (Qt::WA_OpaquePaintEvent, true);
   }
}


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

   if (item && (item->scene () == &_scene)) {

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

    QGraphicsItem *item = _itemTable.lookup (ObjectHandle);

   if (_canvas && item) {

      _canvas->centerOn (item);
      _canvas->set_scale (_canvas->get_scale ());
   }
}


void
dmz::QtModuleCanvasBasic::center_on (const QPointF &Value) {

   if (_canvas) {

      _canvas->centerOn (Value);
      _canvas->set_scale (_canvas->get_scale ());
   }
}


QPointF
dmz::QtModuleCanvasBasic::get_center () const {

   QPointF retVal (0, 0);
   
   if (_canvas) {
      
      QRect vpRect (_canvas->rect ());
      retVal = _canvas->mapToScene (vpRect.center ());
   }
   
   return retVal;
}


// QtWidget Interface
QWidget *
dmz::QtModuleCanvasBasic::get_qt_widget () { return this; }


void
dmz::QtModuleCanvasBasic::resizeEvent (QResizeEvent *event) {
   
   if (event) {
      
      if (_ignoreEvents) { event->ignore (); }
      else { _handle_mouse_event (0, 0); }
   }
}


void
dmz::QtModuleCanvasBasic::keyPressEvent (QKeyEvent *event) {

   if (event) {
      
      if (_ignoreEvents) { event->ignore (); }
      else { _handle_key_event (*event, True); }
   }
}


void
dmz::QtModuleCanvasBasic::keyReleaseEvent (QKeyEvent *event) {

   if (event) {
      
      if (_ignoreEvents) { event->ignore (); }
      else { _handle_key_event (*event, False); }
   }
}


void
dmz::QtModuleCanvasBasic::mousePressEvent (QMouseEvent *event) {

   if (event) {
      
      if (_ignoreEvents) { event->ignore (); }
      else { _handle_mouse_event (event, 0); }
   }
}


void
dmz::QtModuleCanvasBasic::mouseReleaseEvent (QMouseEvent *event) {

   if (event) {
      
      if (_ignoreEvents) { event->ignore (); }
      else { _handle_mouse_event (event, 0); }
   }
}


void
dmz::QtModuleCanvasBasic::mouseMoveEvent (QMouseEvent *event) {

   if (event) {
      
      if (_ignoreEvents) { event->ignore (); }
      else { _handle_mouse_event (event, 0); }
   }
}


void
dmz::QtModuleCanvasBasic::wheelEvent (QWheelEvent *event) {

   if (event) {
      
      if (_ignoreEvents) { event->ignore (); }
      else { _handle_mouse_event (0, event); }
   }
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
dmz::QtModuleCanvasBasic::_init (Config &local) {

   qframe_config_read ("frame", local, this);

   const Int32 MinX = config_to_int32 ("scene.min.x", local, -50000);
   const Int32 MinY = config_to_int32 ("scene.min.y", local, -50000);
   const Int32 MaxX = config_to_int32 ("scene.max.x", local, 100000);
   const Int32 MaxY = config_to_int32 ("scene.max.y", local, 100000);

   _scene.setSceneRect (QRectF (MinX, MinY, MaxX - MinX, MaxY - MinY));

   const Int32 IndexMethod (config_to_int32 ("scene.itemIndexMethod", local, 0));
   
   if (IndexMethod == QGraphicsScene::NoIndex) {
      
      _scene.setItemIndexMethod (QGraphicsScene::NoIndex);
   }
   
   _drawGrid = config_to_boolean ("scene.background.grid", local, _drawGrid);

   _canvas = new QtCanvasView (this);
   _canvas->setMouseTracking (true);

   set_background_transparent (config_to_boolean ("scene.background.transparent", local, False));
   
   // if (config_to_boolean ("scene.background.transparent", local, False)) {
   //    
   //    _scene.enableGrid (False);
   //    
   //    QPalette palette = _canvas->palette ();
   //    palette.setBrush (QPalette::Base, Qt::transparent);
   //    _canvas->setPalette (palette);
   //    _canvas->setAttribute (Qt::WA_OpaquePaintEvent, false);
   // }

   _canvas->setTransformationAnchor (QGraphicsView::AnchorViewCenter);
   _canvas->setResizeAnchor (QGraphicsView::AnchorViewCenter);
   
   const Boolean ScrollBars = config_to_boolean ("scrollbars.value", local, False);

   if (!ScrollBars) {

      _canvas->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
      _canvas->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
   }

   setObjectName (get_plugin_name ().get_buffer ());
   _scene.setObjectName (objectName () + "Scene");
   _canvas->setObjectName (objectName () + "View");

   _canvas->setScene (&_scene);

   QVBoxLayout *layout (new QVBoxLayout ());
   layout->setSpacing (0);
   layout->setContentsMargins (0, 0, 0, 0);
   layout->addWidget (_canvas);
   
   setLayout (layout);
   setMouseTracking (true);
   _inputModuleName = config_to_string ("module.input.name", local);

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
      
      _ignoreEvents = config_to_boolean ("canvas.ignoreevents", local, _ignoreEvents);
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
