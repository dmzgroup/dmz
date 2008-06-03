#ifndef DMZ_ARCHIVE_PLUGIN_UNDO_DOT_H
#define DMZ_ARCHIVE_PLUGIN_UNDO_DOT_H

#include <dmzArchiveObserverUtil.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeUndo.h>

namespace dmz {

   class ArchivePluginUndo :
         public Plugin,
         public ArchiveObserverUtil,
         public UndoDump {

      public:
         ArchivePluginUndo (const PluginInfo &Info, Config &local);
         ~ArchivePluginUndo ();

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

         // UndoDump Interface.
         virtual void start_record (const Handle RecordHandle, const String &Name);

         virtual void store_action (
            const Message &Type,
            const Handle Target,
            const Data *Value);

      protected:
         void _init (Config &local);

         Config _local;
         Config _step;
         Definitions _defs;
         Undo _undo;
         Log _log;

      private:
         ArchivePluginUndo ();
         ArchivePluginUndo (const ArchivePluginUndo &);
         ArchivePluginUndo &operator= (const ArchivePluginUndo &);

   };
};

#endif // DMZ_ARCHIVE_PLUGIN_UNDO_DOT_H
