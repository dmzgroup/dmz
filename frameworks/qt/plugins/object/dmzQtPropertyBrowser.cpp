#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzQtConfigRead.h>
#include "dmzQtPropertyBrowser.h"
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


dmz::QtPropertyBrowser::QtPropertyBrowser (
      const Handle ObjHandle,
      ObjectObserverUtil &observer,
      RuntimeContext *context,
      QWidget *parent) :
      QFrame (parent),
      _Handle (ObjHandle),
      _obs (observer),
      _log ("QtPropertyBrowser", context),
      _defs (context, &_log),
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
      _idToExpanded () {

   _init ();
}


dmz::QtPropertyBrowser::~QtPropertyBrowser () {

}


// Object Observer Interface
void
dmz::QtPropertyBrowser::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (ObjectHandle == _Handle) {
      
      QtVariantProperty *property =
         _variantManagerRO->addProperty (QVariant::String, HandleName);

      property->setValue (_handle_to_string (ObjectHandle));
      
      _add_property (property, HandleName);
      
      property = _variantManagerRO->addProperty (QVariant::String, ObjectTypeName);
         
      property->setValue (_type_to_string (Type));
      
      _add_property (property, ObjectTypeName);
   }
}


void
dmz::QtPropertyBrowser::update_object_uuid (
      const Handle ObjectHandle,
      const UUID &Identity,
      const UUID &PrevIdentity) {

   if (ObjectHandle == _Handle) {
      
      QtVariantProperty *property =
         _variantManager->addProperty (QVariant::String, UUIDName);
      
      property->setValue (_uuid_to_string (Identity));

      _add_property (property, UUIDName);
   }
}


void
dmz::QtPropertyBrowser::remove_object_attribute (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &AttrMask) {

}


void
dmz::QtPropertyBrowser::update_object_locality (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectLocalityEnum Locality,
      const ObjectLocalityEnum PrevLocality) {

   if (ObjectHandle == _Handle) {
      
      QtProperty *property = _enumManager->addProperty (LocalityName);

      QStringList enumNames;
      enumNames << "Unknown" << "Local" << "Remote";
         
      _enumManager->setEnumNames(property, enumNames);
      _enumManager->setValue (property, Locality);
      
      _add_property (property, LocalityName);
   }
}


void
dmz::QtPropertyBrowser::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

}


void
dmz::QtPropertyBrowser::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

}


void
dmz::QtPropertyBrowser::update_link_attribute_object (
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
dmz::QtPropertyBrowser::update_object_counter (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {
         
   if (ObjectHandle == _Handle) {

      _add_int64_property (CounterName, AttributeHandle, Value);
   }
}


void
dmz::QtPropertyBrowser::update_object_counter_minimum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   if (ObjectHandle == _Handle) {

      _add_int64_property (CounterMinimumName, AttributeHandle, Value);
   }
}


void
dmz::QtPropertyBrowser::update_object_counter_maximum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   if (ObjectHandle == _Handle) {

      _add_int64_property (CounterMaximumName, AttributeHandle, Value);
   }
}


void
dmz::QtPropertyBrowser::update_object_alternate_type (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const ObjectType &Value,
      const ObjectType *PreviousValue) {

   if (ObjectHandle == _Handle) {

      _add_string_property (AlternateTypeName, AttributeHandle, _type_to_string (Value));
   }
}


void
dmz::QtPropertyBrowser::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   if (ObjectHandle == _Handle) {

      String name;
      _defs.lookup_state_name (Value, name);
      
      _add_string_property (StateName, AttributeHandle, QLatin1String (name.get_buffer ()));
   }
}


void
dmz::QtPropertyBrowser::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (ObjectHandle == _Handle) {
   
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
dmz::QtPropertyBrowser::update_object_time_stamp (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 &Value,
      const Float64 *PreviousValue) {

   if (ObjectHandle == _Handle) {

      _add_float64_property (TimeStampName, AttributeHandle, Value);
   }
}


void
dmz::QtPropertyBrowser::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (ObjectHandle == _Handle) {

_log.warn << "update_object_position[" << ObjectHandle << ":" << AttributeHandle << "]: " << Value << endl;
      _update_vector_property (PositionName, AttributeHandle, Value);
   }
}


void
dmz::QtPropertyBrowser::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

   if (ObjectHandle == _Handle) {

//      _update_vector_property (PositionName, AttributeHandle, Value);
   }
}


void
dmz::QtPropertyBrowser::update_object_velocity (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (ObjectHandle == _Handle) {

      _update_vector_property (VelocityName, AttributeHandle, Value);
   }
}


void
dmz::QtPropertyBrowser::update_object_acceleration (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (ObjectHandle == _Handle) {

      _update_vector_property (AccelerationName, AttributeHandle, Value);
   }
}


void
dmz::QtPropertyBrowser::update_object_scale (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (ObjectHandle == _Handle) {

      _update_vector_property (ScaleName, AttributeHandle, Value);
   }
}


void
dmz::QtPropertyBrowser::update_object_vector (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (ObjectHandle == _Handle) {

      _add_vector_property (VectorName, AttributeHandle, Value);
   }
}


void
dmz::QtPropertyBrowser::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   if (ObjectHandle == _Handle) {

      _add_float64_property (ScalarName, AttributeHandle, Value);
   }
}


void
dmz::QtPropertyBrowser::update_object_text (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const String &Value,
      const String *PreviousValue) {

   if (ObjectHandle == _Handle) {

      _add_string_property (TextName, AttributeHandle, QLatin1String (Value.get_buffer ()));
   }
}


void
dmz::QtPropertyBrowser::update_object_data (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Data &Value,
      const Data *PreviousValue) {

}


void
dmz::QtPropertyBrowser::_value_changed (QtProperty *property, const QVariant &value) {
   
}


// void
// dmz::QtPropertyBrowser::_clear_properties () {
// 
//    QMap<QtProperty *, QString>::ConstIterator itProp = _propertyToId.constBegin ();
//       
//    while (itProp != _propertyToId.constEnd ()) {
//          
//        delete itProp.key ();
//        itProp++;
//    }
//       
//    _propertyToId.clear ();
//    _idToProperty.clear ();
// }


QtProperty *
dmz::QtPropertyBrowser::_lookup_group_property (const QString &Name) {

   QtProperty *group (_idToProperty[Name]);

   if (!group) {

      group = _groupManager->addProperty (Name);
      _add_property (group, Name);
//      _ui.propertyEditor->setExpanded (item, False);
   }

   return group;
}


void
dmz::QtPropertyBrowser::_add_int64_property (
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

      _add_property (property, Name, False);
   }
}


void
dmz::QtPropertyBrowser::_add_float64_property (
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

      _add_property (property, Name, False);
   }
}


void
dmz::QtPropertyBrowser::_add_string_property (
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

      _add_property (property, Name, False);
   }
}


void
dmz::QtPropertyBrowser::_add_vector_property (
      const QString &GroupName,
      const Handle AttributeHandle,
      const Vector &Value) {

   QtProperty *group (_lookup_group_property (GroupName));

   if (group) {

      const QString Name (_handle_to_name (AttributeHandle));

      QtProperty *property (_vectorManager->addProperty (Name));
      _vectorManager->setValue (property, Value);

      group->addSubProperty (property);

      _add_property (property, Name, False);
   }
}


void
dmz::QtPropertyBrowser::_update_vector_property (
      const QString &GroupName,
      const Handle AttributeHandle,
      const Vector &Value) {

   if (_vectorManager) {
      
      QtProperty *group (_lookup_group_property (GroupName));

      if (group) {

         const QString Name (_handle_to_name (AttributeHandle));

         QtProperty *property (_idToProperty[Name]);

         if (!property) {

            property = _vectorManager->addProperty (Name);

            group->addSubProperty (property);

            _add_property (property, Name, False);
         }

         if (property) {

            _vectorManager->setValue (property, Value);
         }
      }
   }
}


void
dmz::QtPropertyBrowser::_add_property (
      QtProperty *property,
      const QString &Id,
      const Boolean AddToTree) {
   
   _propertyToId[property] = Id;
   _idToProperty[Id] = property;
   
   if (AddToTree) {
      
      QtBrowserItem *item = _ui.propertyEditor->addProperty (property);

      if (_idToExpanded.contains (Id)) {
      
          _ui.propertyEditor->setExpanded (item, _idToExpanded[Id]);
      }

      _ui.propertyEditor->setExpanded (item, False);
   }
}


void
dmz::QtPropertyBrowser::_update_expand_state () {
   
    QList<QtBrowserItem *> itemList = _ui.propertyEditor->topLevelItems ();
    
    QListIterator<QtBrowserItem *> it (itemList);
    
    while (it.hasNext ()) {
       
        QtBrowserItem *item = it.next ();
        QtProperty *prop = item->property ();
        _idToExpanded[_propertyToId[prop]] = _ui.propertyEditor->isExpanded (item);
    }
}


QString
dmz::QtPropertyBrowser::_uuid_to_string (const UUID &Identity) {

   return QLatin1String (Identity.to_string ().get_buffer ());
}


QString
dmz::QtPropertyBrowser::_type_to_string (const ObjectType &Type) {

   return QLatin1String (Type.get_name ().get_buffer ());
}


QString
dmz::QtPropertyBrowser::_handle_to_name (const Handle Object) {

   return QLatin1String (_defs.lookup_named_handle_name (Object).get_buffer ());
}


QString
dmz::QtPropertyBrowser::_handle_to_string (const Handle Object) {

   String str;
   str << Object;
   return QLatin1String (str.get_buffer ());
}


void
dmz::QtPropertyBrowser::_init () {

   // QString name ("%1-%2");
   // name.arg (_obs.get_plugin_name ().get_buffer (), _Handle);
   // setObjectName (name);

   _ui.setupUi (this);
   
   _defaultAttrHandle = _obs.activate_default_object_attribute (
       ObjectCreateMask | ObjectDestroyMask);

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
