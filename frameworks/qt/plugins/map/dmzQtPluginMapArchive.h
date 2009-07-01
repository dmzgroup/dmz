#ifndef DMZ_QT_PLUGIN_MAP_ARCHIVE_DOT_H
#define DMZ_QT_PLUGIN_MAP_ARCHIVE_DOT_H

#include <dmzArchiveObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class QtModuleMap;


   class QtPluginMapArchive :
         public Plugin,
         public ArchiveObserverUtil {

      public:
         QtPluginMapArchive (const PluginInfo &Info, Config &local);
         ~QtPluginMapArchive ();

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
         QtModuleMap *_mapModule;
         String _mapModuleName;

      private:
         QtPluginMapArchive ();
         QtPluginMapArchive (const QtPluginMapArchive &);
         QtPluginMapArchive &operator= (const QtPluginMapArchive &);

   };
};

#endif // DMZ_QT_PLUGIN_MAP_ARCHIVE_DOT_H
