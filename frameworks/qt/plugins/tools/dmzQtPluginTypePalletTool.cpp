#include "dmzQtPluginTypePalletTool.h"
#include <dmzQtUtil.h>
#include <dmzRuntimeConfigWrite.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>

dmz::QtPluginTypePalletTool::QtPluginTypePalletTool (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      MessageObserver (Info),
      QtWidget (Info),
      _log (Info),
      _labelCount (0) {

   _ui.setupUi (this);

   show ();
   raise ();

   _init (local);
}


dmz::QtPluginTypePalletTool::~QtPluginTypePalletTool () {

}


// Plugin Interface
void
dmz::QtPluginTypePalletTool::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

      RuntimeContext *context (get_plugin_runtime_context ());

      if (context) {

         Config session (get_plugin_name ());

         session.add_config (qbytearray_to_config ("geometry", saveGeometry ()));

         session.add_config (
            boolean_to_config ("window", "visible", isVisible () ? True : False));

         set_session_config (context, session);
      }
   }
}


void
dmz::QtPluginTypePalletTool::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Message Observer Interface
void
dmz::QtPluginTypePalletTool::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

}


// QtWidget Interface
void
dmz::QtPluginTypePalletTool::on_filter_textChanged (const QString &Text) {

   _ui.stack->setCurrentIndex (Text.isEmpty () ? 0 : 1);
   _proxyModel.setFilterFixedString (Text);
}


QWidget *
dmz::QtPluginTypePalletTool::get_qt_widget () { return this; }


// QtPluginTypePalletTool Interface
void
dmz::QtPluginTypePalletTool::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

/*
   _showMsg = config_create_message (
      "show.name",
      local,
      "DMZ_Show_Type_Pallet_Tool",
      context);

   subscribe_to_message (_showMsg);
*/

   if (context) {

      Config session (get_session_config (get_plugin_name (), context));

      QByteArray geometry (config_to_qbytearray ("geometry", session, saveGeometry ()));
      restoreGeometry (geometry);

      if (config_to_boolean ("window.visible", session, False)) { show (); }
   }

   _proxyModel.setSourceModel (&_listModel);
   _proxyModel.setDynamicSortFilter (True);
   _proxyModel.setFilterCaseSensitivity (Qt::CaseInsensitive);
   _proxyModel.setFilterKeyColumn (-1); // search all columns.

   _ui.typeTable->setModel (&_proxyModel);
   _ui.typeTree->setModel (&_model);

   QStandardItem *item = new QStandardItem;
   item->setData ("One", Qt::DisplayRole);
   _model.appendRow (item);
   _listModel.appendRow (item->clone ());

   QStandardItem *item2 = new QStandardItem;
   item2->setData ("Two", Qt::DisplayRole);
   item->appendRow (item2);
   _listModel.appendRow (item2->clone ());

   item2 = new QStandardItem;
   item2->setData ("Three", Qt::DisplayRole);
   item->appendRow (item2);
   _listModel.appendRow (item2->clone ());

   _ui.stack->setCurrentIndex (_ui.filter->text ().isEmpty () ? 0 : 1);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginTypePalletTool (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginTypePalletTool (Info, local);
}

};
