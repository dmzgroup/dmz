#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzQtConfigRead.h>
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
   const QLatin1String DataName ("Data");

   // enum ItemTypeEnum {
   //    ItemTypeDefault,
   //    ItemTypeBoolean,
   //    ItemTypeVector
   // };

   // const dmz::Int32 GroupItemType (QStandardItem::UserType + 1);
   // const dmz::Int32 VectorItemType (QStandardItem::UserType + 2);
   // const dmz::Int32 AttrHandleRole (Qt::UserRole + 2);
};


// Q_DECLARE_METATYPE (dmz::Vector);


struct dmz::QtObjectModel::RowStruct {

   public:
      RowStruct (const QString &Name) :
            attr (new QStandardItem (Name)),
            value (new QStandardItem ()) {
         
         attr->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      }
      
      void set_value (const QVariant &Value) { value->setData (Value, Qt::DisplayRole); }
      
      QList<QStandardItem *>get_items() {
         
         QList<QStandardItem *>row;
         row << attr;
         row << value;
         return row;
      }
      
      QStandardItem *attr;
      QStandardItem *value;
};


struct dmz::QtObjectModel::GroupStruct {

   public:
      GroupStruct (const QString &Name) : _item (new QStandardItem (Name)) {
         
         _item->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      }
      
      ~GroupStruct () {;}
      
      void add_row (const Handle AttrHandle, RowStruct *row) {

         if (row) {
            
            _item->appendRow (row->get_items ());

            _handleMap[row] = AttrHandle;
            _rowMap[AttrHandle] = row;
         }
      }
      
      QStandardItem *get_item () { return _item; }
      RowStruct *get_row (const Handle AttrHandle) { return _rowMap[AttrHandle]; }
      Handle get_handle (RowStruct *row) { return _handleMap[row]; }

   protected:
      QStandardItem *_item;
      QMap<RowStruct *, Handle> _handleMap;
      QMap<Handle, RowStruct *> _rowMap;
};


struct dmz::QtObjectModel::State {

   Log log;
   Definitions defs;
   QStandardItemModel *model;
   // QStandardItem *handleItem;
   // QStandardItem *typeItem;
   // QStandardItem *localityItem;
   // QStandardItem *uuidItem;
   // QMap<QStandardItem *, QString> itemToGroupMap;
   // QMap<QStandardItem *, Handle> itemToHandleMap;
   QMap<QString, GroupStruct *> groupMap;

   State (RuntimeContext *theContext) :
         log ("QtObjectModule", theContext),
         defs (theContext),
         model (0) {;}
         // handleItem (0),
         // typeItem (0),
         // localityItem (0),
         // uuidItem (0) {;}

   ~State () {;}

   QString handle_to_name (const Handle Object) {

      return QLatin1String (defs.lookup_named_handle_name (Object).get_buffer ());
   }
   
   // void add_item (const QString &GroupName, const Handle AttrHandle, QStandardItem *item) {
   // 
   //    itemToGroupMap[item] = GroupName;
   //    itemToHandleMap[item] = AttrHandle;
   // }
   // 
   // QStandardItem *append_row (
   //    const QString &Name1,
   //    const QString &Name2 = QString::null,
   //    QStandardItem *parent = 0);
   //    
   GroupStruct *get_group (const QString &GroupName);
   
   RowStruct *get_row (const QString &GroupName, const Handle AttrHandle);
};


// QStandardItem *
// dmz::QtObjectModel::State::append_row (
//       const QString &Name1,
//       const QString &Name2,
//       QStandardItem *parent) {
// 
//    if (!parent) { parent = model.invisibleRootItem (); }
// 
//    QStandardItem *item (0);
//    QList<QStandardItem *>row;
// 
//    item = new QStandardItem (Name1); row << item;
//    item->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
//    
//    item = new QStandardItem (Name2); row << item;
//    
//    parent->appendRow (row);
// 
//    return item;
// }


dmz::QtObjectModel::GroupStruct *
dmz::QtObjectModel::State::get_group (const QString &GroupName) {

   GroupStruct *group (groupMap[GroupName]);
   if (!group) {
   
      group = new GroupStruct (GroupName);
      groupMap[GroupName] = group;

      if (model) { model->invisibleRootItem ()->appendRow (group->get_item ()); }
      
      // QModelIndex index = item->index ();
      // ui.treeView->setExpanded (index, True);
      // ui.treeView->setFirstColumnSpanned (index.row (), index.parent (), True);
   }
   
   return group;
}


dmz::QtObjectModel::RowStruct *
dmz::QtObjectModel::State::get_row (const QString &GroupName, const Handle AttrHandle) {
   
   RowStruct *row (0);
   
   GroupStruct *group (get_group (GroupName));
   if (group) {

      row = group->get_row (AttrHandle);
      if (!row) {
         
         const QString AttrName (handle_to_name (AttrHandle));
         row = new RowStruct (AttrName);
         
         group->add_row (AttrHandle, row);

        // add_item (GroupName, AttrHandle, item);
         
         // if (GroupName == FlagName) {
         // 
         //    row->value->setFlags (
         //       Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
         // }
      }
   }
   
   return row;
}



dmz::QtObjectModel::QtObjectModel (
      RuntimeContext *context,
      QObject *parent) :
      QStandardItemModel (parent),
      _state (*(new State (context))) {

   _state.model = this;
}


dmz::QtObjectModel::~QtObjectModel () {

   delete &_state;
}


// void
// dmz::QtObjectModel::set_state_names (const QStringList &StateList) {
// 
//    _state.stateNameList = StateList;
// }


// Object Observer Interface
void
dmz::QtObjectModel::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {
         
   RowStruct *row (0);

   row = new RowStruct (UUIDName);
   row->set_value (to_qstring (Identity));
   invisibleRootItem ()->appendRow (row->get_items ());

   row = new RowStruct (HandleName);
   row->set_value (to_qstring (ObjectHandle));
   invisibleRootItem ()->appendRow (row->get_items ());

   row = new RowStruct (ObjectTypeName);
   row->set_value (to_qstring (Type));
   invisibleRootItem ()->appendRow (row->get_items ());

   row = new RowStruct (LocalityName);
   QStringList localityNames;
   localityNames << "Unknown" << "Local" << "Remote";
   row->set_value (localityNames[Locality]);
   invisibleRootItem ()->appendRow (row->get_items ());
}


void
dmz::QtObjectModel::update_object_uuid (
      const UUID &Identity,
      const UUID &PrevIdentity) {

   // if (!_state.uuidItem) {
   //    
   //    QStandardItem *attr = new QStandardItem (UUIDName);
   //    attr->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
   //    
   //    _state.uuidItem = new QStandardItem ();
   //    _state.uuidItem->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
   //    
   //    QList<QStandardItem *>row;
   //    row << attr;
   //    row << _state.uuidItem;
   // 
   //    invisibleRootItem ()->appendRow (row);
   // }
   // 
   // if (_state.uuidItem) { _state.uuidItem->setText (to_qstring (Identity)); }
}


void
dmz::QtObjectModel::remove_object_attribute (
      const Handle AttributeHandle,
      const Mask &AttrMask) {

}


void
dmz::QtObjectModel::update_object_locality (
      const ObjectLocalityEnum Locality,
      const ObjectLocalityEnum PrevLocality) {

   // if (!_state.localityItem) {
   //    
   //    QStandardItem *attr = new QStandardItem (LocalityName);
   //    attr->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
   //    
   //    _state.localityItem = new QStandardItem ();
   //    _state.localityItem->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
   //    
   //    QList<QStandardItem *>row;
   //    row << attr;
   //    row << _state.localityItem;
   // 
   //    invisibleRootItem ()->appendRow (row);
   // }
   // 
   // if (_state.localityItem) {
   //    
   //    QStringList localityNames;
   //    localityNames << "Unknown" << "Local" << "Remote";
   //    
   //    _state.localityItem->setText (localityNames[Locality]);
   // }
}


void
dmz::QtObjectModel::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

}


void
dmz::QtObjectModel::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

}


void
dmz::QtObjectModel::update_link_attribute_object (
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
dmz::QtObjectModel::update_object_counter (
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   RowStruct *row = _state.get_row (CounterName, AttributeHandle);
   if (row) { row->set_value (Value); }
}


void
dmz::QtObjectModel::update_object_counter_minimum (
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   RowStruct *row = _state.get_row (CounterMinimumName, AttributeHandle);
   if (row) { row->set_value (Value); }
}


void
dmz::QtObjectModel::update_object_counter_maximum (
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   RowStruct *row = _state.get_row (CounterMaximumName, AttributeHandle);
   if (row) { row->set_value (Value); }
}


void
dmz::QtObjectModel::update_object_alternate_type (
      const Handle AttributeHandle,
      const ObjectType &Value,
      const ObjectType *PreviousValue) {

   RowStruct *row = _state.get_row (AlternateTypeName, AttributeHandle);
   if (row) { row->set_value (to_qstring (Value)); }
}


void
dmz::QtObjectModel::update_object_state (
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

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


void
dmz::QtObjectModel::update_object_flag (
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   RowStruct *row = _state.get_row (FlagName, AttributeHandle);
   if (row) {
      
      row->value->setData (Value ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
   }
}


void
dmz::QtObjectModel::update_object_time_stamp (
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   RowStruct *row = _state.get_row (TimeStampName, AttributeHandle);
   if (row) { row->set_value (Value); }
}


void
dmz::QtObjectModel::update_object_position (
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   RowStruct *row = _state.get_row (PositionName, AttributeHandle);
   if (row) {
      
      QString vec = QString ("[%1, %2, %3]")
         .arg (Value.get_x ()).arg (Value.get_y ()).arg (Value.get_z ());
      
      row->set_value (vec);
   }
}


void
dmz::QtObjectModel::update_object_orientation (
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

//      _update_vector_property (PositionName, AttributeHandle, Value);
}


void
dmz::QtObjectModel::update_object_velocity (
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

      // _state.update_vector_property (VelocityName, AttributeHandle, Value);
}


void
dmz::QtObjectModel::update_object_acceleration (
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

      // _state.update_vector_property (AccelerationName, AttributeHandle, Value);
}


void
dmz::QtObjectModel::update_object_scale (
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

      // _state.update_vector_property (ScaleName, AttributeHandle, Value);
}


void
dmz::QtObjectModel::update_object_vector (
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

      // _state.update_vector_property (VectorName, AttributeHandle, Value);
}


void
dmz::QtObjectModel::update_object_scalar (
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   RowStruct *row = _state.get_row (ScalarName, AttributeHandle);
   if (row) { row->set_value (Value); }
}


void
dmz::QtObjectModel::update_object_text (
      const Handle AttributeHandle,
      const String &Value,
      const String *PreviousValue) {

   RowStruct *row = _state.get_row (TextName, AttributeHandle);
   if (row) { row->set_value (QLatin1String (Value.get_buffer ())); }
}


void
dmz::QtObjectModel::update_object_data (
      const Handle AttributeHandle,
      const Data &Value,
      const Data *PreviousValue) {

   // RowStruct *row = _state.get_row (DataName, AttributeHandle);
   // if (row) { row->set_value (QLatin1String ("[...data...]")); }
}


#if 0
void
dmz::QtObjectModel::_value_changed (QtProperty *property, const QVariant &Value) {

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
dmz::QtObjectModel::_value_changed (QtProperty *property, const Vector &Value) {

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


#if 0

dmz::GroupItem *
dmz::QtObjectModel::_get_group (const QString &Name) {

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
dmz::QtObjectModel::_get_attr (GroupItem *group, const Handle Attr) {
   
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
