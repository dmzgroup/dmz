#include <dmzQtConfigRead.h>
#include "dmzQtPluginPreferencesGeneral.h"
#include <dmzQtUtil.h>
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

      ScalarStruct (const Handle AttrHandle, DataConverterString &dcs);
      ~ScalarStruct ();

      void init (Config &local);
      void update (const Data &InData);
      void send_message ();

      private:
         QDoubleSpinBox *_spinBox;
         Float64 _scale;
   };

   struct BooleanStruct : public BaseWidgetStruct {

      BooleanStruct (const Handle AttrHandle, DataConverterString &dcs);
      ~BooleanStruct ();

      void init (Config &local);
      void update (const Data &InData);
      void send_message ();

   private:
      QCheckBox *_checkBox;
   };

   struct StateStruct : public BaseWidgetStruct {
      StateStruct (const Handle AttrHandle, DataConverterString &dcs);
      ~StateStruct ();

      void init (Config &local);
      void update (const Data &InData);
      void send_message ();

   private:
      QComboBox *_comboBox;

   };

   ScalarStruct::ScalarStruct (const Handle AttrHandle, DataConverterString &dcs) :
         BaseWidgetStruct (AttrHandle, dcs),
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


   BooleanStruct::BooleanStruct (const Handle AttrHandle, DataConverterString &dcs) :
         BaseWidgetStruct (AttrHandle, dcs),
         _checkBox (new QCheckBox) {

      widget = _checkBox;
   }


   BooleanStruct::~BooleanStruct () {

      if (_checkBox) {

         _checkBox->setParent (0);
         delete _checkBox;
         _checkBox = 0;
      }
   }


   void
   BooleanStruct::init (Config &local) {

      if (_checkBox) {

         _checkBox->setChecked (config_to_boolean ("checked", local));
         _checkBox->setText (config_to_string ("text", local).get_buffer ());
      }
   }


   void
   BooleanStruct::send_message () {

      if (_checkBox) {

         Data data;
         data.store_boolean (ValueAttrHandle, 0, _checkBox->isChecked () ? True : False);
         message.send (&data);
      }
   }

   void
   BooleanStruct::update (const Data &InData) {

      if (_checkBox) {

         Boolean value;
         if (InData.lookup_boolean (ValueAttrHandle, 0, value)) {

            _checkBox->setChecked (value);
         }
      }
   }

   StateStruct::StateStruct (const Handle AttrHandle, DataConverterString &dcs) :
         BaseWidgetStruct (AttrHandle, dcs),
         _comboBox (new QComboBox) {

      widget = _comboBox;
   }


   StateStruct::~StateStruct () {

      if (_comboBox) {

         _comboBox->setParent (0);
         delete _comboBox;
         _comboBox = 0;
      }
   }


   void
   StateStruct::init (Config &local) {

      if (_comboBox) {

         ConfigIterator it;
         Config stateList, state;
         local.lookup_all_config ("state", stateList);
         while (stateList.get_next_config (it, state)) {

            const String Name = config_to_string ("label", state);
            _comboBox->addItem (to_qstring(Name));
            if (config_to_boolean ("default", state, False)) {

               _comboBox->setCurrentIndex (_comboBox->findText (to_qstring (Name)));
            }
         }
      }
   }


   void
   StateStruct::send_message () {

      if (_comboBox) {

         Data data = _convert.to_data (qPrintable(_comboBox->currentText ()));
         message.send (&data);
      }
   }

   void
   StateStruct::update (const Data &InData) {

      if (_comboBox) {

         String value = _convert.to_string (InData);
         if (value) {
            _comboBox->setCurrentIndex (_comboBox->findText (to_qstring (value)));
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
      _convert (*(new DataConverterString (Info))),
      _layout (0),
      _valueAttrHandle (0),
      _widgetTable () {

   _init (local);
}


dmz::QtPluginPreferencesGeneral::~QtPluginPreferencesGeneral () {

   _widgetTable.empty ();
   delete &_convert;
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
dmz::QtPluginPreferencesGeneral::_slot_widget_value_changed () {

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

            ScalarStruct *ss (new ScalarStruct (_valueAttrHandle, _convert));
            ws = ss;

            connect (
               ss->widget, SIGNAL (valueChanged (double)),
               this, SLOT (_slot_widget_value_changed ()));
         }
         else if (Type == "boolean") {

            BooleanStruct *bs (new BooleanStruct (_valueAttrHandle, _convert));
            ws = bs;

            connect (
               bs->widget, SIGNAL (stateChanged (int)),
               this, SLOT (_slot_widget_value_changed ()));
         }
         else if (Type == "list") {

//            ComboBoxStruct *cs (new ComboBoxStruct (_valueAttrHandle));
//            ws = cs;

//            connect (
//               cs->widget, SIGNAL (currentIndexChanged (int)),
//               this, SLOT (_slot_widget_value_changed ()));
         }
         else if (Type == "state") {
            StateStruct *ss (new StateStruct (_valueAttrHandle, _convert));
            ws = ss;

            connect (
                  ss->widget, SIGNAL (currentIndexChanged (const QString &)),
                  this, SLOT (_slot_widget_value_changed ()));
         }

         if (ws) {

            ws->init (property);

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
