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
#include <QtCore/QMap>
#include <qteditorfactory.h>
#include <qtpropertymanager.h>
#include <qttreepropertybrowser.h>
#include <qtvariantproperty.h>
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
};


class dmz::QtObjectInspector::GroupStruct {

   public:
      GroupStruct (const QString &Name) : _Name (Name), _property (0) {;}

      ~GroupStruct () {;}

      void add_property (QtProperty *property) { _property = property; }
      void add_property (const dmz::Handle AttrHandle, QtProperty *property) {

         _handleMap[property] = AttrHandle;
         _propertyMap[AttrHandle] = property;
         if (_property) { _property->addSubProperty (property); }
      }

      QtProperty *get_property () { return _property; }
      QtProperty *get_property (const Handle AttrHandle) { return _propertyMap[AttrHandle]; }
      Handle get_handle (QtProperty *property) { return _handleMap[property]; }

   protected:
      const QString _Name;
      QtProperty *_property;
      QMap<QtProperty *, dmz::Handle> _handleMap;
      QMap<dmz::Handle, QtProperty *> _propertyMap;
};


struct dmz::QtObjectInspector::State {

   const Handle ObjHandle;
   ObjectObserverUtil &obs;
   Log log;
   Definitions defs;
   Ui::ObjectInspectorForm ui;
   QtGroupPropertyManager *groupManager;
   QtEnumPropertyManager *enumManager;
   QtEnumPropertyManager *enumManagerRO;
   QtVariantPropertyManager *variantManager;
   QtVariantPropertyManager *variantManagerRO;
   VectorPropertyManager *vectorManager;
   VectorPropertyManager *vectorManagerRO;
   QtVariantProperty *handleProperty;
   QtVariantProperty *typeProperty;
   QtProperty *localityProperty;
   QtVariantProperty *uuidProperty;
   QMap<QtProperty *, QString> propertyToGroupMap;
   QMap<QtProperty *, Handle> propertyToHandleMap;
   QMap<QString, GroupStruct *> groupMap;
   Boolean ignoreUpdates;
   Boolean ignoreValueChanged;
   

   State (const Handle TheHandle, ObjectObserverUtil &theObs, RuntimeContext *theContext) :
         ObjHandle (TheHandle),
         obs (theObs),
         log ("QtObjectInspector", theContext),
         defs (theContext, &log),
         handleProperty (0),
         typeProperty (0),
         localityProperty (0),
         uuidProperty (0),
         ignoreUpdates (False),
         ignoreValueChanged (False) {;}

   ~State () {;}

   void add_property (const QString &GroupName, const Handle AttrHandle, QtProperty *property) {

      propertyToGroupMap[property] = GroupName;
      propertyToHandleMap[property] = AttrHandle;
   }

   GroupStruct *get_group (const QString &GroupName);

   void update_variant_property (
      const QString &GroupName,
      const Handle AttrHandle,
      const int PropertyType,
      const QVariant Value);

   void update_vector_property (
      const QString &GroupName,
      const Handle AttrHandle,
      const Vector &Value);

   QString handle_to_name (const Handle Object) {

      return QLatin1String (defs.lookup_named_handle_name (Object).get_buffer ());
   }
};


dmz::QtObjectInspector::GroupStruct *
dmz::QtObjectInspector::State::get_group (const QString &GroupName) {

   GroupStruct *group (groupMap[GroupName]);
   if (!group && groupManager) {

      group = new GroupStruct (GroupName);
      group->add_property (groupManager->addProperty (GroupName));
      groupMap[GroupName] = group;
      ui.propertyEditor->addProperty (group->get_property ());
   }

   return group;
}


void
dmz::QtObjectInspector::State::update_variant_property (
      const QString &GroupName,
      const Handle AttrHandle,
      const int PropertyType,
      const QVariant Value) {

   if (variantManager) {

      ignoreValueChanged = true;
      
      GroupStruct *group (get_group (GroupName));
      if (group) {

         QtProperty *property (group->get_property (AttrHandle));
         if (!property) {

            const QString AttrName (handle_to_name (AttrHandle));
            property = variantManager->addProperty (PropertyType, AttrName);
            group->add_property (AttrHandle, property);
            add_property (GroupName, AttrHandle, property);
         }

         if (property) { variantManager->setValue (property, Value); }
      }
      
      ignoreValueChanged = false;
   }
}


void
dmz::QtObjectInspector::State::update_vector_property (
      const QString &GroupName,
      const Handle AttrHandle,
      const Vector &Value) {

   if (vectorManager) {

      ignoreValueChanged = true;

      GroupStruct *group (get_group (GroupName));
      if (group) {

         QtProperty *property ( group->get_property (AttrHandle));
         if (!property) {

            const QString AttrName (handle_to_name (AttrHandle));
            property = vectorManager->addProperty (AttrName);
            group->add_property (AttrHandle, property);
            add_property (GroupName, AttrHandle, property);
         }

         if (property) { vectorManager->setValue (property, Value); }
      }
   }
   
   ignoreValueChanged = false;
}


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


// Object Observer Interface
void
dmz::QtObjectInspector::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (ObjectHandle == _state.ObjHandle && !_state.ignoreUpdates) {

      if (_state.variantManagerRO) {

         const QString HandleStr (to_qstring (ObjectHandle));
         setWindowTitle (tr ("%1 (%2)").arg (windowTitle ()).arg (HandleStr));

         _state.handleProperty =
            _state.variantManagerRO->addProperty (QVariant::String, HandleName);

         _state.handleProperty->setValue (HandleStr);
         _state.ui.propertyEditor->addProperty (_state.handleProperty);

         _state.typeProperty =
            _state.variantManagerRO->addProperty (QVariant::String, ObjectTypeName);

         _state.typeProperty->setValue (to_qstring (Type));
         _state.ui.propertyEditor->addProperty (_state.typeProperty);

         update_object_locality (Identity, ObjectHandle, Locality, Locality);
      }
   }
}


void
dmz::QtObjectInspector::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   if (ObjectHandle == _state.ObjHandle && !_state.ignoreUpdates) {

      QMap<QtProperty *, Handle>::ConstIterator itProp =
         _state.propertyToHandleMap.constBegin ();

      while (itProp != _state.propertyToHandleMap.constEnd ()) {

         itProp.key ()->setEnabled (False);
         itProp++;
      }

      _state.handleProperty->setEnabled (False);
      _state.typeProperty->setEnabled (False);
      _state.localityProperty->setEnabled (False);
      _state.uuidProperty->setEnabled (False);
      
      _state.ui.addAttributeButton->setEnabled (False);
      _state.ui.removeAttributeButton->setEnabled (False);
      
      _state.ignoreUpdates = true;
      _state.ignoreValueChanged = true;
   }
}


void
dmz::QtObjectInspector::update_object_uuid (
      const Handle ObjectHandle,
      const UUID &Identity,
      const UUID &PrevIdentity) {

   if (ObjectHandle ==  _state.ObjHandle && !_state.ignoreUpdates) {

      QtVariantProperty *property = _state.uuidProperty;

      if (!property && _state.variantManagerRO) {

         property =  _state.variantManagerRO->addProperty (QVariant::String, UUIDName);
         _state.uuidProperty = property;
         _state.ui.propertyEditor->addProperty (property);
      }

      if (property) { property->setValue (to_qstring (Identity)); }
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

      if (_state.enumManagerRO) {

         QtProperty *property = _state.localityProperty;

         if (!property) {

            property = _state.enumManagerRO->addProperty (LocalityName);
            _state.localityProperty = property;
            _state.ui.propertyEditor->addProperty (property);

            QStringList enumNames;
            enumNames << "Unknown" << "Local" << "Remote";
            _state.enumManagerRO->setEnumNames (property, enumNames);
         }

         if (property) {

            _state.enumManagerRO->setValue (property, Locality);
         }
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

      _state.update_variant_property (
         CounterName, AttributeHandle, QVariant::Int, Value);
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

      _state.update_variant_property (
         CounterMinimumName, AttributeHandle, QVariant::Int, Value);
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

      _state.update_variant_property (
         CounterMaximumName, AttributeHandle, QVariant::Int, Value);
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

      _state.update_variant_property (
         AlternateTypeName, AttributeHandle, QVariant::String, to_qstring (Value));
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

      String name;
      _state.defs.lookup_state_name (Value, name);

      _state.update_variant_property (
         StateName,
         AttributeHandle,
         QVariant::String,
         QLatin1String (name.get_buffer ()));
   }
}


void
dmz::QtObjectInspector::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (ObjectHandle ==  _state.ObjHandle && !_state.ignoreUpdates) {

      _state.update_variant_property (
         FlagName, AttributeHandle, QVariant::Bool, Value);
   }
}


void
dmz::QtObjectInspector::update_object_time_stamp (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   if (ObjectHandle ==  _state.ObjHandle && !_state.ignoreUpdates) {

      _state.update_variant_property (
         TimeStampName, AttributeHandle, QVariant::Double, Value);
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

       _state.update_vector_property (PositionName, AttributeHandle, Value);
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

      _state.update_vector_property (VelocityName, AttributeHandle, Value);
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

      _state.update_vector_property (AccelerationName, AttributeHandle, Value);
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

      _state.update_vector_property (ScaleName, AttributeHandle, Value);
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

      _state.update_vector_property (VectorName, AttributeHandle, Value);
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

      _state.update_variant_property (
         ScalarName, AttributeHandle, QVariant::Double, Value);
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

      _state.update_variant_property (
         TextName,
         AttributeHandle,
         QVariant::String,
         QLatin1String (Value.get_buffer ()));
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
      else if (Group == StateName) {

         Mask state;
         const String Text (qPrintable (Value.toString ()));
         if (_state.defs.lookup_state (Text, state)) {

            objMod->store_state (_state.ObjHandle, Attr, state);
         }
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


void
dmz::QtObjectInspector::closeEvent (QCloseEvent *event) {

   Q_EMIT finished (_state.ObjHandle);
}


void
dmz::QtObjectInspector::_init () {

   // QString name ("%1-%2");
   // name.arg (_obs.get_plugin_name ().get_buffer (),  _state.ObjHandle);
   // setObjectName (name);

   _state.ui.setupUi (this);

   _state.groupManager = new QtGroupPropertyManager (this);
   _state.variantManager = new QtVariantPropertyManager (this);
   _state.variantManagerRO = new QtVariantPropertyManager (this);
   _state.enumManager = new QtEnumPropertyManager (this);
   _state.enumManagerRO = new QtEnumPropertyManager (this);
   _state.vectorManager = new VectorPropertyManager (this);
   _state.vectorManagerRO = new VectorPropertyManager (this);

   connect (
      _state.variantManager, SIGNAL (valueChanged (QtProperty *, const QVariant &)),
      this, SLOT (_value_changed (QtProperty *, const QVariant &)));

   connect (
      _state.vectorManager, SIGNAL (valueChanged (QtProperty *, const Vector &)),
      this, SLOT (_value_changed (QtProperty *, const Vector &)));

   QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory (this);
   QtEnumEditorFactory *comboBoxFactory = new QtEnumEditorFactory(this);
   QtDoubleSpinBoxFactory *doubleSpinBoxFactory = new QtDoubleSpinBoxFactory(this);

   _state.ui.propertyEditor->setFactoryForManager ( _state.variantManager, variantFactory);
   _state.ui.propertyEditor->setFactoryForManager (_state.enumManager, comboBoxFactory);

   _state.ui.propertyEditor->setFactoryForManager (
      _state.vectorManager->subDoublePropertyManager (), doubleSpinBoxFactory);
}
