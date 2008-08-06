#ifndef DMZ_AUDIO_PLUGIN_EVENT_DOT_H
#define DMZ_AUDIO_PLUGIN_EVENT_DOT_H

#include <dmzEventObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class AudioPluginEvent :
         public Plugin,
         public EventObserverUtil {

      public:
         AudioPluginEvent (const PluginInfo &Info, Config &local);
         ~AudioPluginEvent ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // Event Observer Interface
         virtual void end_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality);

     protected:
         void _init (Config &local);

         Log _log;

      private:
         AudioPluginEvent ();
         AudioPluginEvent (const AudioPluginEvent &);
         AudioPluginEvent &operator= (const AudioPluginEvent &);

   };
};

#endif // DMZ_AUDIO_PLUGIN_EVENT_DOT_H
