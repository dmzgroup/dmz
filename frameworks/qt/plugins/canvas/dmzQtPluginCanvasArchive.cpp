#include <dmzArchiveModule.h>
#include <dmzQtConfigRead.h>
#include <dmzQtConfigWrite.h>
#include <dmzQtCanvasView.h>
#include <dmzQtModuleCanvas.h>
#include "dmzQtPluginCanvasArchive.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <QtGui/QtGui>


dmz::QtPluginCanvasArchive::QtPluginCanvasArchive (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ArchiveObserverUtil (Info, local),
      _log (Info),
      _canvasModule (0),
      _canvasModuleName () {

   _init (local);
}


dmz::QtPluginCanvasArchive::~QtPluginCanvasArchive () {

}


// Plugin Interface
void
dmz::QtPluginCanvasArchive::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_canvasModule) {

         _canvasModule = QtModuleCanvas::cast (PluginPtr, _canvasModuleName);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_canvasModule && (_canvasModule == QtModuleCanvas::cast (PluginPtr))) {

         _canvasModule = 0;
      }
   }
}


// ArchiveObserver Interface.
void
dmz::QtPluginCanvasArchive::create_archive (
      const Handle ArchiveHandle,
      const Int32 Version,
      Config &local,
      Config &global) {

   if (is_active_archive_handle (ArchiveHandle)) {

      if (_canvasModule) {

         QGraphicsView *view (_canvasModule->get_view ());

         if (view) {

            QtCanvasView *canvas = dynamic_cast<QtCanvasView *> (view);

            if (canvas) {

               local.add_config (qgraphicsview_to_config ("canvasView", *canvas));
            }
         }
      }
   }
}


void
dmz::QtPluginCanvasArchive::process_archive (
      const Handle ArchiveHandle,
      const Int32 Version,
      Config &local,
      Config &global) {

   if (is_active_archive_handle (ArchiveHandle)) {

      if (_canvasModule) {

         QGraphicsView *view (_canvasModule->get_view ());

         if (view) {

            QtCanvasView *canvas = dynamic_cast<QtCanvasView *> (view);

            if (canvas) {

               qgraphicsview_config_read ("canvasView", local, canvas);
               canvas->set_scale (canvas->get_scale ());
            }
         }
      }
   }
}


void
dmz::QtPluginCanvasArchive::_init (Config &local) {

   init_archive (local);

   _canvasModuleName = config_to_string ("module.canvas.name", local);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginCanvasArchive (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginCanvasArchive (Info, local);
}

};
