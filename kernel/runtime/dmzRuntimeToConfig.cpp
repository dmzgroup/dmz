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

dmz::Config
dmz::runtime_to_config (RuntimeContext *context) {

   Config result (RuntimeName);

   Config defs = definitions_to_config (context);
   Config time = time_to_config (context);
   Config rc = resource_to_config (context);

   result.add_children (time);
   result.add_children (defs);
   result.add_children (rc);

   return result;
}


dmz::Config
dmz::definitions_to_config (RuntimeContext *context) {

   Config result (RuntimeName);

   RuntimeContextDefinitions *defs = (context ? context->get_definitions_context () : 0);

   if (defs) {

      const ObjectType RootObjectType = Definitions (context).get_root_object_type ();
      const EventType RootEventType = Definitions (context).get_root_event_type ();
      const Message GlobalMessage = Definitions (context).get_global_message ();

      defs->ref ();

      StringContainer list;

      Definitions (context).get_state_names (list);

      StringContainerIterator scIt;
      String name;

      while (list.get_next (scIt, name)) {

         Config data ("state");
         data.store_attribute ("name", name);
         result.add_config (data);
      }

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

      it.reset ();
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

      it.reset ();
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


dmz::Config
dmz::time_to_config (RuntimeContext *context) {

   Config result (RuntimeName);

   RuntimeContextTime *time = (context ? context->get_time_context () : 0);

   if (time) {

      time->ref ();

      Config data ("time");
      Config hz ("frequency");
      hz.store_attribute ("value", String::number (time->targetFrequency));
      data.add_config (hz);
      result.add_config (data);

      time->unref ();
   }

   return result;
}


dmz::Config
dmz::resource_to_config (RuntimeContext *context) {

   Config result (RuntimeName);

   RuntimeContextResources *rc = (context ? context->get_resources_context () : 0);

   if (rc) {

      rc->ref ();
      rc->unref ();
   }

   return result;
}

