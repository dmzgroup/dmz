#ifndef DMZ_QT_PLUGIN_GRAPH_DOT_H
#define DMZ_QT_PLUGIN_GRAPH_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzQtWidget.h>
#include <dmzRuntimeDataConverterTypesBase.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHashTableUInt32Template.h>

#include <QtGui/QFrame>
#include <QtGui/QBrush>
#include <QtGui/QGraphicsLineItem>
#include <QtGui/QGraphicsPathItem>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsTextItem>
#include <QtGui/QGraphicsView>
#include <QtGui/QPen>
#include <ui_GraphForm.h>


namespace dmz {
   
   class QtModuleMainWindow;


   class QtPluginGraph :
         public QFrame,
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil,
         public MessageObserver,
         public QtWidget {
            
      Q_OBJECT

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

         virtual void update_object_text (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const String &Value,
            const String *PreviousValue);

         // Message Observer Interface
         virtual void receive_message (
            const Message &Type,
            const Handle MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

         // QtWidget Interface
         virtual QWidget *get_qt_widget ();

      protected slots:
         void on_exportButton_clicked ();
      
      protected:
         struct BarStruct {

            const Int32 Id;
            HashTableHandleTemplate<String> labels;
            Int32 count;
            Float32 offset;
            Float32 height;
            QGraphicsRectItem *bar;
            QGraphicsTextItem *text;
            QGraphicsTextItem *countText;

            BarStruct (const Int32 TheId) :
               Id (TheId),
               count (0),
               offset (0.0),
               height (0.0),
               bar (0),
               text (0),
               countText (0) {;}

            ~BarStruct () { labels.empty (); }

            void update_label () {

               HashTableHandleIterator it;
               String *ptr = labels.get_last (it);
               if (ptr && *ptr && text) { text->setPlainText (ptr->get_buffer ()); }
            }

            String *remove_label (const Handle Object) {

               String *result = labels.remove (Object);
               update_label ();
               return result;
            }
         };

         struct ObjectStruct {

            const Handle Object;
            Int32 count;
            BarStruct *bar;

            ObjectStruct (const Handle TheObject) :
                  Object (TheObject),
                  count (0),
                  bar (0) {;}
         };

         void showEvent (QShowEvent *event);
         void hideEvent (QHideEvent *event);

         QPixmap _screen_grab ();
         void _update_object_count (const Int32 Value, ObjectStruct &obj);
         BarStruct *_lookup_bar (const Int32 Count);
         void _remove_bar (BarStruct &bar);
         void _update_bar (BarStruct &bar);
         void _update_power_law (const BarStruct *LastBar, const Float64 EndOfXAxis);
         void _update_graph ();
         void _init (Config &local);

         Log _log;
         DataConverterBoolean _convertBool;
         DataConverterString _convertString;
         Ui::GraphForm _ui;
         QtModuleMainWindow *_mainWindowModule;
         String _mainWindowModuleName;
         QGraphicsScene *_scene;
         QGraphicsLineItem *_xAxis;
         QGraphicsLineItem *_yAxis;
         QGraphicsTextItem **_yLabels;
         QGraphicsPathItem *_powerLawPath;
         QGraphicsTextItem *_powerLabel;
         QGraphicsTextItem *_xAxisLabel;
         QGraphicsTextItem *_yAxisLabel;

         QBrush _barFill;
         QPen _barStroke;
         QPen _powerStroke;

         Boolean _graphDirty;
         Boolean _showPowerLaw;
         Boolean _showPercents;
         Boolean _rotateLabel;

         Handle _countHandle;
         Handle _labelHandle;

         Int32 _maxCount;
         Int32 _totalCount;
         Int32 _maxBarCount;

         Message _visibleMsg;
         Message _updateXLabelMsg;
         Message _updateYLabelMsg;

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
