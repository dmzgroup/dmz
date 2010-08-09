#include "dmzQtPluginMessageInspector.h"
#include <dmzQtUtil.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigWrite.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <dmzSystemStreamString.h>
#include <QtGui/QTreeWidget>
#include <QtGui/QComboBox>
#include <QtGui/QSpinBox>
#include <QtGui/QCheckBox>

dmz::QtPluginMessageInspector::QtPluginMessageInspector (const PluginInfo &Info, Config &local) :
      QWidget (0),
      Plugin (Info),
      MessageObserver (Info),
      TimeSlice (Info, TimeSliceTypeRuntime, TimeSliceModeSingle, 0.0),
      DefinitionsObserver (Info),
      _log (Info),
      _targetFilterList (),
      _typeFilterList (),
      __messageCount (0) {

   _ui.setupUi (this);
   _init (local);
   set_definitions_observer_callback_mask (DefinitionsDumpAll, DefinitionsMessageMask);


   QStringList labels;
   labels /*<< "#"*/ << "Message Target" << "Message Name" << "Message Data";
   _ui.treeWidgetList->setColumnCount(labels.count ());
   _ui.treeWidgetList->setHeaderLabels (labels);
   _ui.treeWidgetList->setSortingEnabled (True);
   _ui.treeWidgetList->setColumnHidden (labels.size () - 1, True);
   //_ui.treeWidgetList->header ()->setSortIndicator (-1, Qt::AscendingOrder);

   connect (_ui.treeWidgetList, SIGNAL (itemSelectionChanged ()),
            this, SLOT (_update_displayed_text ()));

   connect (_ui.messageTargetList, SIGNAL (itemChanged (QListWidgetItem *)),
            this, SLOT (_apply_new_filter (QListWidgetItem *)));

   connect (_ui.messageTypeList, SIGNAL (itemChanged (QListWidgetItem *)),
            this, SLOT (_apply_new_filter (QListWidgetItem *)));

   connect (_ui.clearButton, SIGNAL (clicked ()), _ui.treeWidgetList, SLOT (clear ()));
   connect (_ui.targetCheckButton, SIGNAL (clicked ()),
            this, SLOT (_check_all_targets ()));
   connect (_ui.targetUncheckButton, SIGNAL (clicked ()),
            this, SLOT (_uncheck_all_targets ()));
   connect (_ui.typeCheckButton, SIGNAL (clicked ()),
            this, SLOT (_check_all_types ()));
   connect (_ui.typeUncheckButton, SIGNAL (clicked()),
            this, SLOT (_uncheck_all_types ()));

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

      Config session (get_session_config (get_plugin_name (), get_plugin_runtime_context ()));

      Config targetList;
      ConfigIterator it;
      if (session.lookup_all_config ("Target.target", targetList)) {

         Config target;
         while (targetList.get_next_config (it, target)) {

            QString targetString = to_qstring (
                  config_to_string ("value", target, "Failed Read"));
            QListWidgetItem *messageTargetItem;
            if (!_targetFilterList.contains (targetString)) {

                messageTargetItem =
                      new QListWidgetItem (targetString, _ui.messageTargetList);
            }
            else {

               QList<QListWidgetItem *> items = _ui.messageTargetList->findItems (
                                                   targetString, Qt::MatchFixedString);
               messageTargetItem = items.first ();
            }

            if (config_to_boolean ("checked", target)) {

               messageTargetItem->setCheckState (Qt::Checked);
            }
            else { messageTargetItem->setCheckState (Qt::Unchecked); }
         }
      }

      Config typeList;
      it.reset ();
      if (session.lookup_all_config ("Type.type", typeList)) {

         Config type;
         while (typeList.get_next_config (it, type)) {

            QString typeString = to_qstring (
                  config_to_string ("value", type, "Failed Read"));
            QListWidgetItem *messageTypeItem;
            if (!_typeFilterList.contains (typeString)) {

                messageTypeItem = new QListWidgetItem (typeString, _ui.messageTypeList);
            }
            else {

               QList<QListWidgetItem *> items =
                     _ui.messageTypeList->findItems (typeString, Qt::MatchFixedString);
               messageTypeItem = items.first ();
            }

            if (config_to_boolean ("checked", type)) {

               messageTypeItem->setCheckState (Qt::Checked);
            }
            else { messageTypeItem->setCheckState (Qt::Unchecked); }
         }
      }

      show ();
   }
   else if (State == PluginStateStart) {}
   else if (State == PluginStateStop) {}
   else if (State == PluginStateShutdown) {

      Config session (get_plugin_name ());
      Config target ("Target");
      for (int index = 0; index < _ui.messageTargetList->count (); ++index) {

         QListWidgetItem *item = _ui.messageTargetList->item (index);
         String text = qPrintable (item->text ());
         Config current (string_to_config ("target", text));
         String boolVal;
         if (item->checkState () == Qt::Checked) { boolVal = "True"; }
         else { boolVal = "False"; }
         current.store_attribute ("checked", boolVal);
         target.add_config (current);
      }

      Config type ("Type");
      for (int index = 0; index < _ui.messageTypeList->count (); ++index) {

         QListWidgetItem *item = _ui.messageTypeList->item (index);
         String text = qPrintable (item->text ());
         Config current (string_to_config ("type", text));
         String boolVal;
         if (item->checkState () == Qt::Checked) { boolVal = "True"; }
         else { boolVal = "False"; }
         current.store_attribute ("checked", boolVal);
         type.add_config (current);
      }

      session.add_config (target);
      session.add_config (type);
      set_session_config (get_plugin_runtime_context (), session);

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

   if (InData) { messageInData = __runtimedata_to_string (*InData); }

   if (TargetObserverHandle) {

      target = defs.lookup_named_handle_name (TargetObserverHandle);
      if (!target) { target = defs.lookup_runtime_name (TargetObserverHandle); }
   }

   QString messageTarget = to_qstring (target);
   QString messageType = to_qstring (Type.get_name ());
   String count;
   count << __messageCount++;
   QStringList messageData;
   messageData /*<< to_qstring (count)*/ << messageTarget << messageType
         << to_qstring(messageInData);

   QTreeWidgetItem *messageItem = new QTreeWidgetItem ((QTreeWidget*)0, messageData);
   _ui.treeWidgetList->addTopLevelItem (messageItem);

//   if (_doColumnSizeUpdate) {
//      for (int i = 0; i < _ui.treeWidgetList->columnCount (); ++ i) {

//         _ui.treeWidgetList->resizeColumnToContents (i);
//      }
//   }

   int index (0);
   for (index = 0; index < _ui.messageTargetList->count (); ++index) {

      if (_ui.messageTargetList->item (index)->text () == messageTarget) { break; }
   }

   if (index == _ui.messageTargetList->count ()) {

      QListWidgetItem *messageTargetItem =
            new QListWidgetItem (messageTarget, _ui.messageTargetList);

      messageTargetItem->setCheckState (Qt::Checked);
   }

   messageItem->setHidden (!_typeFilterList.contains (messageType)
                           || !_targetFilterList.contains (messageTarget));

   start_time_slice ();
}

void
dmz::QtPluginMessageInspector::update_time_slice (const Float64 TimeDelta) {

   for (int i = 0; i < _ui.treeWidgetList->columnCount (); ++ i) {

      _ui.treeWidgetList->resizeColumnToContents (i);
   }
}

void
dmz::QtPluginMessageInspector::define_message (const Message &Type) {

   QString messageType = to_qstring (Type.get_name ());
   UInt32 index = 0;
   for (index = 0; index < _ui.messageTypeList->count (); ++index) {

      if (_ui.messageTypeList->item (index)->text () == messageType) { break; }
   }

   if (index == _ui.messageTypeList->count ()) {

      QListWidgetItem *messageTypeItem =
            new QListWidgetItem (messageType, _ui.messageTypeList);

      messageTypeItem->setCheckState (Qt::Checked);
   }
}

void
dmz::QtPluginMessageInspector::_check_all_targets () {

   int index (0);
   for (index = 0; index < _ui.messageTargetList->count (); ++index) {

      _ui.messageTargetList->item (index)->setCheckState (Qt::Checked);
   }
}

void
dmz::QtPluginMessageInspector::_uncheck_all_targets () {

   int index (0);
   for (index = 0; index < _ui.messageTargetList->count (); ++index) {

      _ui.messageTargetList->item (index)->setCheckState (Qt::Unchecked);
   }
}

void
dmz::QtPluginMessageInspector::_check_all_types () {

   int index (0);
   for (index = 0; index < _ui.messageTypeList->count (); ++index) {

      _ui.messageTypeList->item (index)->setCheckState (Qt::Checked);
   }
}

void
dmz::QtPluginMessageInspector::_uncheck_all_types () {

   int index (0);
   for (index = 0; index < _ui.messageTypeList->count (); ++index) {

      _ui.messageTypeList->item (index)->setCheckState (Qt::Unchecked);
   }
}

void
dmz::QtPluginMessageInspector::_update_displayed_text () {

   QTreeWidgetItem *selectedMessage = _ui.treeWidgetList->currentItem ();
   QString messageData = selectedMessage->text (_ui.treeWidgetList->columnCount () - 1);
   _ui.textEdit->setText (messageData);
}

void
dmz::QtPluginMessageInspector::_apply_new_filter (QListWidgetItem * filter) {

   QString text = filter->text ();
   if (filter->listWidget () == _ui.messageTargetList) {

      if (filter->checkState () == Qt::Checked) { _targetFilterList << text; }
      else { _targetFilterList.removeOne (text); }
   }
   else {

      if (filter->checkState () == Qt::Checked) { _typeFilterList << text; }
      else { _typeFilterList.removeOne (text); }
   }

   QTreeWidgetItem *selectedMessage;
   QString messageType,messageTarget;
   for (int index = 0;
         index < _ui.treeWidgetList->topLevelItemCount (); ++index) {

      selectedMessage = _ui.treeWidgetList->topLevelItem (index);
      /*messageTarget = selectedMessage->text (1);
      messageType = selectedMessage->text (2);*/
      messageTarget = selectedMessage->text (0);
      messageType = selectedMessage->text (1);
      selectedMessage->setHidden (!_typeFilterList.contains (messageType)
                                  || !_targetFilterList.contains (messageTarget));
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
