#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzQtConfigRead.h>
#include "dmzQtObjectInspector.h"
#include <dmzQtUtil.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesUUID.h>
#include <QtGui/QtGui>
#include "ui_ObjectInspectorForm.h"


namespace {

   const QLatin1String HandleName ("Handle");
   const QLatin1String UUIDName ("UUID");
   const QLatin1String LocalityName ("Locality");
   const QLatin1String TimeStampName ("Time Stamp");
   const QLatin1String FlagName ("Flag");
   const QLatin1String PositionName ("Position");
   const QLatin1String TextName ("Text");
   const QLatin1String StateName ("State");
   const QLatin1String ObjectTypeName ("Object Type");
   const QLatin1String VelocityName ("Velocity");
   const QLatin1String AccelerationName ("Acceleration");
   const QLatin1String ScaleName ("Scale");
   const QLatin1String ScalarName ("Scalar");
   const QLatin1String CounterName ("Counter");
   const QLatin1String CounterMinimumName ("Counter Minimum");
   const QLatin1String CounterMaximumName ("Counter Maximum");
   const QLatin1String AlternateTypeName ("Alternate Object Type");
   const QLatin1String VectorName ("Vector");

   const dmz::Int32 GroupItemType (QStandardItem::UserType + 1);
//   const dmz::Int32 AttrHandleRole (Qt::UserRole + 1);
};

namespace dmz {
   
   class GroupItem : public QStandardItem {
      
      public:
//         GroupItem (const QString &Text) : QStandardItem (Text) {;}
         
         int type () const { return GroupItemType; }
         
         QStandardItem *get_child_for_attribute (const Handle AttrHandle) {
            
            QStandardItem *retVal (0);
            
            for (int ix = 0; ix < rowCount (); ix++) {
               
               QStandardItem *item = child (ix, 0);
               if (AttrHandle == item->data ().toULongLong ()) {
               
                  retVal = item;
                  break;
               }
            }
            
            return retVal;
         }
   };
};


struct dmz::QtObjectInspector::State {

   const Handle ObjHandle;
   ObjectObserverUtil &obs;
   Log log;
   Definitions defs;
   Ui::ObjectInspectorForm ui;
   QStandardItemModel model;
   QStringList stateNameList;
   Boolean ignoreUpdates;
   QMap<QString, Mask> stateMap;
   QStandardItem *handleItem;
   QStandardItem *typeItem;
   QStandardItem *localityItem;
   QStandardItem *uuidItem;
   QMap<QStandardItem *, QString> itemToGroupMap;
   QMap<QStandardItem *, Handle> itemToHandleMap;
   QMap<QString, GroupItem *> groupToItemMap;

   State (const Handle TheHandle, ObjectObserverUtil &theObs, RuntimeContext *theContext) :
         ObjHandle (TheHandle),
         obs (theObs),
         log ("QtObjectInspector", theContext),
         defs (theContext),
         ignoreUpdates (false),
         handleItem (0),
         typeItem (0),
         localityItem (0),
         uuidItem (0) {;}

   ~State () {;}

   QString handle_to_name (const Handle Object) {

      return QLatin1String (defs.lookup_named_handle_name (Object).get_buffer ());
   }
};


// dmz::QtObjectInspector::GroupStruct *
// dmz::QtObjectInspector::State::get_group (const QString &GroupName) {
// 
//    GroupStruct *group (groupMap[GroupName]);
//    if (!group && groupManager) {
// 
//       group = new GroupStruct (GroupName);
//       group->add_property (groupManager->addProperty (GroupName));
//       groupMap[GroupName] = group;
//       ui.propertyEditor->addProperty (group->get_property ());
//    }
// 
//    return group;
// }


// void
// dmz::QtObjectInspector::State::update_variant_property (
//       const QString &GroupName,
//       const Handle AttrHandle,
//       const int PropertyType,
//       const QVariant Value) {
// 
//    if (variantManager) {
// 
//       ignoreValueChanged = true;
// 
//       GroupStruct *group (get_group (GroupName));
//       if (group) {
// 
//          QtProperty *property (group->get_property (AttrHandle));
//          if (!property) {
// 
//             const QString AttrName (handle_to_name (AttrHandle));
//             property = variantManager->addProperty (PropertyType, AttrName);
//             group->add_property (AttrHandle, property);
//             add_property (GroupName, AttrHandle, property);
//          }
// 
//          if (property) { variantManager->setValue (property, Value); }
//       }
// 
//       ignoreValueChanged = false;
//    }
// }


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

      QStandardItem *parentItem = _state.model.invisibleRootItem ();
      
      QStandardItem *item (0);
      QList<QStandardItem *>row;
      
      item = new QStandardItem (HandleName); row << item;
      item = new QStandardItem (HandleStr); row << item;
      _state.handleItem = item;
      
      parentItem->appendRow (row);
      row.clear ();
      
      item = new QStandardItem (ObjectTypeName); row << item;
      item = new QStandardItem (to_qstring (Type)); row << item;
      _state.typeItem = item;
      
      parentItem->appendRow (row);

      update_object_locality (Identity, ObjectHandle, Locality, Locality);
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

      QStandardItem *item = _state.uuidItem;
      
      if (!item) {
         
         QStandardItem *parentItem = _state.model.invisibleRootItem ();
         QList<QStandardItem *> row; 

         item = new QStandardItem (UUIDName); row << item;
         item = new QStandardItem (); row << item;
         _state.uuidItem = item;

         parentItem->appendRow (row);
      }

      if (item) {
         
         item->setText (to_qstring (Identity));
      }
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

      QStandardItem *item = _state.localityItem;
      
      if (!item) {
         
         QStandardItem *parentItem = _state.model.invisibleRootItem ();

         QStandardItem *item (0);
         QList<QStandardItem *> row; 

         item = new QStandardItem (LocalityName); row << item;
         item = new QStandardItem (); row << item;
         _state.localityItem = item;

         parentItem->appendRow (row);
      }

      if (item) {
         
         QStringList localityNames;
         localityNames << "Unknown" << "Local" << "Remote";
         
         item->setText (localityNames[Locality]);
      }
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

      // _state.update_variant_property (
      //    CounterName, AttributeHandle, QVariant::Int, Value);
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

      // _state.update_variant_property (
      //    CounterMinimumName, AttributeHandle, QVariant::Int, Value);
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

      // _state.update_variant_property (
      //    CounterMaximumName, AttributeHandle, QVariant::Int, Value);
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

      // _state.update_variant_property (
      //    AlternateTypeName, AttributeHandle, QVariant::String, to_qstring (Value));
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

//      _state.update_mask_property (StateName, AttributeHandle, Value);

      // String name;
      // _state.defs.lookup_state_name (Value, name);
      // 
      // _state.update_variant_property (
      //    StateName,
      //    AttributeHandle,
      //    QVariant::String,
      //    QLatin1String (name.get_buffer ()));
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

      // GroupItem *group = _get_group (FlagName);
      // if (group) {
      // 
      //    QTreeWidgetItem *item = _get_attr (group, AttributeHandle);
      //    if (item) {
      //       
      //       item->setData (1, Qt::DisplayRole, Value);
      //    }
      // }
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

      // GroupItem *group = _get_group (TimeStampName);
      // if (group) {
      // 
      //    QTreeWidgetItem *item = _get_attr (group, AttributeHandle);
      //    if (item) {
      //       
      //       item->setData (1, Qt::DisplayRole, Value);
      //    }
      // }
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

       // _state.update_vector_property (PositionName, AttributeHandle, Value);
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

      // _state.update_variant_property (
      //    ScalarName, AttributeHandle, QVariant::Double, Value);
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

      // _state.update_variant_property (
      //    TextName,
      //    AttributeHandle,
      //    QVariant::String,
      //    QLatin1String (Value.get_buffer ()));
   }
}


void
dmz::QtObjectInspector::update_object_data (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Data &Value,
      const Data *PreviousValue) {

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

/*
dmz::GroupItem *
dmz::QtObjectInspector::_get_group (const QString &Name) {

   GroupItem *group = _state.groupToItemMap[Name];
   if (!group) {
   
      group = new GroupItem (_state.ui.treeWidget);
      group->setFirstColumnSpanned (True);
      group->setText (0, Name);
      _state.groupToItemMap[Name] = group;
   }
   
   return group;
}


QTreeWidgetItem *
dmz::QtObjectInspector::_get_attr (GroupItem *group, const Handle Attr) {
   
   QTreeWidgetItem *item (0);
   
   if (group) {
   
      item = group->get_attribute (Attr);
      
      if (!item) {
         
         const QString AttrName (_state.handle_to_name (Attr));
         
         item = new QTreeWidgetItem (group);
         item->setText (0, AttrName);
         item->setData (0, Qt::UserRole, (qlonglong)Attr);
         group->store_attribute (Attr, item);
      }
   }
   
   return item;
}
*/

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

   // connect (
   //    _state.variantManager, SIGNAL (valueChanged (QtProperty *, const QVariant &)),
   //    this, SLOT (_value_changed (QtProperty *, const QVariant &)));
}
