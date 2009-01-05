#ifndef DMZ_QT_PLUGIN_GRAPH_DOT_H
#define DMZ_QT_PLUGIN_GRAPH_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzQtWidget.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHashTableUInt32Template.h>

#include <QtGui/QBrush>
#include <QtGui/QGraphicsLineItem>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsTextItem>
#include <QtGui/QGraphicsView>
#include <QtGui/QPen>

namespace dmz {

   class QtPluginGraph :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil,
         public QtWidget {

      public:
         QtPluginGraph (const PluginInfo &Info, Config &local);
         ~QtPluginGraph ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void link_objects (
            const Handle LinkHandle,
            const Handle AttributeHandle,
            const UUID &SuperIdentity,
            const Handle SuperHandle,
            const UUID &SubIdentity,
            const Handle SubHandle);

         virtual void unlink_objects (
            const Handle LinkHandle,
            const Handle AttributeHandle,
            const UUID &SuperIdentity,
            const Handle SuperHandle,
            const UUID &SubIdentity,
            const Handle SubHandle);

         virtual void update_object_counter (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Int64 Value,
            const Int64 *PreviousValue);

         // QtWidget Interface
         virtual QWidget *get_qt_widget ();

      protected:
         struct BarStruct {

            const Int32 Id;
            Int32 count;
            Float32 offset;
            QGraphicsRectItem *bar;
            QGraphicsTextItem *text;
            QGraphicsTextItem *countText;

            BarStruct (const Int32 TheId) :
               Id (TheId),
               count (0),
               offset (0.0),
               bar (0),
               text (0),
               countText (0) {;}
         };

         struct ObjectStruct {

            Int32 count;
            BarStruct *bar;

            ObjectStruct () : count (0), bar (0) {;}
         };

         void _update_object_count (const Int32 Value, ObjectStruct &obj);
         BarStruct *_lookup_bar (const Int32 Count);
         void _remove_bar (BarStruct &bar);
         void _update_bar (BarStruct &bar);
         void _update_graph ();
         void _init (Config &local);

         Log _log;
         QGraphicsScene *_scene;
         QGraphicsView *_view;
         QGraphicsLineItem *_xAxis;
         QGraphicsLineItem *_yAxis;
         QGraphicsTextItem **_yLabels;

         QBrush _barFill;
         QPen _barStroke;

         Boolean _graphDirty;

         Int32 _maxCount;
         Int32 _totalCount;
         Int32 _maxBarCount;

         Boolean _ascendingOrder;
         Float32 _barWidth;
         Float32 _barHeight;
         Float32 _spaceWidth;
         Int32 _yDivisions;
         Int32 _steps;

         HashTableUInt32Template<BarStruct> _barTable;
         HashTableUInt32Template<ObjectStruct> _objTable;
         ObjectTypeSet _typeSet;

      private:
         QtPluginGraph ();
         QtPluginGraph (const QtPluginGraph &);
         QtPluginGraph &operator= (const QtPluginGraph &);

   };
};

#endif // DMZ_QT_PLUGIN_GRAPH_DOT_H
