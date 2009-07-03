#include <dmzArchiveModule.h>
#include <dmzQtConfigRead.h>
#include <dmzQtConfigWrite.h>
#include <dmzQtModuleMap.h>
#include "dmzQtPluginMapArchive.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigWrite.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <qmapcontrol.h>
#include <QtGui/QtGui>


dmz::QtPluginMapArchive::QtPluginMapArchive (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ArchiveObserverUtil (Info, local),
      _log (Info),
      _mapModule (0),
      _mapModuleName () {

   _init (local);
}


dmz::QtPluginMapArchive::~QtPluginMapArchive () {

}


// Plugin Interface
void
dmz::QtPluginMapArchive::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_mapModule) {

         _mapModule = QtModuleMap::cast (PluginPtr, _mapModuleName);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_mapModule && (_mapModule == QtModuleMap::cast (PluginPtr))) {

         _mapModule = 0;
      }
   }
}


// ArchiveObserver Interface.
void
dmz::QtPluginMapArchive::create_archive (
      const Handle ArchiveHandle,
      Config &local,
      Config &global) {

   if (is_active_archive_handle (ArchiveHandle)) {

      if (_mapModule) {

         qmapcontrol::MapControl *map (_mapModule->get_map_control ());

         if (map) {
            
            Config result ("mapControl");

            result.add_config (qpointf_to_config ("center", map->currentCoordinate ()));
            result.add_config (int32_to_config ("zoom", map->currentZoom ()));
         
            local.add_config (result);
         }
      }
   }
}


void
dmz::QtPluginMapArchive::process_archive (
      const Handle ArchiveHandle,
      Config &local,
      Config &global) {

   if (is_active_archive_handle (ArchiveHandle)) {

      if (_mapModule) {

         qmapcontrol::MapControl *map (_mapModule->get_map_control ());

         if (map) {
            
            Config cd;
            local.lookup_config ("mapControl", cd);
            
            QPointF center (config_to_qpointf ("center", cd, map->currentCoordinate ()));
            map->setView (center);
            
            Int32 zoom (config_to_int32 ("zoom.value", cd, map->currentZoom ()));
            map->setZoom (zoom);
         }
      }
   }
}


void
dmz::QtPluginMapArchive::_init (Config &local) {

   init_archive (local);

   _mapModuleName = config_to_string ("module.map.name", local);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginMapArchive (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginMapArchive (Info, local);
}

};
