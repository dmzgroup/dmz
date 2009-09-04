#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzQtCanvasConsts.h>
#include <dmzQtConfigRead.h>
#include <dmzQtModuleCanvas.h>
#include "dmzQtPluginCanvasLayer.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <dmzTypesMask.h>
#include <dmzTypesUUID.h>
#include <QtGui/QtGui>


namespace dmz {

   const UInt32 LayerVisibleColumn (0);
   const UInt32 LayerLockedColumn (1);
   const UInt32 LayerNameColumn (2);
   const UInt32 LayerZValueColumn (3);
   const UInt32 LayerColumnCount (4);
};


dmz::QtCanvasLayer::QtCanvasLayer (QGraphicsItem *parent) :
      QGraphicsItem (parent) {;}


dmz::QtCanvasLayer::~QtCanvasLayer () {;}


QRectF
dmz::QtCanvasLayer::boundingRect () const {

   return QRectF (); //childrenBoundingRect ();
}


void
dmz::QtCanvasLayer::paint (
      QPainter *painter,
      const QStyleOptionGraphicsItem *option,
      QWidget *widget) {

   // QColor color (Qt::black);
   // color.setAlphaF (0.25);
   // painter->setBrush (color);
   // painter->drawRect (boundingRect ());
}


void
dmz::QtCanvasLayer::setVisible (bool visible) {

   setFlag (ItemClipsChildrenToShape, !visible);
}


dmz::QtPluginCanvasLayerModel::QtPluginCanvasLayerModel (
      const PluginInfo &Info,
      const Config &Local,
      QObject *parent) :
      QAbstractItemModel (parent),
      ObjectObserverUtil (Info, Local),
      TimeSlice (Info),
      _log (Info),
      _undo (Info),
      _defs (Info, &_log),
      _objectModule (0),
      _canvasModule (0),
      _defaultAttrHandle (0),
      _activeAttrHandle (0),
      _lockedAttrHandle (0),
      _nameAttrHandle (0),
      _linkAttrHandle (0),
      _orderAttrHandle (0),
      _visibleAttrHandle (0),
      _activeLayerHandle (0),
      _rootLayerHandle (0),
      _layerItemTable (),
      _layerHandleList (),
      _layersUpdated (False),
      _layerCount (0) {

   setObjectName ("QtPluginCanvasLayerModel");

   _defaultAttrHandle = activate_default_object_attribute (
      ObjectCreateMask |
      ObjectDestroyMask);

   _linkAttrHandle = _defs.lookup_named_handle (ObjectAttributeLayerLinkName);

   _orderAttrHandle = activate_object_attribute (
      ObjectAttributeLayerOrderName,
      ObjectLinkMask | ObjectUnlinkMask);

   _activeAttrHandle = activate_object_attribute (
      ObjectAttributeLayerActiveName,
      ObjectFlagMask);

   _lockedAttrHandle = activate_object_attribute (
      ObjectAttributeLayerLockedName,
      ObjectFlagMask);

   _visibleAttrHandle = activate_object_attribute (
      ObjectAttributeLayerVisibleName,
      ObjectFlagMask);

   _nameAttrHandle = activate_object_attribute (
      ObjectAttributeLayerName,
      ObjectTextMask);
}


dmz::QtPluginCanvasLayerModel::~QtPluginCanvasLayerModel () {

   _layerItemTable.clear ();
}


void
dmz::QtPluginCanvasLayerModel::store_canvas_module (QtModuleCanvas &module) {

   _canvasModule = &module;
}


void
dmz::QtPluginCanvasLayerModel::remove_canvas_module (QtModuleCanvas &module) {

   if (_canvasModule == &module) { _canvasModule = 0; }
}


void
dmz::QtPluginCanvasLayerModel::_store_object_module (ObjectModule &module) {

   _objectModule = &module;
}


void
dmz::QtPluginCanvasLayerModel::_remove_object_module (ObjectModule &module) {

   if (_objectModule == &module) { _canvasModule = 0; }
}


QModelIndex
dmz::QtPluginCanvasLayerModel::index_for_handle (const Handle ObjectHandle) {

   return index (_get_layer_row (ObjectHandle), LayerNameColumn, QModelIndex ());
}


void
dmz::QtPluginCanvasLayerModel::set_active_layer (const Handle Layer) {

   if (_objectModule) {

      //const Handle PrevActiveLayer (_activeLayerHandle);

      // const Handle UndoHandle (_undo.start_record ("Layer Active"));

      _objectModule->store_flag (Layer, _activeAttrHandle, True);

      //if (PrevActiveLayer) {

      //   _objectModule->store_flag (PrevActiveLayer, _activeAttrHandle, False);
      //}

      // _undo.stop_record (UndoHandle);
   }
}


dmz::Handle
dmz::QtPluginCanvasLayerModel::create_layer (
      const String &Name,
      const Boolean Visible,
      const Boolean Active,
      const Boolean Locked,
      const Boolean Undo) {

   Handle layerHandle (0);

   if (_objectModule) {

      Handle undoHandle (0);

      if (Undo) { undoHandle = _undo.start_record ("Create Layer"); }

      if (!_rootLayerHandle) {

         ObjectType rootLayerType;
         _defs.lookup_object_type ("root_layer", rootLayerType);

         layerHandle = _objectModule->create_object (rootLayerType, ObjectLocal);

         if (layerHandle) {

            if (_objectModule->activate_object (layerHandle)) {

               _log.debug << "Root Layer Created: " << layerHandle << endl;
            }
         }
      }

      ObjectType layerType;
      _defs.lookup_object_type ("layer", layerType);

      Handle nameAttrHandle (_defs.create_named_handle (ObjectAttributeLayerName));

      layerHandle = _objectModule->create_object (layerType, ObjectLocal);

      if (layerHandle) {

         _objectModule->store_text (layerHandle, _nameAttrHandle, Name);
         _objectModule->store_flag (layerHandle, _visibleAttrHandle, Visible);
         _objectModule->store_flag (layerHandle, _lockedAttrHandle, Locked);
         _objectModule->store_flag (layerHandle, _activeAttrHandle, Active);

         _objectModule->activate_object (layerHandle);

         Handle lastLayerHandle (_get_last_layer_handle (_rootLayerHandle));

         _objectModule->link_objects (_orderAttrHandle, lastLayerHandle, layerHandle);
      }

      if (undoHandle) { _undo.stop_record (undoHandle); }
   }

   return layerHandle;
}


dmz::Boolean
dmz::QtPluginCanvasLayerModel::delete_active_layer () {

   Boolean result (False);

   if (_objectModule && _activeLayerHandle) {

      const Handle UndoHandle (_undo.start_record ("Delete Layer"));

      const Handle Layer (_activeLayerHandle);
      const Handle PreviousLayer (_get_next_layer_handle (Layer));
      const Handle NextLayer (_get_previous_layer_handle (Layer));

      HandleContainer links;

      if (_objectModule->lookup_super_links (Layer, _linkAttrHandle, links)) {

         Handle objHandle = links.get_first ();

         while (objHandle) {

            _objectModule->destroy_object (objHandle);

            objHandle = links.get_next ();
         }
      }

      _objectModule->store_flag (Layer, _activeAttrHandle, False);

      _remove_layer (Layer);

      _objectModule->destroy_object (Layer);

      if (NextLayer == _rootLayerHandle) { set_active_layer (PreviousLayer); }
      else { set_active_layer (NextLayer); }

      _undo.stop_record (UndoHandle);

      result = True;
   }

   return result;
}


// QAbstractItemModel Interface
QModelIndex
dmz::QtPluginCanvasLayerModel::index (
      int row,
      int column,
      const QModelIndex &Parent) const {

   QModelIndex retVal;

   if (row < _get_layer_count ()) {

      retVal = createIndex (row, column, (quint32)_get_layer_handle (row));
   }

   return retVal;
}


QModelIndex
dmz::QtPluginCanvasLayerModel::parent (const QModelIndex &Child) const {

   return QModelIndex ();
}


int
dmz::QtPluginCanvasLayerModel::rowCount (const QModelIndex &Parent) const {

   return _get_layer_count ();
}


int
dmz::QtPluginCanvasLayerModel::columnCount (const QModelIndex &Parent) const {

   return LayerColumnCount;
}


QVariant
dmz::QtPluginCanvasLayerModel::data (const QModelIndex &Index, int role) const {

   QVariant retVal;

   if (Index.isValid ()) {

      if (Qt::DisplayRole == role) {

         retVal = _get_display_data (Index);

         if (LayerNameColumn == Index.column ()) {

            const Handle LayerHandle (_get_layer_handle (Index.row ()));

            if (_activeLayerHandle && (LayerHandle == _activeLayerHandle)) {

               retVal = retVal.toString () + " (Active)";
            }
         }
      }
      else if (Qt::CheckStateRole == role) {

         if ((LayerLockedColumn == Index.column ()) ||
               (LayerVisibleColumn == Index.column ())) {

            retVal = _get_check_state_data (Index);
         }
      }
      else if (Qt::EditRole == role) {

         if (LayerNameColumn == Index.column ()) {

            retVal = _get_display_data (Index);
         }
      }
      else if (Qt::ForegroundRole == role) {

         if (LayerNameColumn == Index.column ()) {

            const Handle LayerHandle (_get_layer_handle (Index.row ()));

            if (_activeLayerHandle && (LayerHandle == _activeLayerHandle)) {

               retVal = Qt::red;
            }
         }
      }
      // else if (Qt::BackgroundRole == role) {
      //
      //    if (LayerNameColumn == Index.column ()) {
      //
      //       const Handle LayerHandle (_get_layer_handle (Index.row ()));
      //
      //       if (_activeLayerHandle && (LayerHandle == _activeLayerHandle)) {
      //
      //          retVal = QApplication::palette ().dark ();
      //       }
      //    }
      // }
   }

   return retVal;
}


bool
dmz::QtPluginCanvasLayerModel::setData (
      const QModelIndex &Index,
      const QVariant &Value,
      int role) {

   Boolean retVal (False);

   if (_objectModule && Index.isValid ()) {

      Handle layerHandle (_get_layer_handle (Index.row ()));

      if (layerHandle) {

         Handle undoHandle (0);

         if (role == Qt::CheckStateRole) {

            Boolean flag (Value.toBool ());

            if (LayerLockedColumn == Index.column ()) {

               undoHandle = _undo.start_record (flag ? "Layer Locked" : "Layer Unlocked");

               retVal = _objectModule->store_flag (layerHandle, _lockedAttrHandle, flag);
            }
            else if (LayerVisibleColumn == Index.column ()) {

               undoHandle = _undo.start_record (flag ? "Layer Visible" : "Layer Hidden");

               retVal = _objectModule->store_flag (layerHandle, _visibleAttrHandle, flag);
            }
         }
         else if (role == Qt::EditRole) {

            if (LayerNameColumn == Index.column ()) {

               const String Name (qPrintable (Value.toString ()));

               undoHandle =
                  _undo.start_record (String ("Layer name changed to: ") + Name);

               retVal = _objectModule->store_text (layerHandle, _nameAttrHandle, Name);
            }
         }

         if (undoHandle) {

            _undo.stop_record (undoHandle);
         }
      }
   }

   if (retVal) { dataChanged (Index, Index); }

   return retVal;
}


QStringList
dmz::QtPluginCanvasLayerModel::mimeTypes () const {

   QStringList types;

   types << "text/plain";

   return types;
}


QMimeData *
dmz::QtPluginCanvasLayerModel::mimeData (const QModelIndexList &Indexes) const {

   QMimeData *mimeData (new QMimeData ());

   if (_objectModule) {

      Handle layerHandle (_get_layer_handle (Indexes.first ().row ()));

      UUID uuid;

      if (_objectModule->lookup_uuid (layerHandle, uuid)) {

         mimeData->setText (uuid.to_string ().get_buffer ());
      }
   }

   return mimeData;
}


bool
dmz::QtPluginCanvasLayerModel::dropMimeData (
      const QMimeData *Data,
      Qt::DropAction action,
      int row,
      int column,
      const QModelIndex &Parent) {

   bool retVal (false);

   if (_objectModule) {

      if (action == Qt::MoveAction) {

         if (Data->hasText ()) {

            UUID uuid (qPrintable (Data->text ()));

            Handle dragHandle (_objectModule->lookup_handle_from_uuid (uuid));

            Handle dropHandle (0);

            if ((row == -1) || (row >= _get_layer_count ())) {

               dropHandle = _rootLayerHandle;
            }
            else {

               dropHandle = _get_layer_handle (row);
            }

            _move_layer (dragHandle, dropHandle);

            retVal = true;
         }
      }
   }

   return retVal;
}


QVariant
dmz::QtPluginCanvasLayerModel::headerData (
      int section,
      Qt::Orientation ori,
      int role) const {

   QVariant retVal;

   if ((Qt::Horizontal == ori) && (Qt::DisplayRole == role)) {

      if (LayerLockedColumn == section) {

         retVal = tr ("L");
      }
      else if (LayerVisibleColumn == section) {

         retVal = tr ("V");
      }
      else if (LayerNameColumn == section) {

         retVal = tr ("Name");
      }
      else if (LayerZValueColumn == section) {

         retVal = tr ("Z");
      }
   }

   return retVal;
}


Qt::ItemFlags
dmz::QtPluginCanvasLayerModel::flags (const QModelIndex &Index) const {

   Qt::ItemFlags retVal;

   if (LayerLockedColumn == Index.column ()) {

      retVal = Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
   }
   else if (LayerVisibleColumn == Index.column ()) {

      retVal = Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
   }
   else if (LayerNameColumn == Index.column ()) {

      retVal =
         Qt::ItemIsEditable |
         Qt::ItemIsEnabled |
         Qt::ItemIsSelectable |
         Qt::ItemIsDragEnabled;
   }
   else if (LayerZValueColumn == Index.column ()) {

      retVal = Qt::ItemIsEnabled;
   }
   else {

      retVal = Qt::ItemIsDropEnabled;
   }

   return retVal;
}


Qt::DropActions
dmz::QtPluginCanvasLayerModel::supportedDropActions () const {

   return Qt::CopyAction | Qt::MoveAction;
}


// Object Observer Interface
void
dmz::QtPluginCanvasLayerModel::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   ObjectType layerType;
   _defs.lookup_object_type ("canvas_layer", layerType);

   if (Type.is_of_type (layerType)) {

      ObjectType rootLayerType;
      _defs.lookup_object_type ("root_layer", rootLayerType);

      if (Type.is_of_exact_type (rootLayerType)) {

         _rootLayerHandle = ObjectHandle;

         _log.debug << "Found Root Layer: " << _rootLayerHandle << endl;
      }
      else if (_canvasModule) {

         String name (Type.get_name ());
         name << "." << ObjectHandle;

         QtCanvasLayer *layer (new QtCanvasLayer ());
         layer->setData (QtCanvasObjectNameIndex, name.get_buffer ());
//         layer->setData (QtCanvasObjectHandleIndex, (qlonglong)ObjectHandle);

         _canvasModule->add_item (ObjectHandle, layer);
         _layerItemTable.store (ObjectHandle, layer);
      }
   }
}


void
dmz::QtPluginCanvasLayerModel::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   if (ObjectHandle == _rootLayerHandle) {

      _rootLayerHandle = 0;
      _layersUpdated = True;
   }
   else {

      if (_canvasModule) { _canvasModule->remove_item (ObjectHandle); }

      _layerItemTable.remove (ObjectHandle);

      _layersUpdated = True;

//      beginRemoveRows (QModelIndex (), 0, _get_layer_count ());
//      endRemoveRows ();

   }
}


void
dmz::QtPluginCanvasLayerModel::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   if ((SuperHandle == _rootLayerHandle && _layerItemTable.lookup (SubHandle)) ||
       (_layerItemTable.lookup (SuperHandle) && _layerItemTable.lookup (SubHandle))) {

      if (AttributeHandle == _orderAttrHandle) {

         _layersUpdated = True;

         // beginRemoveRows (QModelIndex (), 0, _get_layer_count () - 1);
         // endRemoveRows ();
      }
   }
}


void
dmz::QtPluginCanvasLayerModel::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   if (_layerItemTable.lookup (SuperHandle) && _layerItemTable.lookup (SubHandle)) {

      if (AttributeHandle == _orderAttrHandle) {

         _layersUpdated = True;

         // beginRemoveRows (QModelIndex (), 0, _get_layer_count () - 1);
         // endRemoveRows ();
      }
   }
}


void
dmz::QtPluginCanvasLayerModel::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   if (_layerItemTable.lookup (ObjectHandle)) {

      // if (AttributeHandle == _defaultAttrHandle) {
      //
      //    ObjectType objType;
      //    _lookup_object_type (ObjectHandle, objType);
      //
      //    _update_object (ObjectHandle, objType, Value);
      // }
   }
}


void
dmz::QtPluginCanvasLayerModel::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (_layerItemTable.lookup (ObjectHandle)) {

      if (AttributeHandle == _activeAttrHandle) {

         _set_layer_active (ObjectHandle, Value);
      }
      else if (AttributeHandle == _lockedAttrHandle) {

         _set_layer_locked (ObjectHandle, Value);
      }
      else if (AttributeHandle == _visibleAttrHandle) {

         _set_layer_visible (ObjectHandle, Value);
      }
   }
}


void
dmz::QtPluginCanvasLayerModel::update_object_text (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const String &Value,
      const String *PreviousValue) {

   if (_layerItemTable.lookup (ObjectHandle)) {

      if (AttributeHandle == _nameAttrHandle) {

         _model_changed (ObjectHandle, ObjectHandle);
      }
   }
}


void
dmz::QtPluginCanvasLayerModel::update_time_slice (const Float64 TimeDelta) {

   if (_layersUpdated) {

      _update_layer_order ();

      reset ();

      _layersUpdated = False;
   }
}


dmz::Int32
dmz::QtPluginCanvasLayerModel::_get_layer_count () const {

   Int32 count (0);

   Handle layerHandle (_get_next_layer_handle (_rootLayerHandle));

   while (layerHandle) {

      count++;
      layerHandle = _get_next_layer_handle (layerHandle);
   }

   return count;
}


dmz::Handle
dmz::QtPluginCanvasLayerModel::_get_layer_handle (const Int32 Row) const {

   Handle result (0);

   const Int32 Count (_get_layer_count ());

   if (Row == -1) {

      result = 0;
   }
   else if (Row < Count) {

      result = _get_next_layer_handle (_rootLayerHandle);

      for (Int32 ix = 1; ix < (Count - Row); ix++) {

         result = _get_next_layer_handle (result);
      }
   }

   return result;
}


dmz::Int32
dmz::QtPluginCanvasLayerModel::_get_layer_row (const Handle ObjectHandle) const {

   Int32 row (0);

   if (ObjectHandle) {

      Handle layerHandle (_get_next_layer_handle (_rootLayerHandle));

      while (layerHandle) {

         row++;

         if (ObjectHandle == layerHandle) {

            layerHandle = 0;
         }
         else {

            layerHandle = _get_next_layer_handle (layerHandle);
         }
      }
   }

   return (_get_layer_count () - row);
}


QVariant
dmz::QtPluginCanvasLayerModel::_get_display_data (const QModelIndex &Index) const {

   QVariant retVal;

   const Handle LayerHandle (_get_layer_handle (Index.row ()));

   if (_objectModule && LayerHandle) {

      if (Index.column () == LayerNameColumn) {

         String layerName;

         _objectModule->lookup_text (LayerHandle, _nameAttrHandle, layerName);

         if (!layerName) { layerName.flush () << "layer-" << LayerHandle; }

         retVal = layerName.get_buffer ();
      }
      else if (Index.column () == LayerZValueColumn) {

         QtCanvasLayer *item (_layerItemTable.lookup (LayerHandle));

         if (item) { retVal = item->zValue (); }
      }
   }

   return retVal;
}


QVariant
dmz::QtPluginCanvasLayerModel::_get_check_state_data (const QModelIndex &Index) const {

   QVariant retVal;

   const Handle LayerHandle (_get_layer_handle (Index.row ()));

   if (_objectModule && LayerHandle) {

      Handle attrHandle (0);

      if (Index.column () == LayerLockedColumn) {

         attrHandle = _lockedAttrHandle;
      }
      else if (Index.column () == LayerVisibleColumn) {

         attrHandle = _visibleAttrHandle;
      }

      if (attrHandle && _objectModule->lookup_flag (LayerHandle, attrHandle)) {

         retVal = Qt::Checked;
      }
      else { retVal = Qt::Unchecked; }

   }

   return retVal;
}


void
dmz::QtPluginCanvasLayerModel::_set_layer_active (
      const Handle ObjectHandle,
      const Boolean Value) {

   if (Value) {

      _activeLayerHandle = ObjectHandle;
      _model_changed (ObjectHandle, ObjectHandle);
   }
   else {

      if (ObjectHandle == _activeLayerHandle) {

         _activeLayerHandle = 0;
         _model_changed (ObjectHandle, ObjectHandle);
      }
   }
}


void
dmz::QtPluginCanvasLayerModel::_set_layer_locked (
      const Handle ObjectHandle,
      const Boolean Value) {

   QtCanvasLayer *item (_layerItemTable.lookup (ObjectHandle));

   if (item) {

      item->setEnabled (Value);
      _model_changed (ObjectHandle, ObjectHandle);
   }
}


void
dmz::QtPluginCanvasLayerModel::_set_layer_visible (
      const Handle ObjectHandle,
      const Boolean Value) {

   QtCanvasLayer *item (_layerItemTable.lookup (ObjectHandle));

   if (item) {

      item->setVisible (Value);

      if (_canvasModule) {

         QGraphicsScene *scene = _canvasModule->get_scene ();
         if (scene) { scene->update (); }
      }

      _model_changed (ObjectHandle, ObjectHandle);
   }
}


void
dmz::QtPluginCanvasLayerModel::_update_layer_order () {

   Float32 zValue (10.0f);

   Handle layerHandle (_get_next_layer_handle (_rootLayerHandle));

   while (layerHandle) {

      QGraphicsItem *item (_get_item (layerHandle));

      if (item) {

         item->setZValue (zValue);

         zValue += 10.0f;
      }

      layerHandle = _get_next_layer_handle (layerHandle);
   }
}


#if 0
void
dmz::QtPluginCanvasLayerModel::_update_layer_list () {

   Float32 zValue (10.0f);

_log.warn << "_update_layer_list" << endl;

   if (!_ignoreUpdates) {

_log.warn << "clear" << endl;
      _layerHandleList.clear ();

      Handle layerHandle (_get_next_layer_handle (_rootLayerHandle));

      while (layerHandle) {

         QGraphicsItem *item (_get_item (layerHandle));

         if (item) {

            item->setZValue (zValue);

            zValue += 10.0f;
         }

         _layerHandleList.prepend (layerHandle);

         layerHandle = _get_next_layer_handle (layerHandle);
      }

      _model_changed (_layerHandleList.first (), _layerHandleList.last ());
   }
}
#endif


QGraphicsItem *
dmz::QtPluginCanvasLayerModel::_get_item (const Handle ObjectHandle) const {

   QGraphicsItem *retVal (0);

   if (_canvasModule && ObjectHandle) {

      retVal = _canvasModule->lookup_item (ObjectHandle);
   }

   return retVal;
}


dmz::Handle
dmz::QtPluginCanvasLayerModel::_get_previous_layer_handle (
      const Handle ObjectHandle) const {

   Handle retVal (0);

   if (_objectModule && ObjectHandle) {

      HandleContainer links;

      if (_objectModule->lookup_super_links (ObjectHandle, _orderAttrHandle, links)) {

         retVal = links.get_first ();
      }
   }

   return retVal;
}


dmz::Handle
dmz::QtPluginCanvasLayerModel::_get_next_layer_handle (const Handle ObjectHandle) const {

   Handle retVal (0);

   if (_objectModule && ObjectHandle) {

      HandleContainer links;

      if (_objectModule->lookup_sub_links (ObjectHandle, _orderAttrHandle, links)) {

         retVal = links.get_first ();
      }
   }

   return retVal;
}


dmz::Handle
dmz::QtPluginCanvasLayerModel::_get_last_layer_handle (const Handle ObjectHandle) const {

   Handle result (ObjectHandle);

   if (_objectModule && ObjectHandle) {

      Handle next (_get_next_layer_handle (ObjectHandle));

      while (next) {

         next = _get_next_layer_handle (result);

         if (next) { result = next; }
      }
   }

   return result;
}


void
dmz::QtPluginCanvasLayerModel::_model_changed (
      const Handle FirstObject,
      const Handle LastObject) {

   int firstRow (_get_layer_row (FirstObject));

   QModelIndex topLeft (createIndex (firstRow, 0, (quint32)FirstObject));

   QModelIndex bottomRight (
      createIndex (firstRow, LayerColumnCount - 1, (quint32)FirstObject));

   if (LastObject && (LastObject != FirstObject)) {

      int lastRow (_get_layer_row (LastObject));

      if (lastRow != -1) {

         bottomRight = createIndex (lastRow, LayerColumnCount - 1, (quint32)LastObject);
      }
   }

   dataChanged (topLeft, bottomRight);
}


dmz::Boolean
dmz::QtPluginCanvasLayerModel::_remove_layer (const Handle SourceHandle) {

   Boolean result (False);

   if (SourceHandle && _objectModule) {

      const Handle SuperHandle (_get_previous_layer_handle (SourceHandle));
      const Handle SubHandle (_get_next_layer_handle (SourceHandle));

      // unlink SourceHandle then relink super to sub

      if (SuperHandle) {

         _objectModule->unlink_objects (
            _objectModule->lookup_link_handle (
               _orderAttrHandle,
               SuperHandle,
               SourceHandle));
      }

      if (SubHandle) {

         _objectModule->unlink_objects (
            _objectModule->lookup_link_handle (
               _orderAttrHandle,
               SourceHandle,
               SubHandle));

         if (SuperHandle) {

            result =
               _objectModule->link_objects (_orderAttrHandle, SuperHandle, SubHandle);
         }
      }
   }

   return result;
}


void
dmz::QtPluginCanvasLayerModel::_move_layer (
      const Handle SourceHandle,
      const Handle TargetHandle) {

   if (SourceHandle && _objectModule) {

      const Handle LastLayerHandle (_get_next_layer_handle (_rootLayerHandle));
      const Handle TargetPrevHandle (_get_previous_layer_handle (TargetHandle));
      const Handle TargetNextHandle (_get_next_layer_handle (TargetHandle));

      if ((SourceHandle == TargetHandle) || (SourceHandle == TargetNextHandle)) {

         // in this case the layer was dragged to a spot that does not move it
         // so no need to do anything
      }
      else {

         const Handle UndoHandle (_undo.start_record ("Layer Order Changed"));

         // unlink SourceHandle then relink super to sub
         _remove_layer (SourceHandle);

         // unlink TargetHandle then relink super to SourceHandle to sub
         Handle superHandle (TargetHandle);
         Handle subHandle (TargetNextHandle);

         if (subHandle) {

            _objectModule->unlink_objects (
               _objectModule->lookup_link_handle (
                  _orderAttrHandle,
                  superHandle,
                  subHandle));
         }

         _objectModule->link_objects (_orderAttrHandle, superHandle, SourceHandle);

         if (subHandle) {

            _objectModule->link_objects (_orderAttrHandle, SourceHandle, subHandle);
         }

         _undo.stop_record (UndoHandle);

         _model_changed (
            _get_last_layer_handle (_rootLayerHandle),
            _get_next_layer_handle (_rootLayerHandle));
      }
   }
}


// ============================================================================
// ============================================================================
// ============================================================================


dmz::QtPluginCanvasLayer::QtPluginCanvasLayer (
      const PluginInfo &Info,
      const Config &Local) :
      QFrame (0),
      Plugin (Info),
      TimeSlice (Info),
      QtWidget (Info),
      _log (Info),
      _defs (Info, &_log),
      _canvasModule (0),
      _canvasModuleName (),
      _objectModule (0),
      _objectModuleName (),
      _layerModel (Info, Local, this),
      _newLayerCount (1) {

   setObjectName (get_plugin_name ().get_buffer ());

   _ui.setupUi (this);

   _ui.treeView->setModel (&_layerModel);

   QHeaderView *header (_ui.treeView->header ());

   if (header) {

      header->setMovable (false);
      header->setResizeMode (LayerVisibleColumn, QHeaderView::ResizeToContents);
      header->setResizeMode (LayerLockedColumn, QHeaderView::ResizeToContents);
      header->setResizeMode (LayerNameColumn, QHeaderView::Stretch);
      header->hideSection (LayerLockedColumn);
      header->hideSection (LayerZValueColumn);
   }

   connect (
      _ui.treeView->selectionModel (),
      SIGNAL (currentRowChanged (const QModelIndex &, const QModelIndex &)),
      this,
      SLOT (_slot_current_changed (const QModelIndex &, const QModelIndex &)));

   _init (Local);
}


dmz::QtPluginCanvasLayer::~QtPluginCanvasLayer () {

}


// Plugin Interface
void
dmz::QtPluginCanvasLayer::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateStart) {

      _load_session ();
   }
   else if (State == PluginStateStop) {

      _save_session ();
   }
   else if (State == PluginStateShutdown) {

      _ui.treeView->setModel (0);
   }
}


void
dmz::QtPluginCanvasLayer::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      const String PluginName (PluginPtr ? PluginPtr->get_plugin_name () : "");

      if (!_canvasModule) {

         _canvasModule = QtModuleCanvas::cast (PluginPtr, _canvasModuleName);

         if (_canvasModule) {

            _layerModel.store_canvas_module (*_canvasModule);
         }
      }

      if (!_objectModule) {

         _objectModule = ObjectModule::cast (PluginPtr, _objectModuleName);

         if (_objectModule) {

   //            ObjectObserver *obs (ObjectObserver::cast (&_layerModel));
            ObjectObserver *obs = dynamic_cast<ObjectObserver *> (&_layerModel);

            if (obs) {

               obs->store_object_module (PluginName, *_objectModule);
            }
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_canvasModule && (_canvasModule == QtModuleCanvas::cast (PluginPtr))) {

         _layerModel.remove_canvas_module (*_canvasModule);

         _canvasModule = 0;
      }

      if (_objectModule && (_objectModule == ObjectModule::cast (PluginPtr))) {

   //      ObjectObserver *obs (ObjectObserver::cast (&_layerModel));
         ObjectObserver *obs = dynamic_cast<ObjectObserver *> (&_layerModel);

         if (obs) {

            obs->remove_object_module (PluginPtr->get_plugin_name (), *_objectModule);
         }

         _objectModule = 0;
      }
   }
}


void
dmz::QtPluginCanvasLayer::update_time_slice (const Float64 TimeDelta) {

   if (_layerModel.rowCount (QModelIndex ()) == 0) {

      _newLayerCount = 0;

      String name ("Network-");
      name << _newLayerCount++;

      Handle layer (_layerModel.create_layer (name, True, True, False, False));
   }

   if (_layerModel.rowCount (QModelIndex ()) > 1) {

      _ui.deleteButton->setEnabled (True);
   }
   else {

      _ui.deleteButton->setEnabled (False);
   }
}


// QtWidget Interface
QWidget *
dmz::QtPluginCanvasLayer::get_qt_widget () { return this; }


void
dmz::QtPluginCanvasLayer::on_addButton_clicked () {

   String name ("Network-");
   name << _newLayerCount++;

   Handle layer (_layerModel.create_layer (name, True, True, False, True));

   // QModelIndex index (_layerModel.index_for_handlered (layer));
   //
   // _ui.treeView->selectionModel ()->setCurrentIndex (
   //    index, QItemSelectionModel::ClearAndSelect);
}


void
dmz::QtPluginCanvasLayer::on_deleteButton_clicked () {

   _layerModel.delete_active_layer ();
}


void
dmz::QtPluginCanvasLayer::_slot_current_changed (
      const QModelIndex &Current,
      const QModelIndex &Previous) {

   if (Current.isValid ()) {

      const Handle CurrentHandle (Current.internalId ());
      _layerModel.set_active_layer (CurrentHandle);
   }
}


void
dmz::QtPluginCanvasLayer::_save_session () {

//   String data;

//   Config session (get_plugin_name ());

//   Config window ("window");

//   session.add_config (window);

//   set_session_config (get_plugin_runtime_context (), session);
}


void
dmz::QtPluginCanvasLayer::_load_session () {

//   Config session (get_session_config (get_plugin_name (), get_plugin_runtime_context ()));
}


void
dmz::QtPluginCanvasLayer::_init (const Config &Local) {

   _objectModuleName = config_to_string ("module.object.name", Local);
   _canvasModuleName = config_to_string ("module.canvas.name", Local);

   qframe_config_read ("frame", Local, this);

   qtoolbutton_config_read ("addLayer", Local, _ui.addButton);
   qtoolbutton_config_read ("deleteLayer", Local, _ui.deleteButton);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginCanvasLayer (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginCanvasLayer (Info, local);
}

};
