#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzQtConfigRead.h>
#include "dmzQtPluginPropertyBrowser.h"
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesUUID.h>
#include <qteditorfactory.h>
#include <qtpropertymanager.h>
#include <qttreepropertybrowser.h>
#include <qtvariantproperty.h>

namespace {
   
   const dmz::UInt32 HandleRole (Qt::UserRole + 1);
   const dmz::UInt32 HandleCol (0);
   const dmz::UInt32 TypeCol (1);
   
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
};


dmz::QtPluginPropertyBrowser::QtPluginPropertyBrowser (
      const PluginInfo &Info,
      Config &local) :
      QFrame (0),
      QtWidget (Info),
      Plugin (Info),
      MessageObserver (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defs (Info, &_log),
      _defaultAttrHandle (0),
      _groupManager (0),
      _variantManager (0),
      _variantManagerRO (0),
      _enumManager (0),
      _enumManagerRO (0),
      _vectorManager (0),
      _vectorManagerRO (0),
      _propertyToId (),
      _idToProperty (),
      _idToExpanded (),
      _currentItem (0),
      _currentObject (0) {

   _init (local);
}


dmz::QtPluginPropertyBrowser::~QtPluginPropertyBrowser () {

}


// QtWidget Interface
QWidget *
dmz::QtPluginPropertyBrowser::get_qt_widget () { return this; }


// Plugin Interface
void
dmz::QtPluginPropertyBrowser::update_plugin_state (
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
dmz::QtPluginPropertyBrowser::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Message Observer Interface
void
dmz::QtPluginPropertyBrowser::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

}


// Object Observer Interface
void
dmz::QtPluginPropertyBrowser::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (ObjectHandle == _currentObject) {
      
      QtVariantProperty *property =
         _variantManagerRO->addProperty (QVariant::String, HandleName);

      property->setValue (_handle_to_string (ObjectHandle));
      
      _add_property (property, HandleName);
      
      property = _variantManagerRO->addProperty (QVariant::String, ObjectTypeName);
         
      property->setValue (_type_to_string (Type));
      
      _add_property (property, ObjectTypeName);
   }
   else {

      QTreeWidgetItem *item = new QTreeWidgetItem;

      item->setText (HandleCol, _handle_to_string (ObjectHandle));
      item->setData (HandleCol, HandleRole, (quint64)ObjectHandle);
      item->setText (TypeCol, _type_to_string (Type));

      _ui.objectTreeWidget->addTopLevelItem (item);

//      if (_ui.objectTreeWidget->topLevelItemCount () == 1) {

         _ui.objectTreeWidget->setCurrentItem (item);
//      }
   }
}


void
dmz::QtPluginPropertyBrowser::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

}


void
dmz::QtPluginPropertyBrowser::update_object_uuid (
      const Handle ObjectHandle,
      const UUID &Identity,
      const UUID &PrevIdentity) {

   if (ObjectHandle == _currentObject) {
      
      QtVariantProperty *property =
         _variantManager->addProperty (QVariant::String, UUIDName);
      
      property->setValue (_uuid_to_string (Identity));

      _add_property (property, UUIDName);
   }
}


void
dmz::QtPluginPropertyBrowser::remove_object_attribute (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &AttrMask) {

}


void
dmz::QtPluginPropertyBrowser::update_object_locality (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectLocalityEnum Locality,
      const ObjectLocalityEnum PrevLocality) {

   if (ObjectHandle == _currentObject) {
      
      QtProperty *property = _enumManager->addProperty (LocalityName);

      QStringList enumNames;
      enumNames << "Unknown" << "Local" << "Remote";
         
      _enumManager->setEnumNames(property, enumNames);
      _enumManager->setValue (property, Locality);
      
      _add_property (property, LocalityName);
   }
}


void
dmz::QtPluginPropertyBrowser::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

}


void
dmz::QtPluginPropertyBrowser::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

}


void
dmz::QtPluginPropertyBrowser::update_link_attribute_object (
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
dmz::QtPluginPropertyBrowser::update_object_counter (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {
         
   if (ObjectHandle == _currentObject) {

      _add_int64_property (CounterName, AttributeHandle, Value);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_counter_minimum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   if (ObjectHandle == _currentObject) {

      _add_int64_property (CounterMinimumName, AttributeHandle, Value);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_counter_maximum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   if (ObjectHandle == _currentObject) {

      _add_int64_property (CounterMaximumName, AttributeHandle, Value);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_alternate_type (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const ObjectType &Value,
      const ObjectType *PreviousValue) {

   if (ObjectHandle == _currentObject) {

      _add_string_property (AlternateTypeName, AttributeHandle, _type_to_string (Value));
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   if (ObjectHandle == _currentObject) {

      String name;
      _defs.lookup_state_name (Value, name);
      
      _add_string_property (StateName, AttributeHandle, QLatin1String (name.get_buffer ()));
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (ObjectHandle == _currentObject) {
   
      QtProperty *group (_lookup_group_property (FlagName));
   
      if (group) {

         const QString Name (_handle_to_name (AttributeHandle));

         QtVariantProperty *property = _variantManager->addProperty (QVariant::Bool, Name);
         
         property->setValue (Value);

         group->addSubProperty (property);
      }
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_time_stamp (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 &Value,
      const Float64 *PreviousValue) {

   if (ObjectHandle == _currentObject) {

      _add_float64_property (TimeStampName, AttributeHandle, Value);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (ObjectHandle == _currentObject) {

      _add_vector_property (PositionName, AttributeHandle, Value);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

   if (ObjectHandle == _currentObject) {

//      _add_vector_property (PositionName, AttributeHandle, Value);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_velocity (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (ObjectHandle == _currentObject) {

      _add_vector_property (VelocityName, AttributeHandle, Value);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_acceleration (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (ObjectHandle == _currentObject) {

      _add_vector_property (AccelerationName, AttributeHandle, Value);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_scale (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (ObjectHandle == _currentObject) {

      _add_vector_property (ScaleName, AttributeHandle, Value);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_vector (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (ObjectHandle == _currentObject) {

      _add_vector_property (VectorName, AttributeHandle, Value);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   if (ObjectHandle == _currentObject) {

      _add_float64_property (ScalarName, AttributeHandle, Value);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_text (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const String &Value,
      const String *PreviousValue) {

   if (ObjectHandle == _currentObject) {

      _add_string_property (TextName, AttributeHandle, QLatin1String (Value.get_buffer ()));
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_data (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Data &Value,
      const Data *PreviousValue) {

}


void
dmz::QtPluginPropertyBrowser::on_objectTreeWidget_currentItemChanged (
      QTreeWidgetItem *current,
      QTreeWidgetItem *previous) {

   ObjectModule *objMod (get_object_module ());
   _currentObject = _item_to_handle (current);
      
   if (_currentObject && objMod) {

      _currentItem = current;
      
      _update_expand_state ();
      
      QMap<QtProperty *, QString>::ConstIterator itProp = _propertyToId.constBegin ();
      
      while (itProp != _propertyToId.constEnd ()) {
         
          delete itProp.key();
          itProp++;
      }
      
      _propertyToId.clear();
      _idToProperty.clear();

      objMod->dump_all_object_attributes (_currentObject, *this);
   }
}


void
dmz::QtPluginPropertyBrowser::_value_changed (QtProperty *property, const QVariant &value) {
   
}


QtProperty *
dmz::QtPluginPropertyBrowser::_lookup_group_property (const QString &Name) {

   QtProperty *group (_idToProperty[Name]);

   if (!group) {

      group = _groupManager->addProperty (Name);
      _add_property (group, Name);
   }

   return group;
}


void
dmz::QtPluginPropertyBrowser::_add_int64_property (
      const QString &GroupName,
      const Handle AttributeHandle,
      const Int64 Value) {

   QtProperty *group (_lookup_group_property (GroupName));

   if (group) {

      const QString Name (_handle_to_name (AttributeHandle));

      QtVariantProperty *property =
         _variantManager->addProperty (QVariant::LongLong, Name);

      property->setValue (Value);

      group->addSubProperty (property);
   }
}


void
dmz::QtPluginPropertyBrowser::_add_float64_property (
      const QString &GroupName,
      const Handle AttributeHandle,
      const Float64 Value) {

   QtProperty *group (_lookup_group_property (GroupName));

   if (group) {

      const QString Name (_handle_to_name (AttributeHandle));

      QtVariantProperty *property =
         _variantManager->addProperty (QVariant::Double, Name);

      property->setValue (Value);

      group->addSubProperty (property);
   }
}


void
dmz::QtPluginPropertyBrowser::_add_string_property (
      const QString &GroupName,
      const Handle AttributeHandle,
      const QString &Value) {

   QtProperty *group (_lookup_group_property (GroupName));

   if (group) {

      const QString Name (_handle_to_name (AttributeHandle));

      QtVariantProperty *property =
         _variantManager->addProperty (QVariant::String, Name);

      property->setValue (Value);

      group->addSubProperty (property);
   }
}


void
dmz::QtPluginPropertyBrowser::_add_vector_property (
      const QString &GroupName,
      const Handle AttributeHandle,
      const Vector &Value) {

   QtProperty *group (_lookup_group_property (GroupName));

   if (group) {

      const QString Name (_handle_to_name (AttributeHandle));

      QtProperty *property (_vectorManager->addProperty (Name));
      _vectorManager->setValue (property, Value);

      group->addSubProperty (property);
   }
}


void
dmz::QtPluginPropertyBrowser::_add_property (QtProperty *property, const QString &id) {
   
   _propertyToId[property] = id;
   _idToProperty[id] = property;
   
   QtBrowserItem *item = _ui.propertyEditor->addProperty (property);
   
   if (_idToExpanded.contains (id)) {
      
      _ui.propertyEditor->setExpanded(item, _idToExpanded[id]);
   }
}


void
dmz::QtPluginPropertyBrowser::_update_expand_state () {
   
    QList<QtBrowserItem *> list = _ui.propertyEditor->topLevelItems ();
    
    QListIterator<QtBrowserItem *> it (list);
    
    while (it.hasNext ()) {
       
        QtBrowserItem *item = it.next ();
        QtProperty *prop = item->property ();
        _idToExpanded[_propertyToId[prop]] = _ui.propertyEditor->isExpanded (item);
    }
}


QString
dmz::QtPluginPropertyBrowser::_uuid_to_string (const UUID &Identity) {

   return QLatin1String (Identity.to_string ().get_buffer ());
}


QString
dmz::QtPluginPropertyBrowser::_type_to_string (const ObjectType &Type) {

   return QLatin1String (Type.get_name ().get_buffer ());
}


QString
dmz::QtPluginPropertyBrowser::_handle_to_name (const Handle Object) {

   return QLatin1String (_defs.lookup_named_handle_name (Object).get_buffer ());
}


QString
dmz::QtPluginPropertyBrowser::_handle_to_string (const Handle Object) {

   String str;
   str << Object;
   return QLatin1String (str.get_buffer ());
}


dmz::Handle
dmz::QtPluginPropertyBrowser::_item_to_handle (QTreeWidgetItem *item) {

   Handle retVal (0);
   if (item) { retVal = item->data (HandleCol, HandleRole).toULongLong (); }
   return retVal;
}


void
dmz::QtPluginPropertyBrowser::_init (Config &local) {

   setObjectName (get_plugin_name ().get_buffer ());

   _ui.setupUi (this);
   
   qframe_config_read ("frame", local, this);

   _defaultAttrHandle = activate_default_object_attribute (
      ObjectCreateMask | ObjectDestroyMask);

   _ui.splitter->setStretchFactor (0, 0);
   _ui.splitter->setStretchFactor (1, 1);
   
   QStringList headerList;
   headerList << "Handle" << "ObjectType";
   _ui.objectTreeWidget->setHeaderLabels (headerList);
   
   _groupManager = new QtGroupPropertyManager (this);
   _variantManager = new QtVariantPropertyManager (this);
   _variantManagerRO = new QtVariantPropertyManager (this);
   _enumManager = new QtEnumPropertyManager (this);
   _enumManagerRO = new QtEnumPropertyManager (this);
   _vectorManager = new VectorPropertyManager (this);
   _vectorManagerRO = new VectorPropertyManager (this);

   connect (
      _variantManager, SIGNAL (valueChanged (QtProperty *, const QVariant &)),
      this, SLOT (_value_changed (QtProperty *, const QVariant &)));

   QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory (this);
   QtEnumEditorFactory *comboBoxFactory = new QtEnumEditorFactory(this);
   QtDoubleSpinBoxFactory *doubleSpinBoxFactory = new QtDoubleSpinBoxFactory(this);

   _ui.propertyEditor->setFactoryForManager (_variantManager, variantFactory);
   _ui.propertyEditor->setFactoryForManager (_enumManager, comboBoxFactory);
   
   _ui.propertyEditor->setFactoryForManager (
      _vectorManager->subDoublePropertyManager (), doubleSpinBoxFactory);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginPropertyBrowser (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginPropertyBrowser (Info, local);
}

};
