#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzQtConfigRead.h>
#include "dmzQtMaskInputDialog.h"
#include "dmzQtObjectConsts.h"
#include "dmzQtObjectInspector.h"
#include <dmzQtUtil.h>
#include "dmzQtVectorInputDialog.h"
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesUUID.h>
#include <dmzTypesVector.h>
#include <QtGui/QtGui>
#include "ui_ObjectInspectorForm.h"


namespace {
   
   const char *ObjectAttrName[dmz::MaxObjectAttr] = {
      "Create Object",
      "Destroy Object",
      "Handle",
      "Object Type",
      "Locality",
      "UUID",
      "Remove",
      "Link",
      "Unlink",
      "Link Object",
      "Counter",
      "Counter Min",
      "Counter Max",
      "Alternate Object Type",
      "State",
      "Flag",
      "TimeStamp",
      "Position",
      "Orientation",
      "Velocity",
      "Acceleration",
      "Scale",
      "Vector",
      "Scalar",
      "Text",
      "Data"
   };
   
   class AttributeItem : public QTreeWidgetItem {
      
      public:
         AttributeItem (const dmz::ObjectAttrEnum Type) :
               QTreeWidgetItem (Type + QTreeWidgetItem::UserType) {
         
            Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
            if (Type == dmz::ObjectAttrFlag) { flags |= Qt::ItemIsUserCheckable; }
            setFlags (flags);
         }
         
         void set_value (const dmz::Vector &Value) {
            
            QString data = QString ("[%1, %2, %3]")
               .arg (Value.get_x (), 0, 'f', 2)
               .arg (Value.get_y (), 0, 'f', 2)
               .arg (Value.get_z (), 0, 'f', 2);

            setText (dmz::ValueCol, data);
         }
   };
   
   class GroupItem : public QTreeWidgetItem {
   
      public:
         GroupItem (QTreeWidget *parent, const dmz::ObjectAttrEnum Type) :
               QTreeWidgetItem (parent, Type + QTreeWidgetItem::UserType) {
                  
            setText (dmz::AttributeCol, ObjectAttrName[Type]);
            setFlags (Qt::ItemIsEnabled);
            setFirstColumnSpanned (true);
            setExpanded (true);  
         }
         
         void add_item (const dmz::Handle AttrHandle, AttributeItem *item) {
         
            _handleMap[item] = AttrHandle;
            _itemMap[AttrHandle] = item;
            addChild (item);
         }
      
         AttributeItem *get_item (const dmz::Handle AttrHandle) {
         
            return _itemMap[AttrHandle];
         }
         
         dmz::Handle get_handle (QTreeWidgetItem *item) {
            
            return _handleMap[item];
         }
      
      protected:
         QMap<QTreeWidgetItem *, dmz::Handle> _handleMap;
         QMap<dmz::Handle, AttributeItem *> _itemMap;
   };
};


struct dmz::QtObjectInspector::State {

   ObjectObserverUtil &obs;
   Log log;
   Definitions defs;
   RuntimeContext *context;
   Handle handle;
   Handle attrHandle;
   Ui::ObjectInspectorForm ui;
   QStringList stateNameList;
   QMap<QString, Mask> stateMap;
   QMap<QTreeWidgetItem *, ObjectAttrEnum> itemToGroupMap;
   QMap<QTreeWidgetItem *, Handle> itemToHandleMap;
   QMap<ObjectAttrEnum, GroupItem *> groupMap;

   State (ObjectObserverUtil &theObs, RuntimeContext *theContext) :
         obs (theObs),
         log ("QtObjectInspector", theContext),
         defs (theContext),
         context (theContext),
         handle (0),
         attrHandle (0) {;}

   ~State () {;}
   
   GroupItem *get_group (const ObjectAttrEnum Type) {
      
      GroupItem *group = groupMap[Type];
      if (!group) {
         
         group = new GroupItem (ui.treeWidget, Type);
         groupMap[Type] = group;
      }
      
      return group;
   }
   
   AttributeItem *get_item (const ObjectAttrEnum Type, const Handle AttrHandle) {

      AttributeItem *item (0);
      
      GroupItem *group = get_group (Type);
      if (group) {

         item = group->get_item (AttrHandle);
         if (!item) {

            item = new AttributeItem (Type);
            item->setText (AttributeCol, handle_to_name (AttrHandle));

            group->add_item (AttrHandle, item);

            itemToGroupMap[item] = Type;
            itemToHandleMap[item] = AttrHandle;
         }
      }
      
      return item;
   }
   
   QString handle_to_name (const Handle Object) {

      return QLatin1String (defs.lookup_named_handle_name (Object).get_buffer ());
   }
};


dmz::QtObjectInspector::QtObjectInspector (
      ObjectObserverUtil &observer,
      RuntimeContext *context,
      QWidget *parent) :
      QFrame (parent),
      _state (*(new State (observer, context))) {

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

   if (!_state.handle) {
      
      QString title = QString ("Object %1").arg (to_qstring (ObjectHandle));
      setWindowTitle (title);
      
      _state.handle = ObjectHandle;
      AttributeItem *item (0);
      
      item = new AttributeItem (ObjectAttrUUID);
      item->setText (AttributeCol, ObjectAttrName[ObjectAttrUUID]);
      item->setText (ValueCol, to_qstring (Identity));
      item->setFlags (Qt::ItemIsEnabled);      
      _state.ui.treeWidget->addTopLevelItem (item);

      item = new AttributeItem (ObjectAttrHandle);
      item->setText (AttributeCol, ObjectAttrName[ObjectAttrHandle]);
      item->setText (ValueCol, to_qstring (ObjectHandle));
      item->setFlags (Qt::ItemIsEnabled);
      _state.ui.treeWidget->addTopLevelItem (item);

      item = new AttributeItem (ObjectAttrObjectType);
      item->setText (AttributeCol, ObjectAttrName[ObjectAttrObjectType]);
      item->setText (ValueCol, to_qstring (Type));
      item->setFlags (Qt::ItemIsEnabled);
      _state.ui.treeWidget->addTopLevelItem (item);

      char *localityNames[3] = { "Unknown", "Local", "Remote" };
      item = new AttributeItem (ObjectAttrLocality);
      item->setText (AttributeCol, ObjectAttrName[ObjectAttrLocality]);
      item->setText (ValueCol, localityNames[Locality]);
      item->setFlags (Qt::ItemIsEnabled);
      _state.ui.treeWidget->addTopLevelItem (item);
   }
}


void
dmz::QtObjectInspector::destroy_object () {

   QString title = QString ("Object %1 (Destroyed)").arg (to_qstring (_state.handle));
   setWindowTitle (title);
}


void
dmz::QtObjectInspector::remove_object_attribute (
      const Handle AttributeHandle,
      const Mask &AttrMask) {

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
      const Handle AttributeObjectHandle) {


}


void
dmz::QtObjectInspector::update_object_counter (
      const Handle AttributeHandle,
      const Int64 Value) {

   AttributeItem *item = _state.get_item (ObjectAttrCounter, AttributeHandle);
   if (item) { item->setText (ValueCol, QString::number (Value)); }
}


void
dmz::QtObjectInspector::update_object_counter_minimum (
      const Handle AttributeHandle,
      const Int64 Value) {

   AttributeItem *item = _state.get_item (ObjectAttrCounterMin, AttributeHandle);
   if (item) { item->setText (ValueCol, QString::number (Value)); }
}


void
dmz::QtObjectInspector::update_object_counter_maximum (
      const Handle AttributeHandle,
      const Int64 Value) {

   AttributeItem *item = _state.get_item (ObjectAttrCounterMax, AttributeHandle);
   if (item) { item->setText (ValueCol, QString::number (Value)); }
}


void
dmz::QtObjectInspector::update_object_alternate_type (
      const Handle AttributeHandle,
      const ObjectType &Value) {

   AttributeItem *item = _state.get_item (ObjectAttrAltObjectType, AttributeHandle);
   if (item) { item->setText (ValueCol, to_qstring (Value)); }
}


void
dmz::QtObjectInspector::update_object_state (
      const Handle AttributeHandle,
      const Mask &Value) {

   AttributeItem *item = _state.get_item (ObjectAttrState, AttributeHandle);
   if (item) {
      
      String name;
      _state.defs.lookup_state_name (Value, name);
      
      item->setText (ValueCol, name.get_buffer ());
   }
}


void
dmz::QtObjectInspector::update_object_flag (
      const Handle AttributeHandle,
      const Boolean Value) {

   AttributeItem *item = _state.get_item (ObjectAttrFlag, AttributeHandle);
   if (item) {
      
      item->setCheckState (ValueCol, Value ? Qt::Checked : Qt::Unchecked);
   }
}


void
dmz::QtObjectInspector::update_object_time_stamp (
      const Handle AttributeHandle,
      const Float64 Value) {

   AttributeItem *item = _state.get_item (ObjectAttrTimeStamp, AttributeHandle);
   if (item) { item->setText (ValueCol, QString::number (Value, 'f', 2)); }
}


void
dmz::QtObjectInspector::update_object_position (
      const Handle AttributeHandle,
      const Vector &Value) {

   AttributeItem *item = _state.get_item (ObjectAttrPosition, AttributeHandle);
   if (item) { item->set_value (Value); }
}


void
dmz::QtObjectInspector::update_object_orientation (
      const Handle AttributeHandle,
      const Matrix &Value) {

   // AttributeItem *item = _state.get_item (ObjectAttrOrientation, AttributeHandle);
   // if (item) {
   // 
   //    QString text = QString ("[%1, %2, %3]")
   //       .arg (Value.get_x (), 0, 'f', 2)
   //       .arg (Value.get_y (), 0, 'f', 2)
   //       .arg (Value.get_z (), 0, 'f', 2);
   // 
   //    item->setText (ValueCol, text);
   // }
}


void
dmz::QtObjectInspector::update_object_velocity (
      const Handle AttributeHandle,
      const Vector &Value) {

   AttributeItem *item = _state.get_item (ObjectAttrVelocity, AttributeHandle);
   if (item) { item->set_value (Value); }
}


void
dmz::QtObjectInspector::update_object_acceleration (
      const Handle AttributeHandle,
      const Vector &Value) {

   AttributeItem *item = _state.get_item (ObjectAttrVelocity, AttributeHandle);
   if (item) { item->set_value (Value); }
}


void
dmz::QtObjectInspector::update_object_scale (
      const Handle AttributeHandle,
      const Vector &Value) {

   AttributeItem *item = _state.get_item (ObjectAttrScale, AttributeHandle);
   if (item) { item->set_value (Value); }
}


void
dmz::QtObjectInspector::update_object_vector (
      const Handle AttributeHandle,
      const Vector &Value) {

   AttributeItem *item = _state.get_item (ObjectAttrVector, AttributeHandle);
   if (item) { item->set_value (Value); }
}


void
dmz::QtObjectInspector::update_object_scalar (
      const Handle AttributeHandle,
      const Float64 Value) {

   AttributeItem *item = _state.get_item (ObjectAttrScalar, AttributeHandle);
   if (item) { item->setText (ValueCol, QString::number (Value, 'f', 2)); }
}


void
dmz::QtObjectInspector::update_object_text (
      const Handle AttributeHandle,
      const String &Value) {

   AttributeItem *item = _state.get_item (ObjectAttrText, AttributeHandle);
   if (item) { item->setText (ValueCol, Value.get_buffer ()); }
}


void
dmz::QtObjectInspector::update_object_data (
      const Handle AttributeHandle,
      const Data &Value) {

}


void
dmz::QtObjectInspector::on_treeWidget_itemDoubleClicked (
      QTreeWidgetItem *item,
      int column) {

   ObjectModule *objMod (_state.obs.get_object_module ());
   if (objMod) {
      
      if (item && (column == ValueCol)) {

         _state.attrHandle = _state.defs.lookup_named_handle (
            qPrintable (item->text (AttributeCol)));
            
         const int Type (item->type () - QTreeWidgetItem::UserType);

         switch (Type) {

            case ObjectAttrCounter: {
               
               Int64 value (0);
               if (objMod->lookup_counter (_state.handle, _state.attrHandle, value)) {
                  
                  QInputDialog *dialog = new QInputDialog (this);
                  dialog->setAttribute (Qt::WA_DeleteOnClose);
                  dialog->setWindowTitle (ObjectAttrName[Type]);
                  dialog->setLabelText (item->text (AttributeCol));
                  dialog->setIntRange (-2147483647, 2147483647);
                  dialog->setIntValue (value);
                  dialog->open (this, SLOT (_update_object_counter (int)));
               }
               
               break;
            }
               
            case ObjectAttrCounterMin: {
               Int64 value (0);
               if (objMod->lookup_counter_minimum (_state.handle, _state.attrHandle, value)) {
                  
                  QInputDialog *dialog = new QInputDialog (this);
                  dialog->setAttribute (Qt::WA_DeleteOnClose);
                  dialog->setWindowTitle (ObjectAttrName[Type]);
                  dialog->setLabelText (item->text (AttributeCol));
                  dialog->setIntRange (-2147483647, 2147483647);
                  dialog->setIntValue (value);
                  dialog->open (this, SLOT (_update_object_counter_minimum (int)));
               }

               break;
            }
               
            case ObjectAttrCounterMax: {
               Int64 value (0);
               if (objMod->lookup_counter_maximum (_state.handle, _state.attrHandle, value)) {
                  
                  QInputDialog *dialog = new QInputDialog (this);
                  dialog->setAttribute (Qt::WA_DeleteOnClose);
                  dialog->setWindowTitle (ObjectAttrName[Type]);
                  dialog->setLabelText (item->text (AttributeCol));
                  dialog->setIntRange (-2147483647, 2147483647);
                  dialog->setIntValue (value);
                  dialog->open (this, SLOT (_update_object_counter_maximum (int)));
               }

               break;
            }

            case ObjectAttrState: {
               Mask value;
               if (objMod->lookup_state (_state.handle, _state.attrHandle, value)) {
                  
                  String mask;
                  _state.defs.lookup_state_name (value, mask);
                  
                  QtMaskInputDialog *dialog = new QtMaskInputDialog (this);
                  
                  connect (
                     dialog, SIGNAL (maskChanged (const QString &)),
                     this, SLOT (_update_object_state (const QString &)));
                     
                  dialog->setAttribute (Qt::WA_DeleteOnClose);
                  dialog->setWindowTitle (ObjectAttrName[Type]);
                  dialog->setLabelText (item->text (AttributeCol));
                  dialog->setMaskItems (_state.stateNameList);
                  dialog->setMask (mask.get_buffer ());

                  dialog->open ();
               }
               
               break;
            }

            case ObjectAttrPosition: {
               Vector value (1.0, 2.0, 3.0);
               if (objMod->lookup_position (_state.handle, _state.attrHandle, value)) {
                  
                  QtVectorInputDialog *dialog = new QtVectorInputDialog (this);
                  
                  connect (
                     dialog, SIGNAL (vectorChanged (const Vector &)),
                     this, SLOT (_update_object_position (const Vector &)));
                     
                  dialog->setAttribute (Qt::WA_DeleteOnClose);
                  dialog->setWindowTitle (ObjectAttrName[Type]);
                  dialog->setLabelText (item->text (AttributeCol));
                  dialog->setVector (value);

                  dialog->open ();
               }
            
               break;
            }
            
            case ObjectAttrVelocity: {
               Vector value (1.0, 2.0, 3.0);
               if (objMod->lookup_velocity (_state.handle, _state.attrHandle, value)) {
               
                  QtVectorInputDialog *dialog = new QtVectorInputDialog (this);
               
                  connect (
                     dialog, SIGNAL (vectorChanged (const Vector &)),
                     this, SLOT (_update_object_velocity (const Vector &)));
                  
                  dialog->setAttribute (Qt::WA_DeleteOnClose);
                  dialog->setWindowTitle (ObjectAttrName[Type]);
                  dialog->setLabelText (item->text (AttributeCol));
                  dialog->setVector (value);

                  dialog->open ();
               }
               
               break;
            }
               
            case ObjectAttrAcceleration: {
               Vector value (1.0, 2.0, 3.0);
               if (objMod->lookup_acceleration (_state.handle, _state.attrHandle, value)) {
               
                  QtVectorInputDialog *dialog = new QtVectorInputDialog (this);
               
                  connect (
                     dialog, SIGNAL (vectorChanged (const Vector &)),
                     this, SLOT (_update_object_acceleration (const Vector &)));
                  
                  dialog->setAttribute (Qt::WA_DeleteOnClose);
                  dialog->setWindowTitle (ObjectAttrName[Type]);
                  dialog->setLabelText (item->text (AttributeCol));
                  dialog->setVector (value);

                  dialog->open ();
               }
               
               break;
            }
            
            case ObjectAttrScale: {
               Vector value (1.0, 2.0, 3.0);
               if (objMod->lookup_scale (_state.handle, _state.attrHandle, value)) {
               
                  QtVectorInputDialog *dialog = new QtVectorInputDialog (this);
               
                  connect (
                     dialog, SIGNAL (vectorChanged (const Vector &)),
                     this, SLOT (_update_object_scale (const Vector &)));
                  
                  dialog->setAttribute (Qt::WA_DeleteOnClose);
                  dialog->setWindowTitle (ObjectAttrName[Type]);
                  dialog->setLabelText (item->text (AttributeCol));
                  dialog->setVector (value);

                  dialog->open ();
               }
               
               break;
            }
            
            case ObjectAttrVector: {
               Vector value (1.0, 2.0, 3.0);
               if (objMod->lookup_vector (_state.handle, _state.attrHandle, value)) {
               
                  QtVectorInputDialog *dialog = new QtVectorInputDialog (this);
               
                  connect (
                     dialog, SIGNAL (vectorChanged (const Vector &)),
                     this, SLOT (_update_object_vector (const Vector &)));
                  
                  dialog->setAttribute (Qt::WA_DeleteOnClose);
                  dialog->setWindowTitle (ObjectAttrName[Type]);
                  dialog->setLabelText (item->text (AttributeCol));
                  dialog->setVector (value);

                  dialog->open ();
               }
               
               break;
            }

            case ObjectAttrOrientation: {
               // Orientation value;
               // if (objMod->lookup_orientation (_state.handle, _state.attrHandle, value)) {
               // 
               // }
            
               break;
            }

            case ObjectAttrScalar: {
               Float64 value (0.0);
               if (objMod->lookup_scalar (_state.handle, _state.attrHandle, value)) {
                  
                  QInputDialog *dialog = new QInputDialog (this);
                  dialog->setAttribute (Qt::WA_DeleteOnClose);
                  dialog->setWindowTitle (ObjectAttrName[Type]);
                  dialog->setLabelText (item->text (AttributeCol));
                  dialog->setDoubleRange (-2147483647, 2147483647);
                  dialog->setDoubleDecimals (4);
                  dialog->setDoubleValue (value);
                  dialog->open (this, SLOT (_update_object_scalar (double)));
               }

               break;
            }

            case ObjectAttrText: {
               String value;
               if (objMod->lookup_text (_state.handle, _state.attrHandle, value)) {
                  
                  QInputDialog *dialog = new QInputDialog (this);
                  dialog->setAttribute (Qt::WA_DeleteOnClose);
                  dialog->setWindowTitle (ObjectAttrName[Type]);
                  dialog->setLabelText (item->text (AttributeCol));
                  dialog->setTextValue (value.get_buffer ());
                  dialog->open (this, SLOT (_update_object_text (const QString &)));
               }

               break;
            }

            default:
               break;
         }
      }
   }
}


void
dmz::QtObjectInspector::on_treeWidget_itemChanged (QTreeWidgetItem *item, int column) {

   ObjectModule *objMod (_state.obs.get_object_module ());
   if (objMod && item && (column == ValueCol)) {
      
      const int Type (item->type () - QTreeWidgetItem::UserType);
      if (Type == ObjectAttrFlag) {
         
         Handle attrHandle = _state.defs.lookup_named_handle (
            qPrintable (item->text (AttributeCol)));
         
         if (_state.handle && attrHandle) {

            Boolean flag (item->checkState (ValueCol) == Qt::Checked ? True : False);
            objMod->store_flag (_state.handle, attrHandle, flag);
         }
      }
   }
}


void
dmz::QtObjectInspector::_update_object_counter (int value) {
 
   ObjectModule *objMod (_state.obs.get_object_module ());
   if (objMod) { objMod->store_counter (_state.handle, _state.attrHandle, value); }
}


void
dmz::QtObjectInspector::_update_object_counter_minimum (int value) {
   
   ObjectModule *objMod (_state.obs.get_object_module ());
   if (objMod) { objMod->store_counter_minimum (_state.handle, _state.attrHandle, value); }
}


void
dmz::QtObjectInspector::_update_object_counter_maximum (int value) {
   
   ObjectModule *objMod (_state.obs.get_object_module ());
   if (objMod) { objMod->store_counter_maximum (_state.handle, _state.attrHandle, value); }
}


void
dmz::QtObjectInspector::_update_object_scalar (double value) {
   
   ObjectModule *objMod (_state.obs.get_object_module ());
   if (objMod) { objMod->store_scalar (_state.handle, _state.attrHandle, value); }
}


void
dmz::QtObjectInspector::_update_object_text (const QString &Value) {
   
   ObjectModule *objMod (_state.obs.get_object_module ());
   if (objMod) { objMod->store_text (_state.handle, _state.attrHandle, qPrintable (Value)); }
}


void
dmz::QtObjectInspector::_update_object_position (const Vector &Value) {
   
   ObjectModule *objMod (_state.obs.get_object_module ());
   if (objMod) { objMod->store_position (_state.handle, _state.attrHandle, Value); }
}


void
dmz::QtObjectInspector::_update_object_velocity (const Vector &Value) {
   
   ObjectModule *objMod (_state.obs.get_object_module ());
   if (objMod) { objMod->store_velocity (_state.handle, _state.attrHandle, Value); }
}


void
dmz::QtObjectInspector::_update_object_acceleration (const Vector &Value) {
   
   ObjectModule *objMod (_state.obs.get_object_module ());
   if (objMod) { objMod->store_acceleration (_state.handle, _state.attrHandle, Value); }
}


void
dmz::QtObjectInspector::_update_object_scale (const Vector &Value) {
   
   ObjectModule *objMod (_state.obs.get_object_module ());
   if (objMod) { objMod->store_scale (_state.handle, _state.attrHandle, Value); }
}


void
dmz::QtObjectInspector::_update_object_vector (const Vector &Value) {
   
   ObjectModule *objMod (_state.obs.get_object_module ());
   if (objMod) { objMod->store_vector (_state.handle, _state.attrHandle, Value); }
}


void
dmz::QtObjectInspector::_update_object_state (const QString &Value) {
   
   String maskStr (qPrintable (Value));
   Mask mask;
   
   _state.defs.lookup_state (maskStr, mask);
   
   ObjectModule *objMod (_state.obs.get_object_module ());
   if (objMod) { objMod->store_state (_state.handle, _state.attrHandle, mask); }
}


void
dmz::QtObjectInspector::closeEvent (QCloseEvent *event) {

   Q_EMIT finished (_state.handle);
}


void
dmz::QtObjectInspector::_init () {

   _state.ui.setupUi (this);
   
   QStringList labels;
   labels << "Property" << "Value";
   _state.ui.treeWidget->setHeaderLabels (labels);
   
   QHeaderView *header (_state.ui.treeWidget->header ());
   if (header) {
   
      header->setResizeMode (0, QHeaderView::ResizeToContents);
      header->setResizeMode (1, QHeaderView::ResizeToContents);
   }
}
