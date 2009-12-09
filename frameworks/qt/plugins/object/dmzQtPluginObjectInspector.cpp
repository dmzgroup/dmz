#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzQtConfigRead.h>
#include "dmzQtPluginObjectInspector.h"
#include "dmzQtObjectInspector.h"
#include <dmzQtUtil.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <dmzTypesUUID.h>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QDesktopWidget>

namespace {

   const dmz::UInt32 HandleRole (Qt::UserRole + 1);
   const dmz::UInt32 HandleCol (0);
   const dmz::UInt32 TypeCol (1);
   const dmz::UInt32 LocalityCol (2);
   const dmz::UInt32 UUIDCol (3);
};


dmz::QtPluginObjectInspector::QtPluginObjectInspector (
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
      _inspectorTable (),
      _objects (),
      _newWindowPos () {

   _init (local);

   QDesktopWidget *desktop = QApplication::desktop ();
   QRect rect = desktop->availableGeometry (desktop->primaryScreen ());
   _newWindowPos = rect.topLeft ();
}


dmz::QtPluginObjectInspector::~QtPluginObjectInspector () {

   _inspectorTable.empty ();
}


// QtWidget Interface
QWidget *
dmz::QtPluginObjectInspector::get_qt_widget () {

   return this;
}


// Plugin Interface
void
dmz::QtPluginObjectInspector::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

      Config session (get_session_config (get_plugin_name (), get_plugin_runtime_context ()));

      QByteArray geometry (config_to_qbytearray ("geometry", session, saveGeometry ()));
      restoreGeometry (geometry);

     // if (config_to_boolean ("window.visible", session, False)) { show (); }

      show ();
   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

      // save geometry to session
      String data;

      Config session (get_plugin_name ());

      session.add_config (qbytearray_to_config ("geometry", saveGeometry ()));

      // if (isVisible ()) {
      //
      //    session.add_config (boolean_to_config ("window", "visible", True));
      // }

      set_session_config (get_plugin_runtime_context (), session);

      // close all inspector windows
      HashTableHandleIterator it;
      QtObjectInspector *inspector (0);

      while (_inspectorTable.get_next (it, inspector)) {

         inspector->close ();
      }

      hide ();
   }
}


void
dmz::QtPluginObjectInspector::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Message Observer Interface
void
dmz::QtPluginObjectInspector::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

}


// Object Observer Interface
void
dmz::QtPluginObjectInspector::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (!_objects.contains (ObjectHandle)) {

      QTreeWidgetItem *item = new QTreeWidgetItem ();

      item->setText (HandleCol, to_qstring (ObjectHandle));
      item->setData (HandleCol, HandleRole, (quint64)ObjectHandle);

      item->setText (TypeCol, to_qstring (Type));
      item->setText (UUIDCol, to_qstring (Identity));

      QLatin1String locality ("Unknown");
      if (Locality == ObjectLocal) { locality = QLatin1String ("Local"); }
      else if (Locality == ObjectRemote) { locality = QLatin1String ("Remote"); }
      item->setText (LocalityCol, locality);

      _ui.objectTreeWidget->addTopLevelItem (item);

      if (_ui.objectTreeWidget->topLevelItemCount () == 1) {

         _ui.objectTreeWidget->setCurrentItem (item);
      }

      _objects.add_handle (ObjectHandle);
   }
   else {

      QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
      if (inspector) {

         inspector->create_object (Identity, ObjectHandle, Type, Locality);
      }
   }
}

void
dmz::QtPluginObjectInspector::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->destroy_object (Identity, ObjectHandle);
   }

   if (_objects.contains (ObjectHandle))  {

      for (int ix = 0; ix < _ui.objectTreeWidget->topLevelItemCount (); ++ix) {

         QTreeWidgetItem *item = _ui.objectTreeWidget->topLevelItem (ix);
         if (ObjectHandle == _item_to_handle (item)) {

            item = _ui.objectTreeWidget->takeTopLevelItem (ix);
            delete item; item = 0;
            break;
         }
      }
   }
}


void
dmz::QtPluginObjectInspector::update_object_uuid (
      const Handle ObjectHandle,
      const UUID &Identity,
      const UUID &PrevIdentity) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->update_object_uuid (ObjectHandle, Identity, PrevIdentity);
   }
}


void
dmz::QtPluginObjectInspector::remove_object_attribute (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &AttrMask) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->remove_object_attribute (
         Identity, ObjectHandle, AttributeHandle, AttrMask);
   }
}


void
dmz::QtPluginObjectInspector::update_object_locality (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectLocalityEnum Locality,
      const ObjectLocalityEnum PrevLocality) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->update_object_locality (Identity, ObjectHandle, Locality, PrevLocality);
   }
}


void
dmz::QtPluginObjectInspector::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   QtObjectInspector *inspector = _inspectorTable.lookup (SuperHandle);
   if (inspector) {

      inspector->link_objects (
         LinkHandle, AttributeHandle, SuperIdentity, SuperHandle, SubIdentity, SubHandle);
   }
   
   inspector = _inspectorTable.lookup (SubHandle);
   if (inspector) {

      inspector->link_objects (
         LinkHandle, AttributeHandle, SuperIdentity, SuperHandle, SubIdentity, SubHandle);
   }
}


void
dmz::QtPluginObjectInspector::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   QtObjectInspector *inspector = _inspectorTable.lookup (SuperHandle);
   if (inspector) {
   
      inspector->unlink_objects (
         LinkHandle, AttributeHandle, SuperIdentity, SuperHandle, SubIdentity, SubHandle);
   }

   QtObjectInspector *inspector = _inspectorTable.lookup (SubHandle);
   if (inspector) {
   
      inspector->unlink_objects (
         LinkHandle, AttributeHandle, SuperIdentity, SuperHandle, SubIdentity, SubHandle);
   }
}


void
dmz::QtPluginObjectInspector::update_link_attribute_object (
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

   // QtObjectInspector *inspector = _inspectorTable.lookup (LinkHandle);
   // if (inspector) {
   //
   //    inspector->update_link_attribute_object (
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
dmz::QtPluginObjectInspector::update_object_counter (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->update_object_counter (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginObjectInspector::update_object_counter_minimum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->update_object_counter_minimum (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginObjectInspector::update_object_counter_maximum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->update_object_counter_maximum (
        Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginObjectInspector::update_object_alternate_type (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const ObjectType &Value,
      const ObjectType *PreviousValue) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->update_object_alternate_type (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginObjectInspector::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->update_object_state (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginObjectInspector::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->update_object_flag (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginObjectInspector::update_object_time_stamp (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->update_object_time_stamp (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginObjectInspector::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->update_object_position (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginObjectInspector::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->update_object_orientation (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginObjectInspector::update_object_velocity (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->update_object_velocity (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginObjectInspector::update_object_acceleration (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->update_object_acceleration (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginObjectInspector::update_object_scale (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->update_object_scale (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginObjectInspector::update_object_vector (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->update_object_vector (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginObjectInspector::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->update_object_scalar (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginObjectInspector::update_object_text (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const String &Value,
      const String *PreviousValue) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->update_object_text (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginObjectInspector::update_object_data (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Data &Value,
      const Data *PreviousValue) {

   QtObjectInspector *inspector = _inspectorTable.lookup (ObjectHandle);
   if (inspector) {

      inspector->update_object_data (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginObjectInspector::on_objectTreeWidget_itemActivated (
      QTreeWidgetItem *item,
      int column) {

   ObjectModule *objMod (get_object_module ());
   Handle objHandle = _item_to_handle (item);

   QtObjectInspector *inspector = _inspectorTable.lookup (objHandle);

   if (inspector) {

      inspector->show ();
      inspector->raise ();
   }
   else if (objMod) {

      QtObjectInspector *inspector =
         new QtObjectInspector (objHandle, *this, get_plugin_runtime_context ());

      if (_inspectorTable.store (objHandle, inspector)) {

         connect (
            inspector, SIGNAL (finished (const Handle)),
            this, SLOT (_inspector_finished (const Handle)));

         objMod->dump_all_object_attributes (objHandle, *this);

         inspector->move (_newWindowPos);
         inspector->show ();

         _newWindowPos += QPoint (25, 25);
      }
      else {

         delete inspector; inspector = 0;
      }
   }
}


void
dmz::QtPluginObjectInspector::_inspector_finished (const Handle ObjectHandle) {

   QtObjectInspector *inspector = _inspectorTable.remove (ObjectHandle);
   if (inspector) { delete inspector; inspector = 0; }
}


dmz::Handle
dmz::QtPluginObjectInspector::_item_to_handle (QTreeWidgetItem *item) {

   Handle retVal (0);
   if (item) { retVal = item->data (HandleCol, HandleRole).toULongLong (); }
   return retVal;
}


void
dmz::QtPluginObjectInspector::_init (Config &local) {

   setObjectName (get_plugin_name ().get_buffer ());

   _ui.setupUi (this);

   qframe_config_read ("frame", local, this);

   QStringList headerList;
   headerList << "Handle" << "ObjectType" << "Locality" << "UUID";
   _ui.objectTreeWidget->setHeaderLabels (headerList);

   activate_global_object_observer ();
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginObjectInspector (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginObjectInspector (Info, local);
}

};
