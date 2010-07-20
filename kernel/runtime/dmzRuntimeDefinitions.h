#ifndef DMZ_RUNTIME_DEFINITIONS_DOT_H
#define DMZ_RUNTIME_DEFINITIONS_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

// NOTE: The code for this class is contained in dmzRuntimeInit.cpp

namespace dmz {

   class Config;
   class EventType;
   class Log;
   class Mask;
   class Message;
   class ObjectType;
   class PluginInfo;
   class RuntimeContext;
   class RuntimeIterator;
   class String;
   class StringContainer;

   //! Default name of dead state.
   const char DefaultStateNameDead[] = "Dead";
   //! Default name of deactivate state.
   const char DefaultStateNameDeactivate[] = "Deactivate";
   //! Default name of smoking state.
   const char DefaultStateNameSmoking[] = "Smoking";
   //! Default name of smoking state.
   const char DefaultStateNameFire[] = "Fire";
   //! Default name of dust trail state.
   const char DefaultStateNameDustTrail[] = "Dust_Trail";
   //! Default name of air born state.
   const char DefaultStateNameAirBorn[] = "Air_Born";

   class DMZ_KERNEL_LINK_SYMBOL Definitions {

      public:
         Definitions (RuntimeContext *context, Log *log = 0);
         Definitions (const PluginInfo &Info, Log *log = 0);
         ~Definitions ();

         Handle create_named_handle (const String &Name);
         Handle lookup_named_handle (const String &Name) const;
         String lookup_named_handle_name (const Handle NamedHandle) const;
         Handle get_first_named_handle (RuntimeIterator &it) const;
         Handle get_next_named_handle (RuntimeIterator &it) const;

         Message get_global_message () const;
         Boolean create_message (const String &Name, Message &type);
         Boolean lookup_message (const String &Name, Message &type) const;
         Boolean lookup_message (const Handle TypeHandle, Message &type) const;
         Boolean get_first_message (RuntimeIterator &it, Message &msg) const;
         Boolean get_next_message (RuntimeIterator &it, Message &msg) const;

         EventType get_root_event_type () const;
         Boolean lookup_event_type (const String &Name, EventType &type) const;
         Boolean lookup_event_type (const Handle TypeHandle, EventType &type) const;
         Boolean get_first_event_type (RuntimeIterator &it, EventType &type) const;
         Boolean get_next_event_type (RuntimeIterator &it, EventType &type) const;

         ObjectType get_root_object_type () const;
         Boolean lookup_object_type (const String &Name, ObjectType &type) const;
         Boolean lookup_object_type (const Handle TypeHandle, ObjectType &type) const;
         Boolean get_first_object_type (RuntimeIterator &it, ObjectType &type) const;
         Boolean get_next_object_type (RuntimeIterator &it, ObjectType &type) const;

         Boolean lookup_state (const String &Name, Mask &state) const;
         Boolean lookup_state_name (const Mask &State, String &name) const;
         void get_state_names (StringContainer &list) const;

         Boolean create_unique_name (const String &Name);
         Boolean lookup_unique_name (const String &Name);
         Boolean release_unique_name (const String &Name);

         String lookup_runtime_name (const Handle TypeHandle) const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };
};

#endif // DMZ_RUNTIME_DEFINITIONS_DOT_H
