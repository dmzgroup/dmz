#ifndef DMZ_QT_PLUGIN_CANVAS_MAP_DOT_H
#define DMZ_QT_PLUGIN_CANVAS_MAP_DOT_H


#include <dmzQtWidget.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <QtGui/QWidget>



namespace dmz {

   class QtModuleCanvas;
   class QtModuleMap;


   class QtPluginCanvasMap :
         public QWidget,
         public Plugin,
         public TimeSlice,
         public QtWidget {
            
   Q_OBJECT

      public:
         QtPluginCanvasMap (const PluginInfo &Info, Config &local);
         ~QtPluginCanvasMap ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);
         
         // QtWidget Interface
         virtual QWidget *get_qt_widget ();
         
      protected slots:
         // void _slot_pan_changed (const QPoint &);
         // void _slot_scale_changed (qreal);

      protected:
//         virtual bool eventFilter (QObject *obj, QEvent *event);
         
//         virtual void mousePressEvent (QMouseEvent *event);
         
         virtual void resizeEvent (QResizeEvent* event);
         void _init (Config &local);

         Log _log;
         QLayout *_layout;
         QtModuleMap *_mapModule;
         String _mapModuleName;
         QtModuleCanvas *_canvasModule;
         String _canvasModuleName;
         String _mapZoomName;
         QWidget *_mapWidget;
         QWidget *_canvasWidget;
         
      private:
         QtPluginCanvasMap ();
         QtPluginCanvasMap (const QtPluginCanvasMap &);
         QtPluginCanvasMap &operator= (const QtPluginCanvasMap &);

   };
};

#endif // DMZ_QT_PLUGIN_CANVAS_MAP_DOT_H
