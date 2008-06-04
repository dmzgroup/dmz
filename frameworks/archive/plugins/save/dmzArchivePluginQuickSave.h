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
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

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
