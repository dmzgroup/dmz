#include "dmzQtPluginMessageInspector.h"
#include <dmzQtUtil.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzSystemStreamString.h>
#include <QtGui/QTreeWidget>
#include <QtGui/QComboBox>
#include <QtGui/QSpinBox>

dmz::QtPluginMessageInspector::QtPluginMessageInspector (const PluginInfo &Info, Config &local) :
      QWidget (0),
      Plugin (Info),
      MessageObserver (Info),
      _log (Info) {

   _ui.setupUi (this);
   _init (local);

   QStringList labels;
   labels << "Message Target" << "Message Name" << "Message Data";
   _ui.treeWidgetList->setColumnCount(labels.count ());
   _ui.treeWidgetList->setHeaderLabels (labels);
   _ui.treeWidgetList->setSortingEnabled (True);
   _ui.treeWidgetList->setColumnHidden (labels.size () - 1, True);

   connect (_ui.treeWidgetList, SIGNAL (itemSelectionChanged ()),
            this, SLOT (_update_displayed_text ()));

   _ui.targetList->setInsertPolicy (QComboBox::InsertAlphabetically);
   connect (_ui.targetList, SIGNAL (currentIndexChanged (const QString &)),
            this, SLOT (_filter_for_selected_target (const QString &)));
   _ui.targetList->addItem ("Show All");

   connect (_ui.clearButton, SIGNAL (clicked ()), _ui.treeWidgetList, SLOT (clear ()));

   _ui.textEdit->setReadOnly (True);
   subscribe_to_message (Definitions (Info).get_global_message ());
}


dmz::QtPluginMessageInspector::~QtPluginMessageInspector () {

}


// Plugin Interface
void
dmz::QtPluginMessageInspector::update_plugin_state (
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

   }
}


void
dmz::QtPluginMessageInspector::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Message Observer Interface
void
dmz::QtPluginMessageInspector::receive_message (
      const Message &Type,
      const Handle MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   Definitions defs (get_plugin_runtime_context ());

   String target ("GLOBAL");
   String messageInData ("No Message Data");

   if (InData) {
      messageInData = __runtimedata_to_string (*InData);
   }

   if (TargetObserverHandle) {

      target = defs.lookup_named_handle_name (TargetObserverHandle);
      if (!target) { target = defs.lookup_runtime_name (TargetObserverHandle); }
   }

   while (_ui.treeWidgetList->topLevelItemCount () > _ui.maxMessageCount->value ()) {
      _ui.treeWidgetList->takeTopLevelItem (
            _ui.treeWidgetList->topLevelItemCount () - 1);
   }

   QString messageTarget = to_qstring (target);
   QStringList messageData;
   messageData << messageTarget << to_qstring (Type.get_name ())
         << to_qstring(messageInData);

   QTreeWidgetItem *messageItem = new QTreeWidgetItem ((QTreeWidget*)0, messageData);
   QString filterText = _ui.targetList->currentText ();
   messageItem->setHidden (filterText != "Show All" && messageTarget != filterText);

   _ui.treeWidgetList->addTopLevelItem (messageItem);

   for (UInt32 i = 0; i < _ui.treeWidgetList->columnCount (); ++ i) {
      _ui.treeWidgetList->resizeColumnToContents (i);
   }

   if (_ui.targetList->findText (messageTarget) == -1) {
      _ui.targetList->addItem (messageTarget);
   }
   //_messageDataTable.store (MessageSendHandle, &messageData);
}

void
dmz::QtPluginMessageInspector::_update_displayed_text () {
   QTreeWidgetItem *selectedMessage = _ui.treeWidgetList->currentItem ();
   QString messageData = selectedMessage->text (_ui.treeWidgetList->columnCount () - 1);
   _ui.textEdit->setText (messageData);
}

void
dmz::QtPluginMessageInspector::_filter_for_selected_target (const QString &text) {
   QTreeWidgetItem *selectedMessage;
   QString pluginTarget;
   for (UInt32 index = 0; index < _ui.treeWidgetList->topLevelItemCount (); ++index) {
      selectedMessage = _ui.treeWidgetList->topLevelItem (index);
      pluginTarget = selectedMessage->text (0);
      selectedMessage->setHidden (text != "Show All" && pluginTarget != text);
   }
}

// QtPluginMessageInspector Interface
void
dmz::QtPluginMessageInspector::_init (Config &local) {

}

dmz::String
dmz::QtPluginMessageInspector::__runtimedata_to_string (const Data &Value) {

   String str;
   StreamString stream (str);
   stream << Value;
   return str;
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginMessageInspector (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginMessageInspector (Info, local);
}

};
