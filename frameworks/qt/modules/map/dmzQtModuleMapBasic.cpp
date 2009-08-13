#include <dmzInputConsts.h>
#include <dmzInputModule.h>
#include "dmzQtModuleMapBasic.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzQtUtil.h>
#include <dmzRenderModulePick.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <dmzSystemFile.h>
#include <qmapcontrol.h>
#include <QtGui/QtGui>


dmz::QtModuleMapBasic::QtModuleMapBasic (const PluginInfo &Info, Config &local) :
      QWidget (0),
      Plugin (Info),
      RenderModulePickConvert (Info),
      QtWidget (Info),
      QtModuleMap (Info),
      _log (Info),
      _pickModule (0),
      _pickModuleName (),
      _inputModule (0),
      _inputModuleName (),
      _keyEvent (),
      _mouseEvent (),
      _ignoreEvents (False),
      _itemTable (),
      _map (0),
      _defaultAdapter (0),
      _mapAdapter (0),
      _baseLayer (0),
      _geomLayer (0),
      _zoomMin (0),
      _zoomMax (17),
      _zoomDefault (_zoomMin),
      _cacheDir () {

   _init (local);
}


dmz::QtModuleMapBasic::~QtModuleMapBasic () {

   _itemTable.clear ();
}


// Plugin Interface
void
dmz::QtModuleMapBasic::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

      _load_session ();
      setFocus (Qt::ActiveWindowFocusReason);
   }
   else if (State == PluginStateStop) {

      _save_session ();
   }
   else if (State == PluginStateShutdown) {

      use_default_map_adapter ();
   }
}


void
dmz::QtModuleMapBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_inputModule) {

         _inputModule = InputModule::cast (PluginPtr, _inputModuleName);
      }
      
      if (!_pickModule) {

         _pickModule = RenderModulePick::cast (PluginPtr, _pickModuleName);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_inputModule && (_inputModule == InputModule::cast (PluginPtr))) {

         _inputModule = 0;
      }
      
      if (_pickModule && (_pickModule == RenderModulePick::cast (PluginPtr))) {

         _pickModule = 0;
      }
   }
}


// RenderModulePickConvert
dmz::Boolean
dmz::QtModuleMapBasic::source_to_world (
      const Int32 ScreenPosX,
      const Int32 ScreenPosY,
      Vector &worldPosition,
      Vector &normal,
      Handle &objectHandle) {

   Boolean retVal (False);

   if (_pickModule) {
      
      retVal = _pickModule->source_to_world (
         get_plugin_handle (), ScreenPosX, ScreenPosY, worldPosition, normal, objectHandle);
   }

   return retVal;
}


dmz::Boolean
dmz::QtModuleMapBasic::world_to_source (
      const Vector &WorldPosition,
      Int32 &screenPosX,
      Int32 &screenPosY) {

   Boolean retVal (False);

   if (_pickModule) {
      
      retVal = _pickModule->world_to_source (
         get_plugin_handle (), WorldPosition, screenPosX, screenPosY);
   }

   return retVal;   
}


// QtWidget Interface
QWidget *
dmz::QtModuleMapBasic::get_qt_widget () { return this; }


// QtModuleMap
qmapcontrol::MapControl *
dmz::QtModuleMapBasic::get_map_control () { return _map; }


void
dmz::QtModuleMapBasic::use_default_map_adapter () {

   set_map_adapter (_defaultAdapter);
}


dmz::String
dmz::QtModuleMapBasic::get_tile_cache_dir () const {
   
   return _cacheDir;
}


void
dmz::QtModuleMapBasic::empty_tile_cache () {

   String file;
   PathContainer fileList;

   if (get_file_list (_cacheDir, fileList)) {
      
      Boolean found (fileList.get_first (file));
         
      while (found) {
      
         const String CleanPath (format_path (_cacheDir + "/" + file));
         if (get_absolute_path (CleanPath, file))  {

            remove_file (file);
         }
         
         found = fileList.get_next (file);
      }
   }
}


void
dmz::QtModuleMapBasic::set_map_adapter (qmapcontrol::MapAdapter *adapter) {

   if (adapter != _mapAdapter) {
      
      if (!adapter) { adapter = _defaultAdapter; }

      if (adapter && _map) {

		  int zoom = _map->currentZoom ();
         _mapAdapter = adapter;

         if (_baseLayer) { _baseLayer->setMapAdapter (_mapAdapter); }
         if (_geomLayer) { _geomLayer->setMapAdapter (_mapAdapter); }
         
         qmapcontrol::ImageManager::instance ()->abortLoading ();
         _map->updateRequestNew ();
         _map->setZoom (zoom);
      }
   }
}


dmz::Boolean
dmz::QtModuleMapBasic::add_item (const Handle ObjectHandle, qmapcontrol::Geometry *item) {
   
   Boolean retVal (False);

   if (item && _geomLayer) {

      retVal = _itemTable.store (ObjectHandle, item);

      if (retVal) {

         _geomLayer->addGeometry (item);
      }
   }

   return retVal;
}


qmapcontrol::Geometry *
dmz::QtModuleMapBasic::lookup_item (const Handle ObjectHandle) {

   qmapcontrol::Geometry *item (_itemTable.lookup (ObjectHandle));
   return item;
}


qmapcontrol::Geometry *
dmz::QtModuleMapBasic::remove_item (const Handle ObjectHandle) {

   qmapcontrol::Geometry *item (_itemTable.remove (ObjectHandle));

   if (item && _geomLayer) {

      _geomLayer->removeGeometry (item);
   }

   return item;
}


void
dmz::QtModuleMapBasic::set_zoom_min_value (const Int32 Value) {

   _zoomMin = Value;
}


dmz::Int32
dmz::QtModuleMapBasic::get_zoom_min_value () const {

   return _zoomMin;
}


void
dmz::QtModuleMapBasic::set_zoom_max_value (const Int32 Value) {

   _zoomMax = Value;
}


dmz::Int32
dmz::QtModuleMapBasic::get_zoom_max_value () const {

   return _zoomMax;
}


void
dmz::QtModuleMapBasic::set_zoom (const Int32 Value) {

   if (_map) {

      Int32 zoom (Value);
      if (Value > _zoomMax) { zoom = _zoomMax; }
      else if (Value < _zoomMin) { zoom = _zoomMin; }

      _map->setZoom (zoom);
   }
}


dmz::Int32
dmz::QtModuleMapBasic::get_zoom () const {

   Int32 retVal (1.0f);

   if (_map) { retVal = _map->currentZoom (); }

   return retVal;
}


void
dmz::QtModuleMapBasic::zoom_in () {

   if (_map) { _map->zoomIn (); }
}


void
dmz::QtModuleMapBasic::zoom_out () {

   if (_map) { _map->zoomOut (); }
}


void
dmz::QtModuleMapBasic::pan_direction (const Int32 Dx, const Int32 Dy) {
   
   if (_map) {
      
      _map->scroll (QPoint (Dx, Dy));
   }
}


QPoint
dmz::QtModuleMapBasic::world_to_screen (const QPointF &Coordinate) const {

   QPoint result;
   
   if (_map) {
      
      result = _map->worldCoordinateToScreen (Coordinate);
   }
   
   return result;
}


QPointF
dmz::QtModuleMapBasic::screen_to_world (const QPoint &Point) const {

   QPointF result;
   
   if (_map) {
      
      result = _map->screenToWorldCoordinate (Point);
   }
   
   return result;
}



// Class Methods
void
dmz::QtModuleMapBasic::resizeEvent (QResizeEvent *event) {

   if (event) {
      
      if (_ignoreEvents) {

         event->ignore ();
      }
      else {

         if (_map && event) {

            _map->resize (event->size ());
         }

         _handle_mouse_event (0, 0);
      }
   }
}


void
dmz::QtModuleMapBasic::keyPressEvent (QKeyEvent *event) {

   if (event) {
      
      if (_ignoreEvents) { event->ignore (); }
      else { _handle_key_event (*event, True); }
   }
}


void
dmz::QtModuleMapBasic::keyReleaseEvent (QKeyEvent *event) {

   if (event) {
      
      if (_ignoreEvents) { event->ignore (); }
      else { _handle_key_event (*event, False); }
   }
}


void
dmz::QtModuleMapBasic::mousePressEvent (QMouseEvent *event) {

   if (event) {
      
      if (_ignoreEvents) { event->ignore (); }
      else { _handle_mouse_event (event, 0); }
   }
}


void
dmz::QtModuleMapBasic::mouseReleaseEvent (QMouseEvent *event) {

   if (event) {
      
      if (_ignoreEvents) { event->ignore (); }
      else { _handle_mouse_event (event, 0); }
   }
}


void
dmz::QtModuleMapBasic::mouseMoveEvent (QMouseEvent *event) {

   if (event) {
      
      if (_ignoreEvents) { event->ignore (); }
      else { _handle_mouse_event (event, 0); }
   }
}


void
dmz::QtModuleMapBasic::wheelEvent (QWheelEvent *event) {

   if (event) {
      
      if (_ignoreEvents) { event->ignore (); }
      else { _handle_mouse_event (0, event); }
   }
}


void
dmz::QtModuleMapBasic::_handle_key_event (
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
dmz::QtModuleMapBasic::_handle_mouse_event (QMouseEvent *me, QWheelEvent *we) {

   if (_inputModule && _map) {

      InputEventMouse event (_mouseEvent);

      QPoint pointOnCanvas (event.get_mouse_x (), event.get_mouse_y ());
      QPoint pointOnScreen (event.get_mouse_screen_x (), event.get_mouse_screen_y ());

      Qt::MouseButtons buttons;

      if (me) {

         pointOnCanvas = _map->mapFrom (this, me->pos ());
         pointOnScreen = me->globalPos ();

         buttons = me->buttons ();
      }
      else if (we) {

         pointOnCanvas = _map->mapFrom (this, we->pos ());
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
dmz::QtModuleMapBasic::_mouse_event_coordinate (
      const QMouseEvent *Event,
      const QPointF Coordinate) {

   QMouseEvent *event ((QMouseEvent *)Event);
   _handle_mouse_event (event, 0);
}


void
dmz::QtModuleMapBasic::_save_session () {

}


void
dmz::QtModuleMapBasic::_load_session () {

}


void
dmz::QtModuleMapBasic::_init (Config &local) {

   setObjectName (get_plugin_name ().get_buffer ());

   _pickModuleName = config_to_string ("module.pick.name", local);

   qwidget_config_read ("widget", local, this);

   _map = new qmapcontrol::MapControl (frameSize (), qmapcontrol::MapControl::None);

   if (_map) {
      
      connect (
         _map, SIGNAL (mouseEventCoordinate (const QMouseEvent *, const QPointF)),
         this, SLOT (_mouse_event_coordinate (const QMouseEvent *, const QPointF)));
      
      _map->setMouseTracking (true);
   
      _cacheDir = get_home_directory ();
      
      if (is_valid_path (_cacheDir)) {
         
#if defined (_WIN32)
         _cacheDir << "/Local Settings/Application Data/";
#elif defined (__APPLE__) || defined (MACOSX)
         _cacheDir << "/Library/Caches/";
#else
         _cacheDir << "/.";
#endif
   
         _cacheDir << "dmz/QMapControl";
         _cacheDir = format_path (_cacheDir);
      }
      else { _cacheDir = ""; }
      
      if (config_to_boolean ("map.cache", local, True) && _cacheDir) {
   
         _log.info << "Persistent cache: " << _cacheDir << endl;
         _map->enablePersistentCache (QString (_cacheDir.get_buffer ()));
      }
      
      _map->showScale (config_to_boolean ("map.scale", local, True));
      _map->showLoading (config_to_boolean ("map.loading", local, True));
   
      _zoomMin = config_to_int32 ("zoom.min", local, _zoomMin);
      _zoomMax = config_to_int32 ("zoom.max", local, _zoomMax);
      _zoomDefault = config_to_int32 ("zoom.default", local, _zoomDefault);
      
      String mapUrl (config_to_string ("tileMapAdapter.url", local, "tile.openstreetmap.org"));
      String mapPath (config_to_string ("tileMapAdapter.path", local, "/%1/%2/%3.png"));
      Int32 tileSize (config_to_int32 ("tileMapAdapter.tileSize", local, 256));
      
       _defaultAdapter = new qmapcontrol::TileMapAdapter (
         mapUrl.get_buffer (),
         mapPath.get_buffer (),
         tileSize,
         _zoomMin,
         _zoomMax);
   
      _baseLayer = new qmapcontrol::MapLayer ("base", _defaultAdapter);
      _map->addLayer (_baseLayer);
   
      _geomLayer = new qmapcontrol::GeometryLayer ("geom", _defaultAdapter);
      _map->addLayer (_geomLayer);
   
      _mapAdapter = _defaultAdapter;
         
      QVBoxLayout *layout (new QVBoxLayout ());
      layout->addWidget (_map);
      layout->setMargin (0);
      
      setLayout (layout);
      setMouseTracking (true);
      
      _inputModuleName = config_to_string ("module.input.name", local);
   
      _keyEvent.set_source_handle (get_plugin_handle ());
      _mouseEvent.set_source_handle (get_plugin_handle ());
      
      set_zoom_min_value (_zoomMin);
      set_zoom_max_value (_zoomMax);
      set_zoom (_zoomDefault);
      
      Float64 latitude (config_to_float64 ("startCoordinate.latitude", local, 0.0));
      Float64 longitude (config_to_float64 ("startCoordinate.longitude", local, 0.0));
      
      _map->setView (QPointF (longitude, latitude));
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtModuleMapBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtModuleMapBasic (Info, local);
}

};
