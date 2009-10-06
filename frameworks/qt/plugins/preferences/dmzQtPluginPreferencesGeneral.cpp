#include <dmzQtConfigRead.h>
#include "dmzQtPluginPreferencesGeneral.h"
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <QtGui/QtGui>

using namespace dmz;

namespace {
   
   typedef dmz::QtPluginPreferencesGeneral::WidgetStruct BaseWidgetStruct;
   
   struct ScalarStruct : public BaseWidgetStruct {

      ScalarStruct (const Handle AttrHandle);
      ~ScalarStruct ();
      
      void init (Config &local);
      void update (const Data &InData);
      void send_message ();
      
      private:
         QDoubleSpinBox *_spinBox;
         Float64 _scale;
   };
   
   ScalarStruct::ScalarStruct (const Handle AttrHandle) :
         BaseWidgetStruct (AttrHandle),
         _spinBox (new QDoubleSpinBox),
         _scale (1.0) {
   
      widget = _spinBox;
   }


   ScalarStruct::~ScalarStruct () {
      
      if (_spinBox) {
         
         _spinBox->setParent (0);
         delete _spinBox;
         _spinBox = 0;
      }
   }
   
   
   void
   ScalarStruct::init (Config &local) {
   
      qdoublespinbox_config_read ("", local, _spinBox);
      _scale = config_to_float64 ("scale", local, 1.0);
      if (_scale < 0.0) { _scale = 1.0; }
      widget = _spinBox;
   }
   
   
   void
   ScalarStruct::send_message () {

      if (_spinBox) {

         Data data;
         data.store_float64 (ValueAttrHandle, 0, _spinBox->value () / _scale);
         message.send (&data);
      }
   }
   
   void
   ScalarStruct::update (const Data &InData) {

      if (_spinBox) {
         
         Float64 value;
         if (InData.lookup_float64 (ValueAttrHandle, 0, value)) {
            
            _spinBox->setValue (value * _scale);
         }
      }
   }
};


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
      _widgetTable () {

   _init (local);
}


dmz::QtPluginPreferencesGeneral::~QtPluginPreferencesGeneral () {

   _widgetTable.empty ();
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
      WidgetStruct *ws (_widgetTable.get_first (it));

      while (ws) {

         ws->send_message ();
         ws = _widgetTable.get_next (it);
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

   WidgetStruct *ws (_widgetTable.lookup (Type.get_name ()));
         
   if (ws && InData) {

      ws->update (*InData);
   }
}


void
dmz::QtPluginPreferencesGeneral::_slot_scalar_value_changed (double value) {
   
   HashTableStringIterator it;
   WidgetStruct *ws (_widgetTable.get_first (it));
   Boolean found (False);
   
   while (ws && !found) {

      if (ws->widget == sender ()) {
         
         ws->send_message ();
         found = True;
      }

      ws = _widgetTable.get_next (it);
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

         WidgetStruct *ws;
         
         if (Type == "scalar") {

            ScalarStruct *ss (new ScalarStruct (_valueAttrHandle));
            ss->init (property);

            connect (
               ss->widget, SIGNAL (valueChanged (double)),
               this, SLOT (_slot_scalar_value_changed (double)));

            ws = ss;
         }
         // else if (Type == "line") {
         //    
         // }
         // else if (Type == "") {
         //    
         // }
         
         if (ws) {
            
            if (ws->widget) {
               
               ws->widget->setObjectName (message.get_name ().get_buffer ());
            }
            
            ws->message = message;
            
            if (_widgetTable.store (ws->message.get_name (), ws)) {
            
               QLabel *label (new QLabel (Name.get_buffer ()));
            
               _layout->addRow (label, ws->widget);
            }
            else { delete ws; ws = 0; }
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
