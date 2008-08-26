#ifndef DMZ_RUNTIME_EVENT_TYPE_DOT_H
#define DMZ_RUNTIME_EVENT_TYPE_DOT_H

#include <dmzKernelExport.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeMessaging.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>
#include <dmzTypesBooleanOperator.h>

namespace dmz {

   class Config;
   class TypeContext;
   class RuntimeContext;

   class DMZ_KERNEL_LINK_SYMBOL EventTypeIterator {

      public:
         EventTypeIterator ();
         ~EventTypeIterator ();

         struct State;
         State &state; //!< Internal state.

      private:
         EventTypeIterator (const EventTypeIterator &);
         EventTypeIterator &operator= (const EventTypeIterator &);
   };

   class DMZ_KERNEL_LINK_SYMBOL EventType {

      public:
         EventType ();
         EventType (const EventType &Type);
         EventType (TypeContext *context);
         EventType (const String &Name, RuntimeContext *context);
         EventType (const Handle TypeHandle, RuntimeContext *context);
         ~EventType ();

         EventType &operator= (const EventType &Type);
         Boolean operator== (const EventType &Type) const;
         Boolean operator!= (const EventType &Type) const;
         Boolean operator! () const;
         DMZ_BOOLEAN_OPERATOR;

         Boolean set_type (const String &Name, RuntimeContext *context);
         Boolean set_type (const Handle TypeHandle, RuntimeContext *context);

         Boolean is_of_type (const EventType &Type) const;
         Boolean is_of_exact_type (const EventType &Type) const;

         String get_name () const;
         Handle get_handle () const;

         EventType get_parent () const;
         Boolean become_parent ();

         Boolean get_first_child (EventTypeIterator &it, EventType &type) const;
         Boolean get_next_child (EventTypeIterator &it, EventType &type) const;

         Config get_config () const;

         // for internal use
         void set_type_context (TypeContext *context);
         TypeContext *get_type_context () const;

      protected:
         TypeContext *_context; //!< Internal state.
   };

   class DMZ_KERNEL_LINK_SYMBOL EventTypeSet {

      public:
         EventTypeSet ();
         EventTypeSet (const EventTypeSet &Value);
         ~EventTypeSet ();

         EventTypeSet &operator= (const EventTypeSet &Value);

         Int32 get_count () const;

         Boolean add_event_type (const EventType &Type);
         Boolean add_event_type (const String &Name, RuntimeContext *context);
         Boolean add_event_type (const Handle TypeHandle, RuntimeContext *context);

         void add_event_set (const EventTypeSet &Set);

         Boolean remove_event_type (const EventType &Type);
         Boolean remove_event_type (const String &Name, RuntimeContext *context);
         Boolean remove_event_type (const Handle TypeHandle, RuntimeContext *context);

         Boolean contains_type (const EventType &Type) const;
         Boolean contains_exact_type (const EventType &Type) const;

         Boolean get_first (EventTypeIterator &it, EventType &type) const;
         Boolean get_next (EventTypeIterator &it, EventType &type) const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };

   DMZ_KERNEL_LINK_SYMBOL EventType
   config_to_event_type (
      const String &Name,
      const Config &Source,
      const String &DefaultEventTypeName,
      RuntimeContext *context);

   DMZ_KERNEL_LINK_SYMBOL EventType
   config_to_event_type (
      const String &Name,
      const Config &Source,
      const EventType &DefaultEventType,
      RuntimeContext *context);

   EventType
   config_to_event_type (
      const String &Name,
      const Config &Source,
      RuntimeContext *context);

   DMZ_KERNEL_LINK_SYMBOL EventTypeSet
   config_to_event_type_set (
      const String &Name,
      const Config &Source,
      RuntimeContext *context);
};


inline dmz::EventType
dmz::config_to_event_type (
      const String &Name,
      const Config &Source,
      RuntimeContext *context) {

   const EventType EmptyType;
   return config_to_event_type (Name, Source, EmptyType, context);
}

#endif // DMZ_RUNTIME_EVENT_TYPE_DOT_H

