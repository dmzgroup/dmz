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
      _defaultAdapterList () {

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

}


void
dmz::QtPluginMapProperties::process_archive (
      const Handle ArchiveHandle,
      Config &local,
      Config &global) {

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
      
      const String Name (config_to_string ("name", data));

      if (Name) {
         
         AdapterItemStruct *ais = new AdapterItemStruct ();
         ais->config = data;
         ais->item = new QListWidgetItem (_ui.mapAdapterListWidget);
         ais->item->setText (Name.get_buffer ());
         
         _adapterList.append (ais);
         
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
   AdapterItemStruct *ais = _adapterList.takeAt (row);
   
   if (ais) {

      delete ais;
      ais = 0;
   }
   
   QListWidgetItem *item = _ui.mapAdapterListWidget->item (row);
   if (!item) { item = _ui.mapAdapterListWidget->item (0); }
   
   _ui.mapAdapterListWidget->setCurrentItem (item);
}


void
dmz::QtPluginMapProperties::on_emptyCacheButton_clicked () {

   if (_mapModule) {
      
      qApp->setOverrideCursor (QCursor (Qt::BusyCursor));
      _mapModule->empty_tile_cache ();
      qApp->restoreOverrideCursor ();
   }
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
         
qDebug () << url;
qWarning () << url.toString (QUrl::RemoveAuthority);
         _mapAdapter = _mapAdapter = new qmapcontrol::WMSMapAdapter (url.host (), url.toString (QUrl::RemoveAuthority), tileSize); 
      }
      
      // _mapAdapter = new qmapcontrol::TileMapAdapter (
      //    server.get_buffer (), path.get_buffer (), tileSize, minZoom, maxZoom);
      
      // _mapAdapter = new qmapcontrol::WMSMapAdapter("www2.demis.nl", "/wms/wms.ashx?Service=WMS&WMS=WorldMap&Version=1.1.0&Request=GetMap&SRS=EPSG:4326&Layers=Countries,Borders,Coastlines&Format=image/png", 256);

//      _mapAdapter = new qmapcontrol::WMSMapAdapter("www2.demis.nl", "/wms/wms.ashx?Service=WMS&WMS=BlueMarble&Version=1.1.0&Request=GetMap&SRS=EPSG:4326&Layers=Countries,Borders,Coastlines&Format=image/gif", 256);
//   	_mapAdapter = new qmapcontrol::WMSMapAdapter("openaerialmap.org", "/wms/?wms=WorldMap&LAYERS=world&FORMAT=image/jpeg&VERSION=1.1.1&SERVICE=WMS&REQUEST=GetMap&STYLES=&EXCEPTIONS=application/vnd.ogc.se_inimage&SRS=EPSG:4326&TRANSPARENT=FALSE", 256);
//   	_mapAdapter = new qmapcontrol::WMSMapAdapter("opentopomap.org", "/wms/?wms=WorldMap&LAYERS=world&FORMAT=image/jpeg&VERSION=1.1.1&SERVICE=WMS&REQUEST=GetMap&STYLES=&EXCEPTIONS=application/vnd.ogc.se_inimage&SRS=EPSG:4326&TRANSPARENT=FALSE", 256);

//      _mapAdapter = new qmapcontrol::WMSMapAdapter("172.20.90.188", "/tilecache/tilecache.py/?LAYERS=osm&FORMAT=image/png&SERVICE=WMS&VERSION=1.1.1&REQUEST=GetMap&STYLES=&EXCEPTIONS=application/Fvnd.ogc.se_inimage&SRS=EPSG:900913", 256);
//      _mapAdapter = new qmapcontrol::WMSMapAdapter("172.20.90.188", "/tilecache/tilecache.py/?LAYERS=dystopia&FORMAT=image/png&SERVICE=WMS&VERSION=1.1.1&REQUEST=GetMap&STYLES=&EXCEPTIONS=application/vnd.ogc.se_inimage&SRS=EPSG:4326&TRANSPARENT=FALSE", 256);
      
//      http://172.20.90.188/tilecache/tilecache.py/?LAYERS=osm&FORMAT=image%2Fpng&SERVICE=WMS&VERSION=1.1.1&REQUEST=GetMap&STYLES=&EXCEPTIONS=application%2Fvnd.ogc.se_inimage&SRS=EPSG%3A900913&BBOX=-15028131.0632,6887894.5228,-14401958.9036,7514066.6824&WIDTH=256&HEIGHT=256
      
      //http://172.20.90.188/tilecache/tilecache.py/?LAYERS=osm&FORMAT=image%2Fpng&SERVICE=WMS&VERSION=1.1.1&REQUEST=GetMap&STYLES=&EXCEPTIONS=application%2Fvnd.ogc.se_inimage&SRS=EPSG%3A900913&BBOX=-15654303.2228,1878517.246,-15028131.0632,2504689.4056&WIDTH=256&HEIGHT=256
      
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
dmz::QtPluginMapProperties::_add_query_item (QUrl &url, const QString &Key, const String &Value) {

   if (Value) { url.addQueryItem (Key, Value.get_buffer ()); }
}


void
dmz::QtPluginMapProperties::_save_session () {

   // String data;
   // 
   // Config session (get_plugin_name ());
   // 
   // Int32 count = _ui.mapAdapterListWidget->count ();
   // 
   // 
   // set_session_config (get_plugin_runtime_context (), session);
}


void
dmz::QtPluginMapProperties::_load_session () {

   Config session (
      get_session_config (get_plugin_name (), get_plugin_runtime_context ()));
   
   Config adapterList;
   
   if (session.is_empty ()) { session = _defaultAdapterList; }
   
   if (session.lookup_all_config ("map-adapter", adapterList)) {
      
      ConfigIterator it;
      Config adapterConfig;
      
      while (adapterList.get_next_config (it, adapterConfig)) {

         const String Name (config_to_string ("name", adapterConfig));

         if (Name) {
            
            AdapterItemStruct *ais = new AdapterItemStruct ();
            ais->config = adapterConfig;
            ais->item = new QListWidgetItem (_ui.mapAdapterListWidget);
            ais->item->setText (Name.get_buffer ());
            
            _adapterList.append (ais);
         }
      }
   }
   
   Int32 index = config_to_int32 ("current.index", session, 0);
   QListWidgetItem *item (_ui.mapAdapterListWidget->item (index));
   _ui.mapAdapterListWidget->setCurrentItem (item);
   _ui.mapAdapterListWidget->scrollToItem (item);
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
