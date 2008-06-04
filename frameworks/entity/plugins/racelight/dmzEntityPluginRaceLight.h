#ifndef DMZ_ENTITY_PLUGIN_RACE_LIGHT_DOT_H
#define DMZ_ENTITY_PLUGIN_RACE_LIGHT_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeSync.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class EntityPluginRaceLight :
         public Plugin,
         public InputObserverUtil,
         public ObjectObserverUtil,
         public Sync {

      public:
         EntityPluginRaceLight (const PluginInfo &Info, Config &local);
         ~EntityPluginRaceLight ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State);

         virtual void receive_button_event (
            const Handle Channel,
            const InputEventButton &Value);

         virtual void receive_key_event (
            const Handle Channel,
            const InputEventKey &Value);

         // Object Observer Interface
         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

         // Sync Interface
         virtual void update_sync (const Float64 DeltaTime);

      protected:
         struct StateStruct {

            const Mask State;
            const Float64 Delay;
            StateStruct *next;

            StateStruct (const Mask &TheState, const Float64 TheDelay) :
               State (TheState),
               Delay (TheDelay),
               next (0) {;}

            ~StateStruct () { if (next) { delete next; next = 0; } }
         };

         virtual void _store_object_module (ObjectModule &objMod);

         void _set_state (StateStruct &current, StateStruct *prev);

         void _init (Config &local);

         Handle _defaultHandle;
         Handle _lightHandle;
         Boolean _on;
         Mask _redState;
         Mask _yellowState;
         Mask _greenState;

         StateStruct *_list;
         StateStruct *_current;

         HashTableHandleTemplate<Handle> _lightTable;

         Log _log;

      private:
         EntityPluginRaceLight ();
         EntityPluginRaceLight (const EntityPluginRaceLight &);
         EntityPluginRaceLight &operator= (const EntityPluginRaceLight &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_RACE_LIGHT_DOT_H
