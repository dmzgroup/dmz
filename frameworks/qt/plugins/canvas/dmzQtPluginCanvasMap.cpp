#include <dmzQtMapUtil.h>
#include <dmzQtModuleCanvas.h>
#include "dmzQtPluginCanvasMap.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <qmapcontrol.h>
#include <QtGui/QtGui>

#include <QtCore/QDebug>

//static QList<dmz::Float64> localResolutions;

static const dmz::Int32 BaseZoom (15);

   
dmz::Float64
local_level_to_scale (const dmz::Int32 Value) {

   return (1.0 / pow (2, BaseZoom - Value));
}


dmz::Int32
local_scale_to_level (const dmz::Float64 Value) {

   return (BaseZoom - log2 (1.0 / Value));
}


dmz::QtPluginCanvasMap::QtPluginCanvasMap (const PluginInfo &Info, Config &local) :
      QWidget (0),
      Plugin (Info),
      TimeSlice (Info),
      QtWidget (Info),
      _log (Info),
      _layout (0),
      _canvasModule (0),
      _canvasModuleName (),
      _map (0),
      _mapAdapter (0),
      _baseLayer (0),
      _canvasWidget (0) {

   _init (local);
}


dmz::QtPluginCanvasMap::~QtPluginCanvasMap () {

}


// Plugin Interface
void
dmz::QtPluginCanvasMap::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::QtPluginCanvasMap::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_canvasModule) {

         _canvasModule = QtModuleCanvas::cast (PluginPtr, _canvasModuleName);

         if (_canvasModule) {

            QGraphicsView *view = _canvasModule->get_view ();

            if (view) {

              // connect (
              //    view, SIGNAL (pan_changed (const QPoint &)),
              //    this, SLOT (_slot_pan_changed (const QPoint &)));

              connect (
                 view, SIGNAL (scale_changed (qreal)),
                 this, SLOT (_slot_scale_changed (qreal)));
            }
            
            QtWidget *w = QtWidget::cast (PluginPtr);
            
            if (w) {
            
               _canvasWidget = w->get_qt_widget ();
            
               if (_canvasWidget) {
               
                  _canvasWidget->setParent (this);
               }
            }
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_canvasModule && (_canvasModule == QtModuleCanvas::cast (PluginPtr))) {

         _canvasWidget = 0;
         _canvasModule = 0;
      }
   }
}

static QPoint lastPoint;
QGraphicsPixmapItem *gItem = 0; 

// TimeSlice Interface
void
dmz::QtPluginCanvasMap::update_time_slice (const Float64 TimeDelta) {

   if (_canvasModule) {
      
//      QPointF center (_canvasModule->get_center ());
      
         QGraphicsView *view = _canvasModule->get_view ();

         if (view) {

            QPoint sourcePoint = view->mapFromScene (QPointF (0.0, 0.0));
            sourcePoint += QPoint (1, 2);
            
            if (lastPoint != sourcePoint) {

               lastPoint = sourcePoint;
            
               QPointF worldPos = _map->screenToWorldCoordinate (sourcePoint);

//qDebug () << sourcePoint;
//qDebug () << worldPos;

//               _map->setView (-worldPos);

//<!-- <startCoordinate latitude="36.5973550921921" longitude="-121.876788139343"/> -->

//sourcePoint = _map->worldCoordinateToScreen (QPointF (-121.876788139343, 36.5973550921921));
sourcePoint = _map->worldCoordinateToScreen (QPointF (0, 0));
qDebug () << sourcePoint;

QPoint screenPoint (_map->mapToGlobal (sourcePoint));
qDebug () << screenPoint;

sourcePoint = view->mapFromGlobal (screenPoint);
qDebug () << sourcePoint;

worldPos = view->mapToScene (sourcePoint);
qDebug () << worldPos;

if (!gItem) {
   
   QGraphicsScene *scene = _canvasModule->get_scene ();

   gItem = scene->addPixmap (QPixmap ("images:NA_Node.svg"));
   gItem->setFlag (QGraphicsItem::ItemIgnoresTransformations, true);
   gItem->setPos (worldPos);
}

qDebug () << "-----------------------------------------------";
            }
         }
   }
}


// QtWidget Interface
QWidget *
dmz::QtPluginCanvasMap::get_qt_widget () { return this; }


void
dmz::QtPluginCanvasMap::_slot_pan_changed (const QPoint &Value) {

   if (_map) {

//      _map->scroll (-Value);
   }
}


void
dmz::QtPluginCanvasMap::_slot_scale_changed (qreal value) {

   if (_map) {
      
      _map->setZoom (local_scale_to_level (value));
   }
}

//static QPoint screenMiddle;

void
dmz::QtPluginCanvasMap::resizeEvent (QResizeEvent *event) {

   if (event) {
      
      if (_map) { _map->resize (event->size () - QSize (1, 1)); }
      
      if (_canvasWidget) { _canvasWidget->resize (event->size ()); }

//----------------------------------------------------------------------------------------
//       QPointF currCoord = _map->currentCoordinate ();
//       QPoint screenCoord = _map->worldCoordinateToScreen (currCoord);
//       qWarning () << "cc: " << currCoord;
//       qWarning () << "sc: " << screenCoord;
//       
//       screenMiddle = QPoint (event->size ().width () / 2, event->size ().height () / 2);
// qWarning () << "sm: " << screenMiddle;
//       
//       Coordinate coord (0, 0);
//       
//       Point centerPoint = latlong_to_pixelxy (coord, _map->currentZoom ());
//       
// _log.warn << "cp: " << centerPoint.x << " " << centerPoint.y << endl;
//----------------------------------------------------------------------------------------
      event->ignore ();
   }
}


void
dmz::QtPluginCanvasMap::_init (Config &local) {

   _canvasModuleName = config_to_string ("module.canvas.name", local);

   Int32 zoomMin = config_to_int32 ("zoom.min", local, 0);
   Int32 zoomMax = config_to_int32 ("zoom.max", local, 17);
   
   String mapUrl (config_to_string ("tileMapAdapter.url", local, "tile.openstreetmap.org"));
   String mapPath (config_to_string ("tileMapAdapter.path", local, "/%1/%2/%3.png"));
   Int32 tileSize (config_to_int32 ("tileMapAdapter.tileSize", local, 256));
   
   _map = new qmapcontrol::MapControl (frameSize (), qmapcontrol::MapControl::None, this);
   
   _map->showScale (config_to_boolean ("map.showScale", local, True));
   
   _mapAdapter = new qmapcontrol::TileMapAdapter (
      mapUrl.get_buffer (),
      mapPath.get_buffer (),
      tileSize,
      zoomMin,
      zoomMax);

   _baseLayer = new qmapcontrol::MapLayer ("base", _mapAdapter);
   _map->addLayer (_baseLayer);

   _layout = new QVBoxLayout (this);
   _layout->setSpacing (0);
   _layout->setContentsMargins (0, 0, 0, 0);
   _layout->addWidget (_map);
   
   setLayout (_layout);
   
   // if (config_to_boolean ("map.cache", local, False)) {
   //    
   //    _map->enablePersistentCache (const QDir& path = QDir::homePath () + "/QMapControl.cache");
   // }
_map->enablePersistentCache ();
   
   // Float64 latitude (config_to_float64 ("startCoordinate.latitude", local, 0.0));
   // Float64 longitude (config_to_float64 ("startCoordinate.longitude", local, 0.0));
   // 
   // _map->setView (QPointF (longitude, latitude));
   
   
qmapcontrol::GeometryLayer *geomLayer = new qmapcontrol::GeometryLayer ("geom", _mapAdapter);
_map->addLayer (geomLayer);

//qmapcontrol::Point *item = new qmapcontrol::ImagePoint (-121.876788139343, 36.5973550921921, "images:NA_Node.svg");
qmapcontrol::Point *item = new qmapcontrol::ImagePoint (0.0, 0.0, "images:NA_Node.svg");
geomLayer->addGeometry (item);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginCanvasMap (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginCanvasMap (Info, local);
}

};
