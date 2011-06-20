#include <dmzQtConfigRead.h>
#include "dmzQtPluginPreferencesAppUpdater.h"
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <dmzRuntimeVersion.h>
#include <QtGui/QtGui>


dmz::QtPluginPreferencesAppUpdater::QtPluginPreferencesAppUpdater (
      const PluginInfo &Info,
      Config &local,
      Config &global) :
      QFrame (0),
      QtWidget (Info),
      Plugin (Info),
      MessageObserver (Info),
      _log (Info),
      _valueAttrHandle (0),
      _updateMessage (),
      _channelMessage () {

   _init (local, global);
}


dmz::QtPluginPreferencesAppUpdater::~QtPluginPreferencesAppUpdater () {

}


// QtWidget Interface
QWidget *
dmz::QtPluginPreferencesAppUpdater::get_qt_widget () {

   return this;
}


// Plugin Interface
void
dmz::QtPluginPreferencesAppUpdater::update_plugin_state (
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
dmz::QtPluginPreferencesAppUpdater::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Message Observer Interface
void
dmz::QtPluginPreferencesAppUpdater::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   if (InData) {

      if (Type == _updateMessage) {

         Boolean value;
         if (InData->lookup_boolean (_valueAttrHandle, 0, value)) {

            _ui.updateCheckBox->setChecked (value);
         }
      }
      else if (Type == _channelMessage) {

         String value;
         if (InData->lookup_string (_valueAttrHandle, 0, value)) {

            int index = _ui.channelComboBox->findText (value.get_buffer ());

            if (index != -1) {

               _ui.channelComboBox->setCurrentIndex (index);
            }
         }
      }
   }
}


void
dmz::QtPluginPreferencesAppUpdater::on_updateCheckBox_stateChanged (int value) {

   Data data;
   data.store_boolean (_valueAttrHandle, 0, value ? True : False);

   _updateMessage.send (&data);
}


void
dmz::QtPluginPreferencesAppUpdater::on_channelComboBox_currentIndexChanged (
      const QString &Text) {

   String value ("stable");
   if (!Text.isEmpty ()) { value = qPrintable (Text); }

   Data data;
   data.store_string (_valueAttrHandle, 0, qPrintable (Text));

   _channelMessage.send (&data);
}


void
dmz::QtPluginPreferencesAppUpdater::_init (Config &local, Config &global) {

   setObjectName (get_plugin_name ().get_buffer ());

   qframe_config_read ("frame", local, this);

   _ui.setupUi (this);

   Version version (global);
   const QString Name (version.get_name ().get_buffer ());

   QString infoText (_ui.infoLabel->text ());
   infoText.replace ("{app_name}", Name);

   _ui.infoLabel->setText (infoText);

   RuntimeContext *context (get_plugin_runtime_context ());

   _updateMessage = config_create_message (
      "update.message", local, "AppUpdaterUpdateMessage", context);

   _channelMessage = config_create_message (
      "channel.message", local, "AppUpdaterChannelMessage", context);

   _updateMessage.set_monostate_mode (MessageMonostateOn);
   _channelMessage.set_monostate_mode (MessageMonostateOn);

   subscribe_to_message (_updateMessage);
   subscribe_to_message (_channelMessage);

   _valueAttrHandle = config_to_named_handle (
      "attribute.value.name",
      local,
      "value",
      context);

   Config channelList;

   if (local.lookup_all_config ("release-channel", channelList)) {

      QStringList itemList;
      ConfigIterator it;
      Config channel;

      while (channelList.get_next_config (it, channel)) {

         const String ChannelName (config_to_string ("name", channel));

         if (ChannelName) {

            itemList << ChannelName.get_buffer ();
         }
      }

      _ui.channelComboBox->clear ();
      _ui.channelComboBox->addItems (itemList);
   }

   _ui.updateCheckBox->setChecked (config_to_boolean ("update.value", local, True));
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginPreferencesAppUpdater (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginPreferencesAppUpdater (Info, local, global);
}

};
