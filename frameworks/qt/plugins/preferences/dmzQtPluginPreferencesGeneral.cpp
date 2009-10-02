#include <dmzQtConfigRead.h>
#include "dmzQtPluginPreferencesGeneral.h"
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <QtGui/QtGui>


dmz::QtPluginPreferencesGeneral::QtPluginPreferencesGeneral (
      const PluginInfo &Info,
      Config &local) :
      QFrame (0),
      QtWidget (Info),
      Plugin (Info),
      MessageObserver (Info),
      _log (Info),
      _defs (Info),
      _layout (0),
      _valueAttrHandle (0),
      _messageTable () {

   _init (local);
}


dmz::QtPluginPreferencesGeneral::~QtPluginPreferencesGeneral () {

   _messageTable.empty ();
}


// QtWidget Interface
QWidget *
dmz::QtPluginPreferencesGeneral::get_qt_widget () {

   return this;
}


// Plugin Interface
void
dmz::QtPluginPreferencesGeneral::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

      HashTableStringIterator it;

      MessageStruct *ms (_messageTable.get_first (it));

      while (ms) {

         QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox *> (ms->widget);

         if (spinBox) {

            Data data;
            data.store_float64 (_valueAttrHandle, 0, spinBox->value ());
            ms->message.send (&data);
         }

         ms = _messageTable.get_next (it);
      }
   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::QtPluginPreferencesGeneral::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Message Observer Interface
void
dmz::QtPluginPreferencesGeneral::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   MessageStruct *ms (_messageTable.lookup (Type.get_name ()));
         
   if (ms && InData) {

      QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox *> (ms->widget);
      
      if (spinBox) {
         
         Float64 value;
         if (InData->lookup_float64 (_valueAttrHandle, 0, value)) {
            
            spinBox->setValue (value);
         }
      }
   }
}


void
dmz::QtPluginPreferencesGeneral::_slot_scalar_value_changed (double value) {
   
   QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox *> (sender ());
   
   if (spinBox) {
      
      QString name (spinBox->objectName ());
      MessageStruct *ms = _messageTable.lookup (qPrintable (name));
      
      if (ms && ms->message) {
         
         Data data;
         const Float64 Value (spinBox->value ());
         data.store_float64 (_valueAttrHandle, 0, Value);
         ms->message.send (&data);
      }
   }
}


void
dmz::QtPluginPreferencesGeneral::_create_properties (Config &list) {
   
   ConfigIterator it;
   Config property;
   
   RuntimeContext *context (get_plugin_runtime_context ());
   
   while (list.get_next_config (it, property)) {
      
      const String Type (config_to_string ("type", property));
      const String Name (config_to_string ("name", property) + ":");
      Message message (config_create_message ("message", property, "", context));
      message.set_monostate_mode (MessageMonostateOn);
      
      if (Type && Name && message) {

         subscribe_to_message (message);

         MessageStruct *ms (new MessageStruct);
         
         if (ms) {
            
            if (Type == "scalar") {

               QDoubleSpinBox *scalar (new QDoubleSpinBox);
               qdoublespinbox_config_read ("", property, scalar);
               
               connect (
                  scalar, SIGNAL (valueChanged (double)),
                  this, SLOT (_slot_scalar_value_changed (double)));
               
               ms->widget = scalar;
            }
            // else if (Type == "line") {
            //    
            // }
            // else if (Type == "") {
            //    
            // }
            
            if (ms->widget) {
               
               ms->widget->setObjectName (message.get_name ().get_buffer ());
            }
            
            ms->message = message;
            
            if (_messageTable.store (ms->message.get_name (), ms)) {
            
               QLabel *label (new QLabel (Name.get_buffer ()));
            
               _layout->addRow (label, ms->widget);
            }
            else { delete ms; ms = 0; }
         }
      }
   }
}


void
dmz::QtPluginPreferencesGeneral::_init (Config &local) {

   setObjectName (get_plugin_name ().get_buffer ());
   
  RuntimeContext *context (get_plugin_runtime_context ());

   qframe_config_read ("frame", local, this);
   
   _layout = new QFormLayout;
   
   setLayout (_layout);
   
   _valueAttrHandle = config_to_named_handle (
      "attribute.value.name",
      local,
      "value",
      context);
      
   Config list;
   
   if (local.lookup_all_config ("property", list)) {
      
      _create_properties (list);
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginPreferencesGeneral (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginPreferencesGeneral (Info, local);
}

};
