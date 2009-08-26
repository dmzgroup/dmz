#ifndef DMZ_ARCHIVE_PLUGIN_INPUT_CHANNEL_STATE_DOT_H
#define DMZ_ARCHIVE_PLUGIN_INPUT_CHANNEL_STATE_DOT_H

#include <dmzArchiveObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHandleContainer.h>

namespace dmz {

   class InputModule;

   class ArchivePluginInputChannelState :
         public Plugin,
         public ArchiveObserverUtil {

      public:
         ArchivePluginInputChannelState (const PluginInfo &Info, Config &local);
         ~ArchivePluginInputChannelState ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // ArchiveObserver Interface.
         virtual void create_archive (
            const Handle ArchiveHandle,
            const Int32 Version,
            Config &local,
            Config &global);

         virtual void process_archive (
            const Handle ArchiveHandle,
            const Int32 Version,
            Config &local,
            Config &global);

      protected:
         void _init (Config &local);

         Log _log;
         InputModule *_input;
         String _inputModuleName;
         HandleContainer _channels;

      private:
         ArchivePluginInputChannelState ();
         ArchivePluginInputChannelState (const ArchivePluginInputChannelState &);
         ArchivePluginInputChannelState &operator= (
            const ArchivePluginInputChannelState &);
   };
};

#endif // DMZ_ARCHIVE_PLUGIN_INPUT_CHANNEL_STATE_DOT_H
