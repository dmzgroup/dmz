#ifndef DMZ_AUDIO_PLUGIN_EVENT_SIMPLE_DOT_H
#define DMZ_AUDIO_PLUGIN_EVENT_SIMPLE_DOT_H

#include <dmzEventObserverUtil.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class AudioModule;

   class AudioPluginEventSimple :
         public Plugin,
         public EventObserverUtil {

      public:
         AudioPluginEventSimple (const PluginInfo &Info, Config &local);
         ~AudioPluginEventSimple ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Event Observer Interface
         virtual void close_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality);

      protected:
         struct EventStruct {

            const String File;
            Handle sound;
            EventType event;
            EventStruct (const String &TheFile) : File (TheFile), sound (0) {;}
         };

         void _init (Config &local);

         Log _log;
         AudioModule *_audioMod;
         Handle _defaultEventHandle;

         HashTableHandleTemplate<EventStruct> _eventTable;

      private:
         AudioPluginEventSimple ();
         AudioPluginEventSimple (const AudioPluginEventSimple &);
         AudioPluginEventSimple &operator= (const AudioPluginEventSimple &);

   };
};

#endif // DMZ_AUDIO_PLUGIN_EVENT_SIMPLE_DOT_H
