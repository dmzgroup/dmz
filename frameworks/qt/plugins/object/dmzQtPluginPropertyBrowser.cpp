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
      _variantManager (0),
      _enumManager (0),
      _groupManager (0),
      _vectorManager (0),
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
      
      const QLatin1String Name ("Handle");
      QtVariantProperty *property;
      
      property = _variantManagerRO->addProperty (QVariant::String, Name);

      String handlAsString;
      handlAsString << ObjectHandle;
      property->setValue (handlAsString.get_buffer ());
      
      _add_property (property, Name);
   }
   else {
      
      QTreeWidgetItem *item = new QTreeWidgetItem;

      String handleAsString;
      handleAsString << ObjectHandle;

      item->setText (HandleCol, handleAsString.get_buffer ());
      item->setData (HandleCol, HandleRole, (quint64)ObjectHandle);
      item->setText (TypeCol, Type.get_name ().get_buffer ());

      _ui.objectTreeWidget->addTopLevelItem (item);

      if (_ui.objectTreeWidget->topLevelItemCount () == 1) {

         _ui.objectTreeWidget->setCurrentItem (item);
      }
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
      
      const QLatin1String Name ("UUID");
      QtVariantProperty *property;
      
      property = _variantManagerRO->addProperty (QVariant::String, Name);
      property->setValue (Identity.to_string ().get_buffer ());

      _add_property (property, Name);
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
      
      const QLatin1String Name ("Locality");
      QtProperty *property;

      property = _enumManager->addProperty (Name);

      QStringList enumNames;
      enumNames << "Unknown" << "Local" << "Remote";
         
      _enumManager->setEnumNames(property, enumNames);
      _enumManager->setValue (property, Locality);
      
      _add_property (property, Name);
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

}


void
dmz::QtPluginPropertyBrowser::update_object_counter_minimum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

}


void
dmz::QtPluginPropertyBrowser::update_object_counter_maximum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

}


void
dmz::QtPluginPropertyBrowser::update_object_alternate_type (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const ObjectType &Value,
      const ObjectType *PreviousValue) {

}


void
dmz::QtPluginPropertyBrowser::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

}


void
dmz::QtPluginPropertyBrowser::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (ObjectHandle == _currentObject) {
   
      QtProperty *group (_idToProperty["Flag"]);
   
      if (!group) {
   
         group = _groupManager->addProperty ("Flag");
         _ui.propertyEditor->addProperty (group);
         _add_property (group, "Flag");
      }
   
      if (group) {

         const QString Name (_defs.lookup_named_handle_name (AttributeHandle).get_buffer ());
      
         QtVariantProperty *property (_variantManager->addProperty (QVariant::Bool, Name));
         property->setValue (Value);

         group->addSubProperty (property);
      
//         _add_property (property, Name);
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

      QtProperty *group (_idToProperty["TimeStamp"]);

      if (!group) {

         group = _groupManager->addProperty ("TimeStamp");
         _ui.propertyEditor->addProperty (group);
         _add_property (group, "TimeStamp");
      }

      if (group) {

         const QString Name (_defs.lookup_named_handle_name (AttributeHandle).get_buffer ());

         QtVariantProperty *property (_variantManager->addProperty (QVariant::Double, Name));
         property->setValue (Value);

         group->addSubProperty (property);

//         _add_property (property, Name);
      }
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

      QtProperty *group (_idToProperty["Position"]);

      if (!group) {

         group = _groupManager->addProperty ("Position");
         _ui.propertyEditor->addProperty (group);
         _add_property (group, "Position");
      }

      if (group) {

         const QString Name (_defs.lookup_named_handle_name (AttributeHandle).get_buffer ());

         QtProperty *property (_vectorManager->addProperty (Name));
         _vectorManager->setValue (property, Value);

         group->addSubProperty (property);

//         _add_property (property, Name);
      }
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

}


void
dmz::QtPluginPropertyBrowser::update_object_velocity (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

}


void
dmz::QtPluginPropertyBrowser::update_object_acceleration (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

}


void
dmz::QtPluginPropertyBrowser::update_object_scale (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

}


void
dmz::QtPluginPropertyBrowser::update_object_vector (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

}


void
dmz::QtPluginPropertyBrowser::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

}


void
dmz::QtPluginPropertyBrowser::update_object_text (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const String &Value,
      const String *PreviousValue) {

   if (ObjectHandle == _currentObject) {

      QtProperty *group (_idToProperty["Text"]);

      if (!group) {

         group = _groupManager->addProperty ("Text");
         _ui.propertyEditor->addProperty (group);
         _add_property (group, "Text");
      }

      if (group) {

         const QString Name (_defs.lookup_named_handle_name (AttributeHandle).get_buffer ());

         QtVariantProperty *property (_variantManager->addProperty (QVariant::String, Name));
         property->setValue (Value.get_buffer ());

         group->addSubProperty (property);

//         _add_property (property, Name);
      }
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
   _currentObject = _handle (current);
      
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


void
dmz::QtPluginPropertyBrowser::_add_property (QtProperty *property, const QString &id) {
   
   _propertyToId[property] = id;
   _idToProperty[id] = property;
   
   QtBrowserItem *item = _ui.propertyEditor->addProperty (property);
   
   if (_idToExpanded.contains (id)) {
      
//      _ui.propertyEditor->setExpanded(item, _idToExpanded[id]);
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


dmz::Handle
dmz::QtPluginPropertyBrowser::_handle (QTreeWidgetItem *item) {

   Handle retVal (0);
   if (item) { retVal = item->data (HandleCol, HandleRole).toULongLong (); }
   return retVal;
}


void
dmz::QtPluginPropertyBrowser::_init (Config &local) {

   setObjectName (get_plugin_name ().get_buffer ());

   _ui.setupUi (this);
   
   qframe_config_read ("frame", local, this);
   
//   activate_global_object_observer ();

   _defaultAttrHandle = activate_default_object_attribute (
      ObjectCreateMask | ObjectDestroyMask);

   _ui.splitter->setStretchFactor (0, 0);
   _ui.splitter->setStretchFactor (1, 1);
   
   QStringList headerList;
   headerList << "Handle" << "ObjectType";
   _ui.objectTreeWidget->setHeaderLabels (headerList);
   
   _variantManager = new QtVariantPropertyManager (this);
   _variantManagerRO = new QtVariantPropertyManager (this);
   _enumManager = new QtEnumPropertyManager (this);
   _groupManager = new QtGroupPropertyManager (this);
   _vectorManager = new VectorPropertyManager (this);

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
