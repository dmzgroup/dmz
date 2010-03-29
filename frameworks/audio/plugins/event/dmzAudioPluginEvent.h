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
         struct TypeStruct {

            const Handle Sound;

            TypeStruct (const Handle TheSound) : Sound (TheSound) {;}
         };

         struct TypeTable {

            const Handle Sound;
            const Handle TypeAttr;

            HashTableHandleTemplate<TypeStruct> map;
            HashTableHandleTemplate<TypeStruct> table;

            TypeTable (const Handle TheSound, const Handle TheTypeAttr) :
                  Sound (TheSound),
                  TypeAttr (TheTypeAttr) {;}

            ~TypeTable () { map.clear (); table.empty (); }
         };

         Handle _get_sound (const Handle EventHandle, const EventType &Type);

         TypeTable *_get_type_table (const EventType &Type);

         TypeStruct *_create_type (
            const Handle EventHandle,
            const EventType &Event,
            const ObjectType &Object,
            TypeTable &table);

         Handle _create_sound (const String &Name);
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
         EventTypeSet _ignoredEventTypes;
         HashTableStringTemplate<Handle> _soundTable;
         HashTableHandleTemplate<TypeTable> _typeTable;
         //! \endcond

      private:
         AudioPluginEvent ();
         AudioPluginEvent (const AudioPluginEvent &);
         AudioPluginEvent &operator= (const AudioPluginEvent &);

   };
};

#endif // DMZ_AUDIO_PLUGIN_EVENT_DOT_H
