#ifndef DMZ_QT_MODULE_MAP_BASIC_DOT_H
#define DMZ_QT_MODULE_MAP_BASIC_DOT_H

#include <dmzInputEventKey.h>
#include <dmzInputEventMouse.h>
#include <dmzQtModuleMap.h>
#include <dmzQtWidget.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <QtGui/QWidget>


namespace qmapcontrol {
   
   class MapControl;
   class MapAdapter;
   class Layer;
};


namespace dmz {

   class InputModule;

   class QtModuleMapBasic :
         public QWidget,
         public Plugin,
         public QtWidget,
         public QtModuleMap {

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

         // QtWidget Interface
         virtual QWidget *get_qt_widget ();

         // QtModuleMap Interface
         
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
         InputModule *_inputModule;
         String _inputModuleName;
         InputEventKey _keyEvent;
         InputEventMouse _mouseEvent;
//         HashTableHandleTemplate<QGraphicsItem> _itemTable;
         // Float32 _zoomMin;
         // Float32 _zoomMax;
         // Float32 _zoomStep;
         // Float32 _zoomDefault;
         qmapcontrol::MapControl *_map;
         qmapcontrol::MapAdapter *_mapAdapter;
         qmapcontrol::Layer *_baseLayer;
         

      private:
         QtModuleMapBasic ();
         QtModuleMapBasic (const QtModuleMapBasic &);
         QtModuleMapBasic &operator= (const QtModuleMapBasic &);
   };
};


#endif // DMZ_QT_MODULE_MAP_BASIC_DOT_H
