#include <dmzRuntimeToConfig.h>
#include <dmzRuntimeConfig.h>
#include "dmzRuntimeContext.h"
#include "dmzRuntimeContextDefinitions.h"
#include "dmzRuntimeContextResources.h"
#include "dmzRuntimeContextTime.h"
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeObjectType.h>

namespace {

static char RuntimeName[] = "runtime";

};

/*!

\brief Serializes a RuntimeContext and stores the result in a Config object.
\ingroup Runtime
\details Format of the returned Config object:
\code
<runtime>
   <!-- time configuration -->
   <!-- state configuration -->
   <!-- message configuration -->
   <!-- object-type configuration -->
   <!-- event-type configuration -->
   <!-- resources configuration -->
</runtime>
\endcode
\param[in] context Pointer to the RuntimeContext to serialize.
\return Returns a Config object containing the serialized RuntimeContext.
\sa
dmz::runtime_time_to_config()
dmz::runtime_states_to_config()
dmz::runtime_messages_to_config()
dmz::runtime_object_types_to_config()
dmz::runtime_event_types_to_config()
dmz::runtime_resources_to_config()

*/
dmz::Config
dmz::runtime_to_config (RuntimeContext *context) {

   Config result (RuntimeName);

   Config defs = runtime_definitions_to_config (context);
   Config time = runtime_time_to_config (context);
   Config rc = runtime_resources_to_config (context);

   result.add_children (time);
   result.add_children (defs);
   result.add_children (rc);

   return result;
}


/*!

\brief Serializes the Definitions in a RuntimeContext and stores the result in a Config
object.
\ingroup Runtime
\details Format of the returned Config object:
\code
<runtime>
   <!-- state configuration -->
   <!-- message configuration -->
   <!-- object-type configuration -->
   <!-- event-type configuration -->
</runtime>
\endcode
\param[in] context Pointer to the RuntimeContext to serialize.
\return Returns a Config object containing the serialized Definitions in the
RuntimeContext.
\sa
dmz::runtime_states_to_config()
dmz::runtime_messages_to_config()
dmz::runtime_object_types_to_config()
dmz::runtime_event_types_to_config()

*/
dmz::Config
dmz::runtime_definitions_to_config (RuntimeContext *context) {

   Config result (RuntimeName);

   Config states = runtime_states_to_config (context);
   Config messages = runtime_messages_to_config (context);
   Config otypes = runtime_object_types_to_config (context);
   Config etypes = runtime_event_types_to_config (context);

   result.add_children (states);
   result.add_children (messages);
   result.add_children (otypes);
   result.add_children (etypes);

   return result;
}


/*!

\brief Serializes the states in a RuntimeContext and stores the result in a Config
object.
\ingroup Runtime
\details Format of the returned Config object:
\code
<runtime>
   <state name="String"/>
</runtime>
\endcode

- \b state.name String containing the name of the state.

\param[in] context Pointer to the RuntimeContext to serialize.
\return Returns a Config object containing the serialized states in the
RuntimeContext.

*/
dmz::Config
dmz::runtime_states_to_config (RuntimeContext *context) {

   RuntimeContextDefinitions *defs = (context ? context->get_definitions_context () : 0);

   Config result (RuntimeName);

   if (defs) {

      defs->ref ();

      StringContainer list;

      Definitions (context).get_state_names (list);

      StringContainerIterator it;
      String name;

      while (list.get_next (it, name)) {

         Config data ("state");
         data.store_attribute ("name", name);
         result.add_config (data);
      }

      defs->unref ();
   }

   return result;
}


/*!

\brief Serializes the Message definitions in a RuntimeContext and stores the result in a
Config object.
\ingroup Runtime
\details Format of the returned Config object:
\code
<runtime>
   <message name="String" monostate="Boolean"/>
</runtime>
\endcode

- \b message.name String containing the name of the Message.
- \b message.monostate Boolean indicating if the message has a monostate.

\param[in] context Pointer to the RuntimeContext to serialize.
\return Returns a Config object containing the serialized Message definitions in the
RuntimeContext.

*/
dmz::Config
dmz::runtime_messages_to_config (RuntimeContext *context) {

   RuntimeContextDefinitions *defs = (context ? context->get_definitions_context () : 0);

   Config result (RuntimeName);

   if (defs) {

      defs->ref ();

      const Message GlobalMessage = Definitions (context).get_global_message ();

      HashTableHandleIterator it;
      Message *msg (0);

      while (defs->messageHandleTable.get_next (it, msg)) {

         if (GlobalMessage != *msg) {

            Config data ("message");
            data.store_attribute ("name", msg->get_name ());

            if (msg->get_monostate_mode () == MessageMonostateOn) {

               data.store_attribute ("monostate", "true");
            }

            result.add_config (data);
         }
      }

      defs->unref ();
   }

   return result;
}


/*!

\brief Serializes the ObjectType definitions in a RuntimeContext and stores the result
in a Config object.
\ingroup Runtime
\details Format of the returned Config object:
\code
<runtime>
   <object-type name="String" parent="String"/>
</runtime>
\endcode

- \b object-type.name String containing the name of the ObjectType.
- \b object-type.parent String containing the name of the parent ObjectType.
- \b object-type.* Config data of the ObjectType.

\param[in] context Pointer to the RuntimeContext to serialize.
\return Returns a Config object containing the serialized ObjectType definitions in the
RuntimeContext.

*/
dmz::Config
dmz::runtime_object_types_to_config (RuntimeContext *context) {

   RuntimeContextDefinitions *defs = (context ? context->get_definitions_context () : 0);

   Config result (RuntimeName);

   if (defs) {

      defs->ref ();

      const ObjectType RootObjectType = Definitions (context).get_root_object_type ();

      HashTableHandleIterator it;
      ObjectType *otype (0);

      while (defs->objectHandleTable.get_next (it, otype)) {

         if (RootObjectType != *otype) {

            Config data ("object-type");
            data.store_attribute ("name", otype->get_name ());
            ObjectType parent = otype->get_parent ();

            if (parent != RootObjectType) {

               data.store_attribute ("parent", parent.get_name ());
            }

            data.add_children (otype->get_config ());

            result.add_config (data);
         }
      }

      defs->unref ();
   }

   return result;
}


/*!

\brief Serializes the EventType definitions in a RuntimeContext and stores the result
in a Config object.
\ingroup Runtime
\details Format of the returned Config object:
\code
<runtime>
   <event-type name="String" parent="String"/>
</runtime>
\endcode

- \b event-type.name String containing the name of the EventType.
- \b event-type.parent String containing the name of the parent EventType.
- \b event-type.* Config data of the EventType.

\param[in] context Pointer to the RuntimeContext to serialize.
\return Returns a Config object containing the serialized EventType definitions in the
RuntimeContext.

*/
dmz::Config
dmz::runtime_event_types_to_config (RuntimeContext *context) {

   RuntimeContextDefinitions *defs = (context ? context->get_definitions_context () : 0);

   Config result (RuntimeName);

   if (defs) {

      defs->ref ();

      const EventType RootEventType = Definitions (context).get_root_event_type ();

      HashTableHandleIterator it;
      ObjectType *otype (0);
      EventType *etype (0);

      while (defs->eventHandleTable.get_next (it, etype)) {

         if (RootEventType != *etype) {

            Config data ("event-type");
            data.store_attribute ("name", etype->get_name ());
            EventType parent = etype->get_parent ();

            if (parent != RootEventType) {

               data.store_attribute ("parent", parent.get_name ());
            }

            data.add_children (etype->get_config ());

            result.add_config (data);
         }
      }

      defs->unref ();
   }

   return result;
}


/*!

\brief Serializes the Time context in a RuntimeContext and stores the result
in a Config object.
\ingroup Runtime
\details Format of the returned Config object:
\code
<runtime>
   <time>
      <frequency value="Float64"/>
      <factor value="Float64"/>
   </time>
</runtime>
\endcode

- \b time.frequency.value Float64 containing the desired application loop frequency.
- \b time.factor.value Float64 containing the desired application time factor.

\param[in] context Pointer to the RuntimeContext to serialize.
\return Returns a Config object containing the serialized Time context from the
RuntimeContext.

*/
dmz::Config
dmz::runtime_time_to_config (RuntimeContext *context) {

   Config result (RuntimeName);

   RuntimeContextTime *time = (context ? context->get_time_context () : 0);

   if (time) {

      time->ref ();

      Config data ("time");

      Config hz ("frequency");
      hz.store_attribute ("value", String::number (time->targetFrequency));
      data.add_config (hz);

      Config factor ("factor");
      factor.store_attribute ("value", String::number (time->timeFactor));
      data.add_config (factor);

      result.add_config (data);

      time->unref ();
   }

   return result;
}


/*!

\brief Serializes the Resources in a RuntimeContext and stores the result
in a Config object.
\ingroup Runtime
\details Format of the returned Config object:
\code
<runtime>
   <resource-map>
      <search name="String">
         <path value="String"/>
         <path value="String"/>
         <!-- ... -->
         <path value="String"/>
      </search>
      <!-- ... -->
      <search name="String">
         <!-- ... -->
      </search>
      <resource name="String" file="String"/>
      <resource name="String" file="String"/>
      <!-- ... -->
      <resource name="String" file="String"/>
   </resource-map>
</runtime>
\endcode

- \b resource-map.search.name String containing the name of the search paths. [Optional]
- \b resource-map.search.path.value String containing the name of the path to use in the search
- \b resource-map.resource.name String containing the name of the resource.
- \b resource-map.resource.file String containing the file name of the resource.

\param[in] context Pointer to the RuntimeContext to serialize.
\return Returns a Config object containing the serialized Resources in the
RuntimeContext.

*/
dmz::Config
dmz::runtime_resources_to_config (RuntimeContext *context) {

   Config result (RuntimeName);
   Config map ("resource-map");
   result.add_config (map);

   RuntimeContextResources *rc = (context ? context->get_resources_context () : 0);

   if (rc) {

      rc->ref ();

      HashTableStringIterator it;
      StringContainer *paths (0);

      while (rc->pathTable.get_next (it, paths)) {

         Config list ("search");

         const String Name = it.get_hash_key ();

         if (Name) { list.store_attribute ("name", Name); }

         StringContainerIterator scIt;
         String value;

         while (paths->get_next (scIt, value)) {

            Config data ("path");
            data.store_attribute ("value", value);
            list.add_config (data);
         }

         map.add_config (list);
      }

      it.reset ();

      Config *ptr (0);

      while (rc->rcTable.get_next (it, ptr)) { map.add_config (*ptr); }

      rc->unref ();
   }

   return result;
}

