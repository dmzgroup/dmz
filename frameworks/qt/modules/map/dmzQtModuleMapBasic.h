#ifndef DMZ_QT_MODULE_MAP_BASIC_DOT_H
#define DMZ_QT_MODULE_MAP_BASIC_DOT_H

#include <dmzInputEventKey.h>
#include <dmzInputEventMouse.h>
#include <dmzQtModuleMap.h>
#include <dmzQtWidget.h>
#include <dmzRenderModulePickConvert.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <QtGui/QFrame>


namespace qmapcontrol {

   class MapAdapter;
   class MapLayer;
   class GeometryLayer;
   class Geometry;
};


namespace dmz {

   class InputModule;
   class RenderModulePick;

   class QtModuleMapBasic :
         public QFrame,
         public Plugin,
         public RenderModulePickConvert,
         public QtWidget,
         public QtModuleMap {

   Q_OBJECT

      public:
         QtModuleMapBasic (const PluginInfo &Info, Config &local);
         ~QtModuleMapBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // RenderModulePickConvert Interface
         virtual Boolean source_to_world (
            const Int32 ScreenPosX,
            const Int32 ScreenPosY,
            Vector &worldPosition,
            Vector &normal,
            Handle &objectHandle);

         virtual Boolean world_to_source (
            const Vector &WorldPosition,
            Int32 &screenPosX,
            Int32 &screenPosY);

         // QtWidget Interface
         virtual QWidget *get_qt_widget ();

         // QtModuleMap Interface
         virtual qmapcontrol::MapControl *get_map_control ();

         virtual void use_default_map_adapter ();
         virtual void set_map_adapter (qmapcontrol::MapAdapter *adapter);

         virtual String get_tile_cache_dir () const;
         virtual void empty_tile_cache ();

         virtual Boolean add_item (
            const Handle ObjectHandle,
            qmapcontrol::Geometry *item);

         virtual qmapcontrol::Geometry *lookup_item (const Handle ObjectHandle);
         virtual qmapcontrol::Geometry *remove_item (const Handle ObjectHandle);

         virtual void set_zoom_min_value (const Int32 Value);
         virtual Int32 get_zoom_min_value () const;

         virtual void set_zoom_max_value (const Int32 Value);
         virtual Int32 get_zoom_max_value () const;

         virtual void set_zoom (const Int32 Value);
         virtual Int32 get_zoom () const;

         virtual void zoom_in ();
         virtual void zoom_out ();

         virtual void pan_direction (const Int32 Dx, const Int32 Dy);

         virtual void center_on (const Float64 Dx, const Float64 Dy);

         virtual QPoint world_to_screen (const QPointF &World) const;
         virtual QPointF screen_to_world (const QPoint &Screen) const;

      protected slots:
         void _mouse_event_coordinate (
            const QMouseEvent *Event,
            const QPointF Coordinate);

      protected:
         virtual void resizeEvent (QResizeEvent* event);
         virtual void keyPressEvent (QKeyEvent *event);
         virtual void keyReleaseEvent (QKeyEvent* event);
         virtual void mousePressEvent (QMouseEvent *event);
         virtual void mouseReleaseEvent (QMouseEvent* event);
         virtual void mouseMoveEvent (QMouseEvent *event);
         virtual void wheelEvent (QWheelEvent* event);

         void _handle_key_event (
            const QKeyEvent &Event,
            const Boolean KeyState);

         void _handle_mouse_event (QMouseEvent *me, QWheelEvent *we);

         void _save_session ();
         void _load_session ();

         void _init (Config &local);

         Log _log;
         RenderModulePick *_pickModule;
         String _pickModuleName;
         InputModule *_inputModule;
         String _inputModuleName;
         InputEventKey _keyEvent;
         InputEventMouse _mouseEvent;
         Boolean _ignoreEvents;
         HashTableHandleTemplate<qmapcontrol::Geometry> _itemTable;
         qmapcontrol::MapControl *_map;
         qmapcontrol::MapAdapter *_defaultAdapter;
         qmapcontrol::MapAdapter *_mapAdapter;
         qmapcontrol::MapLayer *_baseLayer;
         qmapcontrol::GeometryLayer *_geomLayer;
         Int32 _zoomMin;
         Int32 _zoomMax;
         Int32 _zoomDefault;
         String _cacheDir;

      private:
         QtModuleMapBasic ();
         QtModuleMapBasic (const QtModuleMapBasic &);
         QtModuleMapBasic &operator= (const QtModuleMapBasic &);
   };
};


#endif // DMZ_QT_MODULE_MAP_BASIC_DOT_H
