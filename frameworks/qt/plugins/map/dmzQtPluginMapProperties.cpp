#include <dmzArchiveModule.h>
#include <dmzQtModuleCanvas.h>
#include <dmzQtModuleMap.h>
#include <dmzQtModuleMainWindow.h>
#include "dmzQtPluginMapProperties.h"
#include "dmzQtPluginMapPropertiesMapAdapter.h"
#include <dmzQtUtil.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <dmzSystemFile.h>
#include <dmzSystemStreamString.h>
#include <qmapcontrol.h>
#include <QtGui/QtGui>

#include <QtCore/QDebug>

dmz::QtPluginMapProperties::QtPluginMapProperties (
      const PluginInfo &Info,
      Config &local) :
      QDialog (0),
      Plugin (Info),
      ArchiveObserverUtil (Info, local),
      MessageObserver (Info),
      _log (Info),
      _canvasModule (0),
      _canvasModuleName (),
      _mapModule (0),
      _mapModuleName (),
      _mainWindowModule (0),
      _mainWindowModuleName (),
      _propertiesEditMessage (),
      _adapterList (),
      _mapAdapter (0),
      _defaultAdapterList (),
      _timerId (0) {

   setObjectName (get_plugin_name ().get_buffer ());

   _ui.setupUi (this);

   _init (local);
}


dmz::QtPluginMapProperties::~QtPluginMapProperties () {

   while (!_adapterList.isEmpty ()) {
      
      AdapterItemStruct *ais = _adapterList.takeFirst ();
      if (ais) { delete ais; }
   }
}


// Plugin Interface
void
dmz::QtPluginMapProperties::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {
      
      if (_mapModule) {
         
         _ui.cacheDirLabel->setText (_mapModule->get_tile_cache_dir ().get_buffer ());
      }
   }
   else if (State == PluginStateStart) {

      _load_session ();
   }
   else if (State == PluginStateStop) {

      _save_session ();
   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::QtPluginMapProperties::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_canvasModule) {

         _canvasModule = QtModuleCanvas::cast (PluginPtr, _canvasModuleName);
      }

      if (!_mapModule) {

         _mapModule = QtModuleMap::cast (PluginPtr, _mapModuleName);
      }
      
      if (!_mainWindowModule) {

         _mainWindowModule = QtModuleMainWindow::cast (PluginPtr, _mainWindowModuleName);
         
         if (_mainWindowModule) {
            
            setParent (_mainWindowModule->get_widget (), Qt::Dialog);
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_canvasModule && (_canvasModule == QtModuleCanvas::cast (PluginPtr))) {

         _canvasModule = 0;
      }

      if (_mapModule && (_mapModule == QtModuleMap::cast (PluginPtr))) {

         if (_mapAdapter) {

            delete _mapAdapter;
            _mapAdapter = 0;
         }
         
         _mapModule = 0;
      }
      
      if (_mainWindowModule &&
          (_mainWindowModule == QtModuleMainWindow::cast (PluginPtr))) {

         setParent (0);
         _mainWindowModule = 0;
      }
   }
}


// ArchiveObserver Interface.
void
dmz::QtPluginMapProperties::create_archive (
      const Handle ArchiveHandle,
      Config &local,
      Config &global) {

   if (is_active_archive_handle (ArchiveHandle)) {

      Config map ("map");
      map.store_attribute (
         "on", _ui.mapCheckBox->isChecked () ? "true" : "false");

      local.add_config (map);

      if (_ui.mapCheckBox->isChecked ()) {

         AdapterItemStruct *ais = _adapterList.value (
            _ui.mapAdapterListWidget->currentRow ());
   
         if (ais) {
      
            local.add_config (ais->config);
         }
      }
   }
}


void
dmz::QtPluginMapProperties::process_archive (
      const Handle ArchiveHandle,
      Config &local,
      Config &global) {

   if (is_active_archive_handle (ArchiveHandle)) {

      _ui.mapCheckBox->setChecked (
         config_to_boolean ("map.on", local, _ui.mapCheckBox->isChecked ()));

      Config adapterConfig;
      if (local.lookup_config ("map-adapter", adapterConfig)) {
      
         AdapterItemStruct *ais (_lookup_adapter_item (adapterConfig)); 

         if (!ais) { ais = _create_adapter_item (adapterConfig); }

         if (ais) { _ui.mapAdapterListWidget->setCurrentItem (ais->item); }
      }
   }
}


// Message Observer Interface
void
dmz::QtPluginMapProperties::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   if (Type == _propertiesEditMessage) {

      qApp->setOverrideCursor (QCursor (Qt::BusyCursor));
      _update_cache_info ();
      qApp->restoreOverrideCursor ();

      exec ();
   }
}


void
dmz::QtPluginMapProperties::on_mapCheckBox_stateChanged (int state) {

   if (_mapModule && _canvasModule) {

      qmapcontrol::MapControl *map (_mapModule->get_map_control ());
      
      if (state) {

         qmapcontrol::ImageManager::instance ()->workOffline (false);
         _canvasModule->set_background_transparent (true);
         
         if (map) {
            
            map->show ();
            map->updateRequestNew ();
         }
      }
      else {
            
         if (map) { map->hide (); }
         
         qmapcontrol::ImageManager::instance ()->workOffline (true);
         _canvasModule->set_background_transparent (false);
      }
   }
}


void
dmz::QtPluginMapProperties::on_mapAdapterListWidget_itemActivated (QListWidgetItem *item) {
   
   if (_mapModule && item) {
      
      foreach (AdapterItemStruct *ais, _adapterList) {
         
         if (ais->item == item) {
            
            _update_adapter (ais->config);
            break;
         }
      }
   }
}


void
dmz::QtPluginMapProperties::on_mapAdapterListWidget_currentRowChanged (int currentRow) {

   if (currentRow == -1) {
      
      _ui.mapAdapterEditButton->setEnabled (False);
      _ui.mapAdapterDeleteButton->setEnabled (False);
      
      if (_mapModule) {
         
         _mapModule->use_default_map_adapter ();
         
         if (_mapAdapter) {
            
            delete _mapAdapter;
            _mapAdapter = 0;
         }
      }
   }
   else {
      
      _ui.mapAdapterEditButton->setEnabled (True);
      _ui.mapAdapterDeleteButton->setEnabled (True);
      
      AdapterItemStruct *ais = _adapterList.value (currentRow);
   
      if (ais) { _update_adapter (ais->config); }
   }
}


void
dmz::QtPluginMapProperties::on_mapAdapterAddButton_clicked () {

   QtPluginMapPropertiesMapAdapter dialog (this);
   
   if (dialog.exec () == QDialog::Accepted) {

      Config data ("map-adapter");
      dialog.to_config (data);
      
      AdapterItemStruct *ais (_create_adapter_item (data));

      if (ais) {

         _ui.mapAdapterListWidget->setCurrentItem (ais->item);
      }
   }
}


void
dmz::QtPluginMapProperties::on_mapAdapterEditButton_clicked () {

   AdapterItemStruct *ais = _adapterList.value (_ui.mapAdapterListWidget->currentRow ());
   
   if (ais) {
      
      QtPluginMapPropertiesMapAdapter dialog (this);

      dialog.from_config (ais->config);

      if (dialog.exec () == QDialog::Accepted) {
         
         Config data ("map-adapter");
         dialog.to_config (data);
         ais->config = data;
         
         _update_adapter (ais->config);
      }
   }
}


void
dmz::QtPluginMapProperties::on_mapAdapterDeleteButton_clicked () {

   Int32 row = _ui.mapAdapterListWidget->currentRow ();

   AdapterItemStruct *prevItemStruct = _adapterList.value (row - 1);
   if (!prevItemStruct) { prevItemStruct = _adapterList.value (0); }
   
   if (prevItemStruct) {
      
      _ui.mapAdapterListWidget->setCurrentItem (prevItemStruct->item);
   }

   AdapterItemStruct *itemStruct = _adapterList.takeAt (row);
   
   if (itemStruct) {

      delete itemStruct;
      itemStruct = 0;
   }
}


void
dmz::QtPluginMapProperties::on_emptyCacheButton_clicked () {

   if (_mapModule) {
      
      qApp->setOverrideCursor (QCursor (Qt::BusyCursor));
      _mapModule->empty_tile_cache ();
      _update_cache_info ();
      qApp->restoreOverrideCursor ();
   }
}


void
dmz::QtPluginMapProperties::showEvent (QShowEvent *event) {

   _timerId = startTimer (5000);
   QWidget::showEvent (event);
}


void
dmz::QtPluginMapProperties::closeEvent (QCloseEvent * event) {

   killTimer (_timerId);
   _timerId = 0;
   QWidget::closeEvent (event);
}


void
dmz::QtPluginMapProperties::timerEvent (QTimerEvent *event) {

   _update_cache_info ();
}


dmz::QtPluginMapProperties::AdapterItemStruct *
dmz::QtPluginMapProperties::_lookup_adapter_item (const Config &Adapter) {

   AdapterItemStruct *itemStruct (0);

   foreach (AdapterItemStruct *ais, _adapterList) {

      if (Adapter.are_attributes_equal (ais->config)) {

         itemStruct = ais;
         break;
      }
   }

   return (itemStruct);
}


dmz::QtPluginMapProperties::AdapterItemStruct *
dmz::QtPluginMapProperties::_create_adapter_item (const Config &Adapter) {

   AdapterItemStruct *itemStruct (0);

   const String Name (config_to_string ("name", Adapter));

   if (Name) {
            
      itemStruct = new AdapterItemStruct ();
      itemStruct->config = Adapter;
      itemStruct->item = new QListWidgetItem (_ui.mapAdapterListWidget);
      itemStruct->item->setText (Name.get_buffer ());
         
      _adapterList.append (itemStruct);
   }

   return (itemStruct);
}


void
dmz::QtPluginMapProperties::_update_adapter (const Config &Adapter) {
   
   if (_mapModule) {
      
      qmapcontrol::MapAdapter *oldAdapter = _mapAdapter;
      _mapAdapter = 0;
      
      String type = config_to_string ("type", Adapter, "tile").get_lower ();
      String server = config_to_string ("server", Adapter);
      
      if (type == "tile") {

         String path = config_to_string ("path", Adapter);
         Int32 tileSize = config_to_int32 ("tileSize", Adapter);
         Int32 minZoom = config_to_int32 ("minZoom", Adapter);
         Int32 maxZoom = config_to_int32 ("maxZoom", Adapter);
      
         _mapAdapter = _mapAdapter = new qmapcontrol::TileMapAdapter (
            server.get_buffer (), path.get_buffer (), tileSize, minZoom, maxZoom);
      }
      else if (type == "wms") {
         
         QUrl url;
         url.setHost (server.get_buffer ());
         url.setPath (config_to_string ("path", Adapter).get_buffer ());
         
         _add_query_item (url, "SERVICE", config_to_string ("service", Adapter));
         _add_query_item (url, "WMS", config_to_string ("wms", Adapter));
         _add_query_item (url, "VERSION", config_to_string ("version", Adapter, "1.1.0"));
         _add_query_item (url, "REQUEST", config_to_string ("request", Adapter, "GetMap"));
         _add_query_item (url, "SRS", config_to_string ("projection", Adapter));
         _add_query_item (url, "LAYERS", config_to_string ("layers", Adapter));
         _add_query_item (url, "STYLES", config_to_string ("styles", Adapter));
         _add_query_item (url, "FORMAT", config_to_string ("imageFormat", Adapter));
         
         Int32 tileSize = config_to_int32 ("tileSize", Adapter, 256);

         QString path (url.toString (QUrl::RemoveAuthority));
         
         _log.debug << "WMSMapAdapter: " << qPrintable (path) << endl;
                       
         _mapAdapter = _mapAdapter = new qmapcontrol::WMSMapAdapter (
            url.host (), path, tileSize); 
      }
      
      if (_mapAdapter) {
         
         _mapModule->set_map_adapter (_mapAdapter);
      
         if (oldAdapter) {
         
            delete oldAdapter;
            oldAdapter = 0;
         }
      }
   }
}


void
dmz::QtPluginMapProperties::_add_query_item (
      QUrl &url,
      const QString &Key,
      const String &Value) {

   if (Value) { url.addQueryItem (Key, Value.get_buffer ()); }
}


void
dmz::QtPluginMapProperties::_update_cache_info () {

   if (_mapModule) {

      String CacheDir (_mapModule->get_tile_cache_dir ()); 

      Int64 size (0);
      Int64 count (0);

      String file;
      PathContainer fileList;

      if (get_file_list (CacheDir, fileList)) {
      
         count = fileList.get_count ();

         Boolean found (fileList.get_first (file));
            
         while (found) {
      
            const String CleanPath (format_path (CacheDir + "/" + file));
            if (get_absolute_path (CleanPath, file))  {

               size += get_file_size (file);
            }
         
            found = fileList.get_next (file);
         }

         Float64 sizeInMb = Float64 (size) / 1048576;

         QString info =
            QString ("%1 items, totalling %2 MB").arg (count).arg (sizeInMb, 0, 'f', 1);

         _ui.cacheInfoLabel->setText (info);
         _ui.cacheInfoLabel->show ();
      }
   }
   else {

      _ui.cacheInfoLabel->hide ();
   }
}


void
dmz::QtPluginMapProperties::_save_session () {

   String data;
    
   Config session (get_plugin_name ());

   Config adapterList ("map-adapter-list");
   session.add_config (adapterList);
   
   foreach (AdapterItemStruct *item, _adapterList) {
      
      adapterList.add_config (item->config);
   } 
   
   Config current ("current");
   data.flush () << _ui.mapAdapterListWidget->currentRow ();
   current.store_attribute ("index", data);
   adapterList.add_config (current);

   Config map ("map");
   map.store_attribute ("on", _ui.mapCheckBox->isChecked () ? "true" : "false");
   session.add_config (map);
   
   set_session_config (get_plugin_runtime_context (), session);
}


void
dmz::QtPluginMapProperties::_load_session () {

   Config session (
      get_session_config (get_plugin_name (), get_plugin_runtime_context ()));
   
   Config adapterList;
   if (!session.lookup_config ("map-adapter-list", adapterList)) {
      
      adapterList = _defaultAdapterList;
   }
   
   Config data;
   if (adapterList.lookup_all_config ("map-adapter", data)) {
      
      ConfigIterator it;
      Config adapterConfig;
      
      while (data.get_next_config (it, adapterConfig)) {

         AdapterItemStruct *ais (_create_adapter_item (adapterConfig));
      }
   }

   Int32 index = config_to_int32 ("current.index", adapterList);
   QListWidgetItem *item (_ui.mapAdapterListWidget->item (index));
   _ui.mapAdapterListWidget->setCurrentItem (item);
   _ui.mapAdapterListWidget->scrollToItem (item);
   
   _ui.mapCheckBox->setChecked (config_to_boolean ("map.on", session, True));
}


void
dmz::QtPluginMapProperties::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   _mainWindowModuleName = config_to_string (
      "module.mainWindow.name",
      local,
      "dmzQtModuleMainWindowBasic");
      
   _canvasModuleName = config_to_string (
      "module.canvas.name",
      local,
      "dmzQtModuleCanvasBasic");

   _mapModuleName = config_to_string (
      "module.map.name",
      local,
      "dmzQtModuleMapBasic");
   
   init_archive (local);
   
   _propertiesEditMessage = config_create_message (
      "message.mapPropertiesEdit.name",
      local,
      "MapPropertiesEditMessage",
      context,
      &_log);

   subscribe_to_message (_propertiesEditMessage);
   
   local.lookup_config ("default-map-adapter-list", _defaultAdapterList);
   
   init_archive (local);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginMapProperties (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginMapProperties (Info, local);
}

};
