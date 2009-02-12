#ifndef DMZ_AUDIO_PLUGIN_EVENT_DOT_H
#define DMZ_AUDIO_PLUGIN_EVENT_DOT_H

#include <dmzEventObserverUtil.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeResources.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHashTableStringTemplate.h>

namespace dmz {

   class AudioModule;
   class EventModule;
   class EventType;
   class ObjectModule;

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

            HashTableHandleTemplate<ObjectTypeStruct> table;

            ObjectTypeStruct (const Handle TheSound) : Sound (TheSound) {;}
            ~ObjectTypeStruct () { table.empty (); }
         };

         struct AttrStruct {

            const Handle Attr;
            AttrStruct *next;
            AttrStruct (const Handle TheAttr) : Attr (TheAttr), next (0) {;}
            ~AttrStruct () { if (next) { delete next; next = 0; } }
         };

         struct EventStruct {

            const Handle Sound;

            AttrStruct *attr;
            ObjectTypeStruct *types;

            EventStruct (const Handle TheSound) :
                  Sound (TheSound),
                  attr (0),
                  types (0) {;}

            ~EventStruct () {

               if (attr) { delete attr; attr = 0; }
               if (types) { delete types; types = 0; }
            }
         };

         EventStruct *_create_event_struct (const EventType &Type);
         Handle _create_sound (const String &Name);

         void _create_attr_list (
            const EventType &Type,
            const Config &Source,
            EventStruct &event);

         void _create_obj_types (
            const EventType &Type,
            const Config &Source,
            ObjectTypeStruct &types);

         void _init (Config &local);
         void _clear ();

         Log _log;
         Resources _rc;
         Definitions _defs;
         AudioModule *_audioMod;
         EventModule *_eventMod;
         ObjectModule *_objMod;

         Handle _defaultEventHandle;
         EventTypeSet _eventTypes;
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
