#include "dmzQtPluginList.h"
#include <dmzQtUtil.h>
#include <dmzRuntimeConfigWrite.h>
#include <dmzRuntimeModule.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <dmzSystemDynamicLibrary.h>

dmz::QtPluginList::QtPluginList (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      PluginObserver (Info),
      MessageObserver (Info),
      _log (Info),
      _labelCount (0) {

   _ui.setupUi (this);

   _init (local);
}


dmz::QtPluginList::~QtPluginList () {

   _itemTable.clear ();
}


// Plugin Interface
void
dmz::QtPluginList::update_plugin_state (
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
dmz::QtPluginList::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// PluginObserver Interface
void
dmz::QtPluginList::update_runtime_plugin (
      const PluginDiscoverEnum Mode,
      const Handle RuntimeModuleHandle,
      const Handle PluginHandle) {

   if (Mode == PluginDiscoverAdd) {

      RuntimeModule *rm = lookup_runtime_module (RuntimeModuleHandle);

      if (rm) {

         const PluginInfo *Info = rm->lookup_plugin_info (PluginHandle);

         if (Info) {

            QStandardItemList list;

            QStandardItem *item (new QStandardItem);
            item->setData ((quint64)PluginHandle, Qt::DisplayRole);
            item->setEditable (false);
            list.append (item);

            if (!_itemTable.store (PluginHandle, item)) {

               _log.error << "Failed to register plugin: " << Info->get_name () << endl;
            }

            item = new QStandardItem;
            item->setData (Info->get_name ().get_buffer (), Qt::DisplayRole);
            item->setEditable (false);
            list.append (item);

            item = new QStandardItem;
            item->setData (Info->get_scope_name ().get_buffer (), Qt::DisplayRole);
            item->setEditable (false);
            list.append (item);

            item = new QStandardItem;
            item->setData (Info->get_class_name ().get_buffer (), Qt::DisplayRole);
            item->setEditable (false);
            list.append (item);

            item = new QStandardItem;
            QString mode ("Yes");
            if (Info->get_delete_mode () == PluginDeleteModeDoNotDelete) { mode = "No"; }
            item->setData (mode, Qt::DisplayRole);
            item->setEditable (false);
            list.append (item);

            DynamicLibrary *dl = Info->get_dynamic_library ();

            if (dl) {

               mode = dl->get_mode () == DynamicLibraryModeUnload ?
                  "Reloadable" : "Locked";
               item = new QStandardItem;
               item->setData (mode, Qt::DisplayRole);
               item->setEditable (false);
               list.append (item);

               item = new QStandardItem;
               item->setData (dl->get_name ().get_buffer (), Qt::DisplayRole);
               item->setEditable (false);
               list.append (item);
            }

            _model.appendRow (list);
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      QStandardItem *item = _itemTable.remove (PluginHandle);

      if (item) {

         QStandardItemList row = _model.takeRow (item->row ());
         qDeleteAll (row);
      }
   }

   QHeaderView *header (_ui.pluginTable->horizontalHeader ());

   if (header) {

      for (int ix = 0; ix < _labelCount; ix++) {

         header->setResizeMode (ix, QHeaderView::ResizeToContents);
      }
   }

   if (_baseTitle.isEmpty () == false) {

      setWindowTitle (_baseTitle + " (" +
         QString::number (_itemTable.get_count ()) + ")");
   }
}


// Message Observer Interface
void
dmz::QtPluginList::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   show ();
   raise ();
}


// QtPluginList Interface
void
dmz::QtPluginList::on_unloadButton_clicked () {

}


void
dmz::QtPluginList::on_reloadButton_clicked () {

}


void     
dmz::QtPluginList::on_filter_textChanged (const QString &Text) {

   _proxyModel.setFilterFixedString (Text);
}


void
dmz::QtPluginList::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   _showMsg = config_create_message (
      "show.name",
      local,
      "DMZ_Show_Plugin_List_Console",
      context);

   subscribe_to_message (_showMsg);

   if (context) {

      Config session (get_session_config (get_plugin_name (), context));

      QByteArray geometry (config_to_qbytearray ("geometry", session, saveGeometry ()));
      restoreGeometry (geometry);

      if (config_to_boolean ("window.visible", session, False)) { show (); }
   }

   _proxyModel.setSourceModel (&_model);
   _proxyModel.setDynamicSortFilter (True);
   _proxyModel.setFilterCaseSensitivity (Qt::CaseInsensitive);
   _proxyModel.setFilterKeyColumn (-1); // search all columns.
         
   _ui.pluginTable->setModel (&_proxyModel);

   QStringList labels;
   labels << "Id" << "Name" << "Scope" << "Class" << "Deletable"
      << "Reloadable" << "Library";

   _labelCount = labels.count ();

   _model.setHorizontalHeaderLabels (labels);

   QHeaderView *header (_ui.pluginTable->horizontalHeader ());

   if (header) {

      for (int ix = 0; ix < _labelCount; ix++) {

         header->setResizeMode (ix, QHeaderView::ResizeToContents);
      }
   }

   _baseTitle = windowTitle ();

   if (_itemTable.get_count () > 0) {

      setWindowTitle (_baseTitle + " (" +
         QString::number (_itemTable.get_count ()) + ")");
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginList (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginList (Info, local);
}

};
