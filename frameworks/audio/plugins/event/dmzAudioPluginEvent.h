#ifndef DMZ_AUDIO_PLUGIN_EVENT_DOT_H
#define DMZ_AUDIO_PLUGIN_EVENT_DOT_H

#include <dmzEventObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeResources.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHashTableStringTemplate.h>

namespace dmz {

   class AudioModule;
   class EventType;

   class AudioPluginEvent :
         public Plugin,
         public EventObserverUtil {

      public:
         //! \cond
         AudioPluginEvent (const PluginInfo &Info, Config &local);
         ~AudioPluginEvent ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Event Observer Interface
         virtual void close_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality);

     protected:
         struct ObjectTypeStruct {

            const Handle Sound;

            HashTableHandleTemplate <ObjectTypeStruct> table;

            ObjectTypeStruct (const Handle TheSound) : Sound (TheSound) {;}
            ~ObjectTypeStruct () { table.empty (); }
         };

         struct EventStruct {

            const Handle Sound;

            HashTableHandleTemplate <ObjectTypeStruct> table;

            EventStruct (const Handle TheSound) : Sound (TheSound) {;}
            ~EventStruct () { table.empty (); }
         };

         EventStruct *_create_event_struct (const EventType &Type);
         void _clear ();
         void _init (Config &local);

         Log _log;
         Resources _rc;
         AudioModule *_audioMod;

         HashTableStringTemplate<Handle> _soundTable;
         HashTableHandleTemplate<EventStruct> _eventTable;
         //! \endcond

      private:
         AudioPluginEvent ();
         AudioPluginEvent (const AudioPluginEvent &);
         AudioPluginEvent &operator= (const AudioPluginEvent &);

   };
};

#endif // DMZ_AUDIO_PLUGIN_EVENT_DOT_H
