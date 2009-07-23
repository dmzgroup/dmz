#include <dmzArchiveModule.h>
#include <dmzQtModuleCanvas.h>
#include <dmzQtModuleMap.h>
#include <dmzQtModuleMainWindow.h>
#include "dmzQtPluginMapProperties.h"
#include <dmzQtUtil.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzSystemFile.h>
#include <dmzSystemStreamString.h>
#include <qmapcontrol.h>
#include <QtGui/QtGui>
#include "ui_dmzQtPluginMapPropertiesTileMapAdapter.h"


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
      _propertiesEditMessage () {

   setObjectName (get_plugin_name ().get_buffer ());

   _ui.setupUi (this);

   _init (local);
}


dmz::QtPluginMapProperties::~QtPluginMapProperties () {

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

   if (Type == _propertiesEditMessage && _mapModule && _canvasModule) {

      qmapcontrol::MapControl *map (_mapModule->get_map_control ());
      
      if (exec () == QDialog::Accepted) {
         
         // if (_ui.mapCheckBox->isChecked ()) {
         // 
         //    qmapcontrol::ImageManager::instance ()->workOffline (false);
         //    _canvasModule->set_background_transparent (true);
         //    map->show ();
         //    if (map) { map->updateRequestNew (); }
         // }
         // else {
         //    
         //    map->hide ();
         //    qmapcontrol::ImageManager::instance ()->workOffline (true);
         //    _canvasModule->set_background_transparent (false);
         // }
   
         if (_mapModule) {
            
             // qmapcontrol::TileMapAdapter *adapter = new qmapcontrol::TileMapAdapter (
             //   "tile.openstreetmap.org",
             //   "/%1/%2/%3.png",
             //   256,
             //   0,
             //   17);
               
//            qmapcontrol::TileMapAdapter *adapter = new qmapcontrol::OSMMapAdapter ();
//            qmapcontrol::TileMapAdapter *adapter = new qmapcontrol::GoogleMapAdapter ();
//            _mapModule->set_map_adapter (adapter);
         }
      }
      else {
         
         if (_mapModule) {
            
//            qmapcontrol::TileMapAdapter *adapter = new qmapcontrol::YahooMapAdapter();
//            qmapcontrol::TileMapAdapter *adapter = new qmapcontrol::YahooMapAdapter("us.maps3.yimg.com", "/aerial.maps.yimg.com/png?v=1.7&t=a&s=256&x=%2&y=%3&z=%1");
//            _mapModule->set_map_adapter (adapter);
//            _mapModule->use_default_map_adapter ();
         }
      }
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
dmz::QtPluginMapProperties::on_mapAdapterAddButton_clicked () {
  
}


void
dmz::QtPluginMapProperties::on_mapAdapterEditButton_clicked () {
   
}


void
dmz::QtPluginMapProperties::on_mapAdapterDeleteButton_clicked () {
   
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
dmz::QtPluginMapProperties::_save_session () {

   // String data;
   // 
   // Config session (get_plugin_name ());
   // 
   // session.add_config (
   //    qbytearray_to_config ("geometry", saveGeometry ()));
   // 
   // session.add_config (qbytearray_to_config ("state", saveState (LocalSessionVersion)));
   // 
   // set_session_config (get_plugin_runtime_context (), session);
}


void
dmz::QtPluginMapProperties::_load_session () {

   // Config session (
   //    get_session_config (get_plugin_name (), get_plugin_runtime_context ()));
   // 
   // QByteArray geometry (config_to_qbytearray ("geometry", session, saveGeometry ()));
   // restoreGeometry (geometry);
   // 
   // QByteArray stateData (
   //    config_to_qbytearray ("state", session, saveState (LocalSessionVersion)));
   // 
   // restoreState (stateData, LocalSessionVersion);
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
