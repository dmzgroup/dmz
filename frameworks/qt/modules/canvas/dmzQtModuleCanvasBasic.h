#ifndef DMZ_QT_MODULE_CANVAS_BASIC_DOT_H
#define DMZ_QT_MODULE_CANVAS_BASIC_DOT_H

#include <dmzInputEventKey.h>
#include <dmzInputEventMouse.h>
#include "dmzQtCanvasScene.h"
#include <dmzQtModuleCanvas.h>
#include <dmzQtWidget.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <QtGui/QWidget>


namespace dmz {

   class InputModule;
   class QtCanvasView;

   class QtModuleCanvasBasic :
         public QWidget,
         public Plugin,
         public QtModuleCanvas,
         public QtWidget {

      Q_OBJECT

      public:
         QtModuleCanvasBasic (const PluginInfo &Info, Config &local);
         ~QtModuleCanvasBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // QtModuleCanvas Interface
         virtual QGraphicsScene *get_scene ();
         virtual QGraphicsView *get_view ();

         virtual Boolean add_item (const Handle ObjectHandle, QGraphicsItem *item);
         virtual QGraphicsItem *lookup_item (const Handle ObjectHandle);
         virtual QGraphicsItem *remove_item (const Handle ObjectHandle);

         virtual void set_zoom_min_value (const Float32 Value);
         virtual Float32 get_zoom_min_value () const;

         virtual void set_zoom_max_value (const Float32 Value);
         virtual Float32 get_zoom_max_value () const;

         virtual void set_zoom_step_value (const Float32 Value);
         virtual Float32 get_zoom_step_value () const;

         virtual void set_zoom (const Float32 Value);
         virtual Float32 get_zoom () const;

         virtual void zoom_in ();
         virtual void zoom_out ();
         virtual void zoom_extents ();

         virtual void pan_direction (const Int32 Dx, const Int32 Dy);

         virtual void center_on (const Handle ObjectHandle);
//         virtual void center_on (const QPointF &Value);
         
         virtual QPointF get_center () const;
         
         // QtWidget Interface
         virtual QWidget *get_qt_widget ();

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

         void _init (Config &local);

         Log _log;
         InputModule *_inputModule;
         String _inputModuleName;
         QtCanvasScene _scene;
         QtCanvasView *_canvas;
         InputEventKey _keyEvent;
         InputEventMouse _mouseEvent;
         HashTableHandleTemplate<QGraphicsItem> _itemTable;
         Float32 _zoomMin;
         Float32 _zoomMax;
         Float32 _zoomStep;
         Float32 _zoomDefault;

      private:
         QtModuleCanvasBasic ();
         QtModuleCanvasBasic (const QtModuleCanvasBasic &);
         QtModuleCanvasBasic &operator= (const QtModuleCanvasBasic &);
   };
};


#endif // DMZ_QT_MODULE_CANVAS_BASIC_DOT_H
