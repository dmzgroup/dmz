#ifndef DMZ_QT_PLUGIN_CANVAS_ARCHIVE_DOT_H
#define DMZ_QT_PLUGIN_CANVAS_ARCHIVE_DOT_H

#include <dmzArchiveObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {
   
   class QtModuleCanvas;
   
   
   class QtPluginCanvasArchive :
         public Plugin,
         public ArchiveObserverUtil {
      
      public:
         QtPluginCanvasArchive (const PluginInfo &Info, Config &local);
         ~QtPluginCanvasArchive ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // ArchiveObserver Interface.
         virtual void create_archive (
            const Handle ArchiveHandle,
            Config &local,
            Config &global);

         virtual void process_archive (
            const Handle ArchiveHandle,
            Config &local,
            Config &global);

      protected:
         void _init (Config &local);

         Log _log;
         QtModuleCanvas *_canvasModule;
         String _canvasModuleName;
         Handle _archiveHandle;

      private:
         QtPluginCanvasArchive ();
         QtPluginCanvasArchive (const QtPluginCanvasArchive &);
         QtPluginCanvasArchive &operator= (const QtPluginCanvasArchive &);

   };
};

#endif // DMZ_QT_PLUGIN_CANVAS_ARCHIVE_DOT_H
