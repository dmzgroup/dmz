#include "dmzQtPluginList.h"
#include <dmzQtUtil.h>
#include <dmzRuntimeConfigWrite.h>
#include <dmzRuntimeLoadPlugins.h>
#include <dmzRuntimeModule.h>
#include <dmzRuntimePluginContainer.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <dmzSystemDynamicLibrary.h>
#include <dmzTypesHandleContainer.h>

dmz::QtPluginList::QtPluginList (const PluginInfo &Info, Config &local, Config &global) :
      Plugin (Info),
      PluginObserver (Info),
      MessageObserver (Info),
      _log (Info),
      _global (global),
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

      const Boolean IsSelf (PluginHandle == get_plugin_handle ());

      RuntimeModule *rm = lookup_runtime_module (RuntimeModuleHandle);

      if (rm) {

         _rmTable.store (PluginHandle, rm);

         const PluginInfo *Info = rm->lookup_plugin_info (PluginHandle);

         if (Info) {

            const Boolean Selectable = IsSelf ? False :
               (Info->get_delete_mode () == PluginDeleteModeDoNotDelete ? False : True);

            QStandardItemList list;

            QStandardItem *item (new QStandardItem);
            item->setData ((quint64)PluginHandle, Qt::DisplayRole);
            item->setEditable (false);
            item->setSelectable (Selectable);
            item->setEnabled (Selectable);
            list.append (item);

            if (!_itemTable.store (PluginHandle, item)) {

               _log.error << "Failed to register plugin: " << Info->get_name () << endl;
            }

            item = new QStandardItem;
            item->setData (Info->get_name ().get_buffer (), Qt::DisplayRole);
            item->setEditable (false);
            item->setSelectable (Selectable);
            item->setEnabled (Selectable);
            list.append (item);

            item = new QStandardItem;
            item->setData (Info->get_scope_name ().get_buffer (), Qt::DisplayRole);
            item->setEditable (false);
            item->setSelectable (Selectable);
            item->setEnabled (Selectable);
            list.append (item);

            item = new QStandardItem;
            item->setData (Info->get_class_name ().get_buffer (), Qt::DisplayRole);
            item->setEditable (false);
            item->setSelectable (Selectable);
            item->setEnabled (Selectable);
            list.append (item);

            item = new QStandardItem;
            QString mode ("Yes");

            if (IsSelf || Info->get_delete_mode () == PluginDeleteModeDoNotDelete) {

               mode = "No";
            }

            item->setData (mode, Qt::DisplayRole);
            item->setEditable (false);
            item->setSelectable (Selectable);
            item->setEnabled (Selectable);
            list.append (item);

            DynamicLibrary *dl = Info->get_dynamic_library ();

            if (dl) {

               mode = dl->get_mode () == DynamicLibraryModeUnload ?
                  "Reloadable" : "Locked";
               if (IsSelf) { mode = "Locked"; }
               item = new QStandardItem;
               item->setData (mode, Qt::DisplayRole);
               item->setEditable (false);
               item->setSelectable (Selectable);
               item->setEnabled (Selectable);
               list.append (item);

               item = new QStandardItem;
               item->setData (dl->get_name ().get_buffer (), Qt::DisplayRole);
               item->setEditable (false);
               item->setSelectable (Selectable);
               item->setEnabled (Selectable);
               list.append (item);
            }

            _model.appendRow (list);
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      _rmTable.remove (PluginHandle);

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

   HandleContainer list;

   _get_selected (list);

   HandleContainerIterator it;
   Handle plugin (0);

   while (list.get_next (it, plugin)) {

      RuntimeModule *rm (_rmTable.lookup (plugin));

      if (rm) { rm->unload_plugin (plugin); }
      else { _log.error << "RuntimeModule not found for: " << plugin << endl; }
   }
}


void
dmz::QtPluginList::on_reloadButton_clicked () {

   HandleContainer list;

   _get_selected (list);

   Config pluginList ("plugin-list");

   RuntimeModule *firstRm (0);

   HandleContainerIterator it;
   Handle plugin (0);

   while (list.get_next (it, plugin)) {

      RuntimeModule *rm (_rmTable.lookup (plugin));

      if (!firstRm) { firstRm = rm; }

      if (rm && (rm == firstRm)) {

         const PluginInfo *Info = rm->lookup_plugin_info (plugin);
         DynamicLibrary *dl = (Info ? Info->get_dynamic_library () : 0);

         if (Info && dl) {

            Config data ("plugin");
            data.store_attribute ("name", Info->get_class_name ());
            data.store_attribute ("scope", Info->get_scope_name ());
            data.store_attribute ("unique", Info->get_name ());
            data.store_attribute ("factory", Info->get_factory_name ());
            data.store_attribute ("library", dl->get_name ());

            pluginList.add_config (data);

            rm->unload_plugin (plugin);
         }
         else {

            if (!Info) {

               _log.error << "PluginInfo not found for plugin: " << plugin << endl;
            }

            if (!dl) {

               _log.error << "Plugin "
                  << (Info ? Info->get_name () : String::number (plugin))
                  << " not loaded from a Dynamic Library." << endl;
            }
         }
      }
      else if (rm && (rm != firstRm)) {

      }
      else { _log.error << "RuntimeModule not found for: " << plugin << endl; }
   }

   if (firstRm) {

      RuntimeContext *context (get_plugin_runtime_context ());

      PluginContainer container (context, &_log);
      Config init ("dmz");
      _global.lookup_all_config_merged ("dmz", init);

      if (load_plugins (context, pluginList, init, _global, container, &_log)) {

         if (!firstRm->add_plugins (container)) {

            _log.error << "Failed adding plugins to Runtime." << endl;
         }
      }
      else {

         _log.error << "Failed reloading plugins." << endl;
      }
   }
}


void     
dmz::QtPluginList::on_filter_textChanged (const QString &Text) {

   _proxyModel.setFilterFixedString (Text);
}


void
dmz::QtPluginList::_get_selected (HandleContainer &list) {

   QItemSelectionModel *selected = _ui.pluginTable->selectionModel ();

   if (selected) {

      QModelIndexList indexes = selected->selectedRows ();

      for (int ix = 0; ix < indexes.size (); ix++) {

         QStandardItem *item =
            _model.itemFromIndex (_proxyModel.mapToSource (indexes[ix]));

         if (item) {

            const Handle Value = (Handle)item->data (Qt::DisplayRole).toUInt ();

            list.add (Value);
         }
         else { _log.error << "No item found for selection index" << endl; }
      }
   }
   else  { _log.error << "No selection model." << endl; }
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

   return new dmz::QtPluginList (Info, local, global);
}

};
