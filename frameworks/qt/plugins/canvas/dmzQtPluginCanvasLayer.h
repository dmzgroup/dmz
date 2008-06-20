#ifndef DMZ_QT_PLUGIN_CANVAS_LAYER_DOT_H
#define DMZ_QT_PLUGIN_CANVAS_LAYER_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzRuntimeUndo.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <QtGui/QGraphicsItem>
#include <QtGui/QStandardItemModel>
#include <QtGui/QWidget>
#include "ui_dmzQtPluginCanvasLayer.h"

class QDockWidget;


namespace dmz {

   class QtModuleCanvas;
   class QtModuleMainWindow;

   class QtCanvasLayer : public QGraphicsItem {

      public:
         QtCanvasLayer (QGraphicsItem *parent = 0);
         ~QtCanvasLayer ();

         virtual QRectF boundingRect () const;

         virtual void paint (
            QPainter *painter,
            const QStyleOptionGraphicsItem *option,
            QWidget *widget);

         virtual void setVisible (bool visible);
   };


   class QtPluginCanvasLayerModel :
         public QAbstractItemModel,
         public ObjectObserverUtil,
         public TimeSlice {

      Q_OBJECT

      public:
         QtPluginCanvasLayerModel (
            const PluginInfo &Info,
            const Config &Local,
            QObject *parent);

         ~QtPluginCanvasLayerModel ();

         void store_canvas_module (QtModuleCanvas &module);
         void remove_canvas_module (QtModuleCanvas &module);

         QModelIndex index_for_handle (const Handle ObjectHandle);

         void set_active_layer (const Handle Layer);

         Handle create_layer (
            const String &Name,
            const Boolean Visible,
            const Boolean Active,
            const Boolean Locked,
            const Boolean Undo);

         Boolean delete_active_layer ();

         // QAbstractItemModel Interface
         virtual QModelIndex index (
            int row,
            int column,
            const QModelIndex &Parent) const;

         virtual QVariant headerData (
            int section,
            Qt::Orientation orientation,
            int role) const;

         virtual bool setData (
            const QModelIndex &Index,
            const QVariant &Value,
            int role = Qt::EditRole);

         virtual bool dropMimeData (
            const QMimeData *Data,
            Qt::DropAction action,
            int row,
            int column,
            const QModelIndex &Parent);

         virtual QStringList mimeTypes () const;
         virtual QMimeData *mimeData (const QModelIndexList &Indexes) const;
         virtual QModelIndex parent (const QModelIndex &Child) const;
         virtual int rowCount (const QModelIndex &Parent) const;
         virtual int columnCount (const QModelIndex &Parent) const;
         virtual QVariant data (const QModelIndex &Index, int role) const;
         Qt::ItemFlags flags (const QModelIndex &Index) const;
         Qt::DropActions supportedDropActions () const;
         //QStringList mimeTypes () const;

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

         virtual void update_object_type (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const ObjectType &Value,
            const ObjectType *PreviousValue);

         virtual void update_object_state (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &Value,
            const Mask *PreviousValue);

         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

         virtual void update_object_text (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const String &Value,
            const String *PreviousValue);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

      protected:
         virtual void _store_object_module (ObjectModule &objMod);
         virtual void _remove_object_module (ObjectModule &objMod);
         void _set_layer_active (const Handle ObjectHandle, const Boolean Value);
         void _set_layer_locked (const Handle ObjectHandle, const Boolean Value);
         void _set_layer_visible (const Handle ObjectHandle, const Boolean Value);
         void _update_layer_list ();
         QGraphicsItem *_get_item (const Handle ObjectHandle) const;
         Int32 _get_layer_count () const;
         Handle _get_layer_handle (const Int32 Row) const;
         Int32 _get_layer_row (const Handle ObjectHandle) const;
         Handle _get_previous_layer_handle (const Handle ObjectHandle) const;
         Handle _get_next_layer_handle (const Handle ObjectHandle) const;
         Handle _get_last_layer_handle (const Handle ObjectHandle) const;
         QVariant _get_display_data (const QModelIndex &Index) const;
         QVariant _get_check_state_data (const QModelIndex &Index) const;
         Boolean _remove_layer (const Handle SourceHandle);
         void _move_layer (const Handle SourceHandle, const Handle TargetHandle);
         void _model_changed (const Handle FirstObject, const Handle LastObject);
         void _update_layer_order ();

         Log _log;
         Undo _undo;
         Definitions _defs;
         ObjectModule *_objectModule;
         QtModuleCanvas *_canvasModule;
         Handle _defaultAttrHandle;
         Handle _activeAttrHandle;
         Handle _lockedAttrHandle;
         Handle _nameAttrHandle;
         Handle _linkAttrHandle;
         Handle _orderAttrHandle;
         Handle _visibleAttrHandle;
         Handle _activeLayerHandle;
         Handle _rootLayerHandle;
         HashTableHandleTemplate<QtCanvasLayer> _layerItemTable;
         QList<Handle> _layerHandleList;
         Boolean _layersUpdated;
         Int32 _layerCount;

      private:
         QtPluginCanvasLayerModel ();
         QtPluginCanvasLayerModel (const QtPluginCanvasLayerModel &);
         QtPluginCanvasLayerModel &operator= (const QtPluginCanvasLayerModel &);
   };


   class QtPluginCanvasLayer : public QWidget, public Plugin, public TimeSlice {

      Q_OBJECT

      public:
         QtPluginCanvasLayer (const PluginInfo &Info, const Config &Local);
         ~QtPluginCanvasLayer ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

      protected slots:
         void on_addButton_clicked ();
         void on_deleteButton_clicked ();

         void _slot_current_changed (
            const QModelIndex &Current,
            const QModelIndex &Previous);

      protected:
         void _save_session ();
         void _load_session ();
         void _init (const Config &Local);

         Log _log;
         Definitions _defs;
         Ui::layersForm _ui;
         ObjectModule *_objectModule;
         String _objectModuleName;
         QtModuleCanvas *_canvasModule;
         String _canvasModuleName;
         QtModuleMainWindow *_mainWindowModule;
         String _mainWindowModuleName;
         Handle _channel;
         QString _title;
         QDockWidget *_dock;
         QtPluginCanvasLayerModel _layerModel;
         Int32 _newLayerCount;

      private:
         QtPluginCanvasLayer ();
         QtPluginCanvasLayer (const QtPluginCanvasLayer &);
         QtPluginCanvasLayer &operator= (const QtPluginCanvasLayer &);

   };
};

#endif // DMZ_QT_PLUGIN_CANVAS_LAYER_DOT_H
