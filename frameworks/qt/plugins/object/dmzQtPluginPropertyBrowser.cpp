#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzQtConfigRead.h>
#include "dmzQtPluginPropertyBrowser.h"
#include "dmzQtPropertyBrowser.h"
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesUUID.h>

namespace {
  
   const dmz::UInt32 HandleRole (Qt::UserRole + 1);
   const dmz::UInt32 HandleCol (0);
   const dmz::UInt32 TypeCol (1); 
};

dmz::QtPluginPropertyBrowser::QtPluginPropertyBrowser (
      const PluginInfo &Info,
      Config &local) :
      QMainWindow (0),
      Plugin (Info),
      MessageObserver (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defs (Info, &_log),
      _defaultAttrHandle (0),
      _browserTable () {

   _init (local);
}


dmz::QtPluginPropertyBrowser::~QtPluginPropertyBrowser () {

   _browserTable.empty ();
}


// Plugin Interface
void
dmz::QtPluginPropertyBrowser::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

      show ();
   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

      hide ();
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

   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {
      
      browser->create_object (Identity, ObjectHandle, Type, Locality);
   }
   else {
      
      QTreeWidgetItem *item = new QTreeWidgetItem;

      item->setText (HandleCol, _handle_to_string (ObjectHandle));
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

   QtPropertyBrowser *browser = _browserTable.remove (ObjectHandle);
   if (browser) { delete browser; browser = 0; }
   
   for (int ix = 0; ix < _ui.objectTreeWidget->topLevelItemCount (); ++ix) {

      QTreeWidgetItem *item = _ui.objectTreeWidget->topLevelItem (ix);
      if (ObjectHandle == _item_to_handle (item)) {
         
         item = _ui.objectTreeWidget->takeTopLevelItem (ix);
         delete item; item = 0;
         break;
      }
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_uuid (
      const Handle ObjectHandle,
      const UUID &Identity,
      const UUID &PrevIdentity) {

   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {
   
      browser->update_object_uuid (ObjectHandle, Identity, PrevIdentity);
   }
}


void
dmz::QtPluginPropertyBrowser::remove_object_attribute (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &AttrMask) {

   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {

      browser->remove_object_attribute (
         Identity, ObjectHandle, AttributeHandle, AttrMask);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_locality (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectLocalityEnum Locality,
      const ObjectLocalityEnum PrevLocality) {

   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {

      browser->update_object_locality (Identity, ObjectHandle, Locality, PrevLocality);
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

   // QtPropertyBrowser *browser = _browserTable.lookup (LinkHandle);
   // if (browser) {
   // 
   //    browser->link_objects (
   //       LinkHandle, AttributeHandle, SuperIdentity, SuperHandle, SubIdentity, SubHandle);
   // }
}


void
dmz::QtPluginPropertyBrowser::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   // QtPropertyBrowser *browser = _browserTable.lookup (LinkHandle);
   // if (browser) {
   //    
   //    browser->unlink_objects (
   //       LinkHandle, AttributeHandle, SuperIdentity, SuperHandle, SubIdentity, SubHandle);
   // }
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


   // QtPropertyBrowser *browser = _browserTable.lookup (LinkHandle);
   // if (browser) {
   //    
   //    browser->update_link_attribute_object (
   //       LinkHandle,
   //       AttributeHandle,
   //       SuperIdentity,
   //       SuperHandle,
   //       SubIdentity,
   //       SubHandle,
   //       AttributeIdentity,
   //       AttributeObjectHandle,
   //       PrevAttributeIdentity,
   //       PrevAttributeObjectHandle);
   // }
}


void
dmz::QtPluginPropertyBrowser::update_object_counter (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {

      browser->update_object_counter (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_counter_minimum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {
    
      browser->update_object_counter_minimum (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_counter_maximum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {

      browser->update_object_counter_maximum (
        Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_alternate_type (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const ObjectType &Value,
      const ObjectType *PreviousValue) {

   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {

      browser->update_object_alternate_type (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {

      browser->update_object_state (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {

      browser->update_object_flag (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_time_stamp (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 &Value,
      const Float64 *PreviousValue) {

   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {

      browser->update_object_time_stamp (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {


   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {

      browser->update_object_position (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {

      browser->update_object_orientation (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_velocity (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {

      browser->update_object_velocity (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_acceleration (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {

      browser->update_object_acceleration (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_scale (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {

      browser->update_object_scale (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_vector (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {

      browser->update_object_vector (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {

      browser->update_object_scalar (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_text (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const String &Value,
      const String *PreviousValue) {

   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {

      browser->update_object_text (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_data (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Data &Value,
      const Data *PreviousValue) {

   QtPropertyBrowser *browser = _browserTable.lookup (ObjectHandle);
   if (browser) {

      browser->update_object_data (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::on_objectTreeWidget_itemActivated (
      QTreeWidgetItem *item,
      int column) {

   ObjectModule *objMod (get_object_module ());
   Handle objHandle = _item_to_handle (item);
      
   if (objHandle && objMod && !_browserTable.lookup (objHandle)) {

      QtPropertyBrowser *browser =
         new QtPropertyBrowser (objHandle, *this, get_plugin_runtime_context ());
      
      if (_browserTable.store (objHandle, browser)) {
      
         objMod->dump_all_object_attributes (objHandle, *this);
         browser->show ();
      }
      else {
         
         delete browser; browser = 0;
      }
   }
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
   
   QStringList headerList;
   headerList << "Handle" << "ObjectType";
   _ui.objectTreeWidget->setHeaderLabels (headerList);
   
   _defaultAttrHandle = activate_default_object_attribute (
      ObjectCreateMask | ObjectDestroyMask);
       
   activate_global_object_observer ();
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
