#ifndef DMZ_ARCHIVE_PLUGIN_QUICK_SAVE_DOT_H
#define DMZ_ARCHIVE_PLUGIN_QUICK_SAVE_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class ArchiveModule;

   class ArchivePluginQuickSave :
         public Plugin,
         public InputObserverUtil {

      public:
         //! \cond
         ArchivePluginQuickSave (const PluginInfo &Info, Config &local);
         ~ArchivePluginQuickSave ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin () {;}
         virtual void stop_plugin () {;}
         virtual void shutdown_plugin () {;}
         virtual void remove_plugin (const Plugin *PluginPtr);

         // Input Observer Interface
         virtual void receive_key_event (
            const Handle Channel,
            const InputEventKey &Value);

      protected:
         void _init (Config &local);

         String _dumpFile;
         ArchiveModule *_archiveMod;
         Handle _defaultHandle;
         UInt32 _key;
         Log _log;
         //! \endcond

      private:
         ArchivePluginQuickSave ();
         ArchivePluginQuickSave (const ArchivePluginQuickSave &);
         ArchivePluginQuickSave &operator= (const ArchivePluginQuickSave &);
   };
};

#endif // DMZ_ARCHIVE_PLUGIN_QUICK_SAVE_DOT_H
