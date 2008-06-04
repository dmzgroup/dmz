#ifndef DMZ_ARCHIVE_PLUGIN_INPUT_DOT_H
#define DMZ_ARCHIVE_PLUGIN_INPUT_DOT_H

#include <dmzRuntimeConfig.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeSync.h>
#include <dmzRuntimeTime.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableUInt32Template.h>

namespace dmz {

// Not commenting until Plugin finalized.
//! \cond
   class ArchivePluginInput : public Plugin, public Sync {

      public:
         ArchivePluginInput (const PluginInfo &Info, Config &local);
         ~ArchivePluginInput ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Sync Interface
         virtual void update_sync (const Float64 DeltaTime);

      protected:
         struct ChannelState;
         struct EventStruct;

         void _config_to_channel_state (Config &eventData);
         void _config_to_axis_event (Config &eventData);
         void _config_to_button_event (Config &eventData);
         void _config_to_key_event (Config &eventData);
         void _config_to_event (Config &eventData);

         void _add_event (EventStruct *event);
         void _destroy_events ();
         void _init (Config &local);

         Log _log;
         Definitions _defs;
         
         EventStruct *_eventList;
         EventStruct *_eventListTail;
         
         InputModule *_channels;
         Config _localData;

         Handle _activeChannel;

      private:
         ArchivePluginInput ();
         ArchivePluginInput (const ArchivePluginInput &);
         ArchivePluginInput &operator= (const ArchivePluginInput &);

   };
//! \endcond
};

#endif // DMZ_ARCHIVE_PLUGIN_INPUT_DOT_H
