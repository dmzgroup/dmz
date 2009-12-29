#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzQtConfigRead.h>
#include "dmzQtObjectInspector.h"
#include "dmzQtObjectModel.h"
#include <dmzQtUtil.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesUUID.h>
#include <dmzTypesVector.h>
#include <QtGui/QtGui>
#include "ui_ObjectInspectorForm.h"


struct dmz::QtObjectInspector::State {

   const Handle ObjHandle;
   ObjectObserverUtil &obs;
   Log log;
   Definitions defs;
   Ui::ObjectInspectorForm ui;
   QtObjectModel model;
   QStringList stateNameList;
   Boolean ignoreUpdates;
   QMap<QString, Mask> stateMap;

   State (const Handle TheHandle, ObjectObserverUtil &theObs, RuntimeContext *theContext) :
         ObjHandle (TheHandle),
         obs (theObs),
         log ("QtObjectInspector", theContext),
         defs (theContext),
         ignoreUpdates (false),
         model (theContext) {;}

   ~State () {;}

   QString handle_to_name (const Handle Object) {

      return QLatin1String (defs.lookup_named_handle_name (Object).get_buffer ());
   }
};


dmz::QtObjectInspector::QtObjectInspector (
      const Handle ObjHandle,
      ObjectObserverUtil &observer,
      RuntimeContext *context,
      QWidget *parent) :
      QFrame (parent),
      _state (*(new State (ObjHandle, observer, context))) {

   _init ();
}


dmz::QtObjectInspector::~QtObjectInspector () {

   delete &_state;
}


void
dmz::QtObjectInspector::set_state_names (const QStringList &StateList) {

   _state.stateNameList = StateList;
}


void
dmz::QtObjectInspector::expand_all () {
   
   _state.ui.treeView->expandAll ();
}


void
dmz::QtObjectInspector::collapse_all () {
   
   _state.ui.treeView->collapseAll ();
}



// Object Observer Interface
void
dmz::QtObjectInspector::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (ObjectHandle == _state.ObjHandle && !_state.ignoreUpdates) {

      const QString HandleStr (to_qstring (ObjectHandle));
      setWindowTitle (tr ("%1 (%2)").arg (windowTitle ()).arg (HandleStr));

      _state.model.create_object (Identity, ObjectHandle, Type, Locality);
   }
}


void
dmz::QtObjectInspector::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   if (ObjectHandle == _state.ObjHandle) {


      // _state.handleItem->setFlags (False);
      // _state.typeItem->setEnabled (False);
      // _state.localityItem->setEnabled (False);
      // _state.uuidItem->setEnabled (False);
      
      // _state.ui.treeWidget->setEnabled (False);
      // _state.ui.addAttributeButton->setEnabled (False);
      // _state.ui.removeAttributeButton->setEnabled (False);
      
      _state.ignoreUpdates = true;

      // QMap<QtProperty *, Handle>::ConstIterator itProp =
      //    _state.propertyToHandleMap.constBegin ();
      // 
      // while (itProp != _state.propertyToHandleMap.constEnd ()) {
      // 
      //    itProp.key ()->setEnabled (False);
      //    itProp++;
      // }
   }
}


void
dmz::QtObjectInspector::update_object_uuid (
      const Handle ObjectHandle,
      const UUID &Identity,
      const UUID &PrevIdentity) {

   if (ObjectHandle ==  _state.ObjHandle && !_state.ignoreUpdates) {

      _state.model.update_object_uuid (Identity, PrevIdentity);
   }
}


void
dmz::QtObjectInspector::remove_object_attribute (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &AttrMask) {

}


void
dmz::QtObjectInspector::update_object_locality (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectLocalityEnum Locality,
      const ObjectLocalityEnum PrevLocality) {

   if (ObjectHandle ==  _state.ObjHandle && !_state.ignoreUpdates) {

      _state.model.update_object_locality (Locality, PrevLocality);
   }
}


void
dmz::QtObjectInspector::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

}


void
dmz::QtObjectInspector::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

}


void
dmz::QtObjectInspector::update_link_attribute_object (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle,
      const UUID &AttributeIdentity,
      const Handle AttributeObjectHandle,
      const UUID &PrevAttributeIdentity,
      const Handle PrevAttributeObjectHandle) {


}


void
dmz::QtObjectInspector::update_object_counter (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   if (ObjectHandle ==  _state.ObjHandle && !_state.ignoreUpdates) {

      _state.model.update_object_counter (AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtObjectInspector::update_object_counter_minimum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   if (ObjectHandle ==  _state.ObjHandle && !_state.ignoreUpdates) {

      _state.model.update_object_counter_minimum (AttributeHandle, Value, PreviousValue); 
   }
}


void
dmz::QtObjectInspector::update_object_counter_maximum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   if (ObjectHandle ==  _state.ObjHandle && !_state.ignoreUpdates) {

      _state.model.update_object_counter_maximum (AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtObjectInspector::update_object_alternate_type (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const ObjectType &Value,
      const ObjectType *PreviousValue) {

   if (ObjectHandle ==  _state.ObjHandle && !_state.ignoreUpdates) {

      _state.model.update_object_alternate_type (AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtObjectInspector::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   if ((ObjectHandle == _state.ObjHandle) && !_state.ignoreUpdates) {
      
      _state.model.update_object_state (AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtObjectInspector::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (ObjectHandle == _state.ObjHandle && !_state.ignoreUpdates) {

      _state.model.update_object_flag (AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtObjectInspector::update_object_time_stamp (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   if (ObjectHandle == _state.ObjHandle && !_state.ignoreUpdates) {

      _state.model.update_object_time_stamp (AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtObjectInspector::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (ObjectHandle ==  _state.ObjHandle && !_state.ignoreUpdates) {

      _state.model.update_object_position (AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtObjectInspector::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

   if (ObjectHandle ==  _state.ObjHandle && !_state.ignoreUpdates) {

//      _update_vector_property (PositionName, AttributeHandle, Value);
   }
}


void
dmz::QtObjectInspector::update_object_velocity (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (ObjectHandle ==  _state.ObjHandle && !_state.ignoreUpdates) {

      // _state.update_vector_property (VelocityName, AttributeHandle, Value);
   }
}


void
dmz::QtObjectInspector::update_object_acceleration (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (ObjectHandle ==  _state.ObjHandle && !_state.ignoreUpdates) {

      // _state.update_vector_property (AccelerationName, AttributeHandle, Value);
   }
}


void
dmz::QtObjectInspector::update_object_scale (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (ObjectHandle ==  _state.ObjHandle && !_state.ignoreUpdates) {

      // _state.update_vector_property (ScaleName, AttributeHandle, Value);
   }
}


void
dmz::QtObjectInspector::update_object_vector (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (ObjectHandle ==  _state.ObjHandle && !_state.ignoreUpdates) {

      // _state.update_vector_property (VectorName, AttributeHandle, Value);
   }
}


void
dmz::QtObjectInspector::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   if (ObjectHandle ==  _state.ObjHandle && !_state.ignoreUpdates) {

      _state.model.update_object_scalar (AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtObjectInspector::update_object_text (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const String &Value,
      const String *PreviousValue) {

   if (ObjectHandle ==  _state.ObjHandle && !_state.ignoreUpdates) {

      _state.model.update_object_text (AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtObjectInspector::update_object_data (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Data &Value,
      const Data *PreviousValue) {

   if (ObjectHandle ==  _state.ObjHandle && !_state.ignoreUpdates) {

      _state.model.update_object_data (AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtObjectInspector::_item_activated (const QModelIndex &Index) {

   QStandardItem *item = _state.model.itemFromIndex (Index);
   
   if (item) {

      _state.log.warn << "item_activated: " << (Int32)Index.row () << " " << (Int32)Index.column () << endl;
   }
}


#if 0
void
dmz::QtObjectInspector::_value_changed (QtProperty *property, const QVariant &Value) {

   _state.ignoreUpdates = True;

   ObjectModule *objMod (_state.obs.get_object_module ());
   if (objMod && !_state.ignoreValueChanged) {

      const QString Group (_state.propertyToGroupMap[property]);
      const Handle Attr (_state.propertyToHandleMap[property]);

      if (Group == TextName) {

         const String Text (qPrintable (Value.toString ()));
         objMod->store_text (_state.ObjHandle, Attr, Text);
      }
      else if (Group == FlagName) {

         objMod->store_flag (_state.ObjHandle, Attr, Value.toBool ());
      }
      else if (Group == TimeStampName) {

         objMod->store_time_stamp (_state.ObjHandle, Attr, Value.toDouble ());
      }
      else if (Group == ScalarName) {

         objMod->store_scalar (_state.ObjHandle, Attr, Value.toDouble ());
      }
      else if (Group == CounterName) {

         objMod->store_counter (_state.ObjHandle, Attr, Value.toLongLong ());
      }
      else if (Group == CounterMinimumName) {

         objMod->store_counter_minimum (_state.ObjHandle, Attr, Value.toLongLong ());
      }
      else if (Group == CounterMaximumName) {

         objMod->store_counter_maximum (_state.ObjHandle, Attr, Value.toLongLong ());
      }
      else if (Group == AlternateTypeName) {

         ObjectType type;
         const String Text (qPrintable (Value.toString ()));
         if (_state.defs.lookup_object_type (Text, type)) {

            objMod->store_alternate_object_type (_state.ObjHandle, Attr, type);
         }
      }
   }

   _state.ignoreUpdates = False;
}

void
dmz::QtObjectInspector::_value_changed (QtProperty *property, const Vector &Value) {

   _state.ignoreUpdates = True;

   ObjectModule *objMod (_state.obs.get_object_module ());
   if (objMod) {

      const QString Group (_state.propertyToGroupMap[property]);
      const Handle Attr (_state.propertyToHandleMap[property]);

      if (Group == PositionName) {

         objMod->store_position (_state.ObjHandle, Attr, Value);
      }
      else if (Group == VelocityName) {

         objMod->store_velocity (_state.ObjHandle, Attr, Value);
      }
      else if (Group == AccelerationName) {

         objMod->store_acceleration (_state.ObjHandle, Attr, Value);
      }
      else if (Group == ScaleName) {

         objMod->store_scale (_state.ObjHandle, Attr, Value);
      }
      else if (Group == VectorName) {

         objMod->store_vector (_state.ObjHandle, Attr, Value);
      }
   }

   _state.ignoreUpdates = False;
}
#endif


void
dmz::QtObjectInspector::closeEvent (QCloseEvent *event) {

   Q_EMIT finished (_state.ObjHandle);
}

#if 0

dmz::GroupItem *
dmz::QtObjectInspector::_get_group (const QString &Name) {

   GroupItem *group = _state.groupToItemMap[Name];
   if (!group) {
   
      group = new GroupItem ();
      group->setText (Name);
      _state.groupToItemMap[Name] = group;
      
      QStandardItem *parentItem = _state.model.invisibleRootItem ();
      parentItem->appendRow (group);
   }
   
   return group;
}


QStandardItem *
dmz::QtObjectInspector::_get_attr (GroupItem *group, const Handle Attr) {
   
   QStandardItem *item (0);
   
   if (group) {
   
_state.log.warn << "group rowCount: " << (Int32)group->rowCount () << " checking for " << Attr << endl;
//      item = group->get_child_with_attribute (Attr);
      
      for (Int32 ix = 0; ix < group->rowCount (); ix++) {
         
         QStandardItem *childItem = group->child (ix, 0);
         
         _state.log.warn << "child: " << ix << " : " << (Handle)(childItem->data ().toLongLong ()) << endl;
         if (Attr == childItem->data ().toLongLong ()) {
         
            item = childItem;
            break;
         }
      }
      
      if (!item) {
         
         const QString AttrName (_state.handle_to_name (Attr));
         QList<QStandardItem *> row; 
         
         item = new QStandardItem (AttrName); row << item;
         item = new QStandardItem (); row << item;
         item->setData ((qlonglong)Attr);
         
         group->appendRow (row);
      }
   }
   
   return item;
}

#endif


void
dmz::QtObjectInspector::_init () {

   // QString name ("%1-%2");
   // name.arg (_state.obs.get_plugin_name ().get_buffer ()).arg (_state.ObjHandle);
   // setObjectName (name);

   _state.ui.setupUi (this);
   _state.ui.treeView->setModel (&_state.model);
   
   QStringList labels;
   labels << "Property" << "Value";
   _state.model.setHorizontalHeaderLabels (labels);
   
   QHeaderView *header (_state.ui.treeView->header ());
   if (header) {

      header->setResizeMode (0, QHeaderView::ResizeToContents);
      header->setResizeMode (1, QHeaderView::ResizeToContents);
   }
   
   connect (
      _state.ui.treeView, SLOT (activated (const QModelIndex &)),
      this, SLOT (_item_ativated (const QModelIndex &)));
}
