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
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

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
