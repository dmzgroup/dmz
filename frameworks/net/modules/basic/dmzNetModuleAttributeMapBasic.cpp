#include "dmzNetModuleAttributeMapBasic.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeInit.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesStringTokenizer.h>

/*!

\class dmz::NetModuleAttributeMapBasic
\ingroup Net
\brief Basic Network Attribute Module.
\details
\code
<dmz>
<runtime>
   <object-type name="Type Name">
      <net>
         <enum value="X.X.X" depth="enumeration depth"/>
         <state name="State Name" mask="Bit mask"/>
         ...
      </net>
   </object-type>
   <event-type name="Type Name">
      <net>
         <enum value="X.X.X" depth="enumeration depth"/>
         <state name="State Name" mask="Bit mask"/>
         ...
      </net>
   </event-type>
</runtime>
</dmz>
\endcode

*/

//! \cond
static dmz::String
local_array_to_enum (const dmz::ArrayUInt32 &Array) {

   dmz::String result;

   for (dmz::Int32 ix = 0; ix < Array.get_size (); ix++) {

      if (result) { result << "."; }
      result << Array.get (ix);
   }

   return result;
}


dmz::NetModuleAttributeMapBasic::NetModuleAttributeMapBasic (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      NetModuleAttributeMap (Info),
      _log (Info),
      _defs (Info, &_log) {

   _init (local);
}


dmz::NetModuleAttributeMapBasic::~NetModuleAttributeMapBasic () {

   _netObjectTypeTable.empty ();
   _internalObjectTypeTable.empty ();
   _netEventTypeTable.empty ();
   _internalEventTypeTable.empty ();
   _objectMaskTable.empty ();
   _eventMaskTable.empty ();
}


// NetModuleAttributeMap Interface
dmz::Boolean
dmz::NetModuleAttributeMapBasic::to_net_object_mask (
      const ObjectType &Type,
      const Mask &InValue,
      ArrayUInt32 &outValue) {

   Boolean result (False);

   ObjectMaskStruct *ms (_find_mask_struct_from_object_type (Type));

   if (ms) { _to_net_object_mask (InValue, outValue, ms); result = True; }

   return result;
}


dmz::Boolean
dmz::NetModuleAttributeMapBasic::to_internal_object_mask (
      const ObjectType &Type,
      const ArrayUInt32 &InValue,
      Mask &outValue) {

   Boolean result (False);

   ObjectMaskStruct *ms (_find_mask_struct_from_object_type (Type));

   if (ms) { _to_internal_object_mask (InValue, outValue, ms); result = True; }

   return result;
}


dmz::Boolean
dmz::NetModuleAttributeMapBasic::to_net_object_type (
      const ObjectType &Type,
      ArrayUInt32 &array) {

   Boolean result (False);

   InternalObjectStruct *is (_internalObjectTypeTable.lookup (Type.get_handle ()));

   if (is) { array = is->NetType; result = True; }

   return result;
}


dmz::Boolean
dmz::NetModuleAttributeMapBasic::to_internal_object_type (
      const ArrayUInt32 &Array,
      ObjectType &type) {

   Boolean result (False);

   const Int32 Size (Array.get_size ());

   Int32 count (0);

   NetObjectStruct *current (_netObjectTypeTable.lookup (Array.get (count)));

   if (current) {

      count++;

      Boolean done (False);

      while (!done) {

         if (count >= Size) { done = True; }
         else {

            NetObjectStruct *next (current->table.lookup (Array.get(count)));

            if (!next) { done = True; }
            else { current = next; count++; }
         }
      }

      if (current) { type = current->type; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::NetModuleAttributeMapBasic::to_net_event_mask (
      const EventType &Type,
      const Mask &InValue,
      ArrayUInt32 &outValue) {

   Boolean result (False);

   EventMaskStruct *ms (_find_mask_struct_from_event_type (Type));

   if (ms) { _to_net_event_mask (InValue, outValue, ms); result = True; }

   return result;
}


dmz::Boolean
dmz::NetModuleAttributeMapBasic::to_internal_event_mask (
      const EventType &Type,
      const ArrayUInt32 &InValue,
      Mask &outValue) {

   Boolean result (False);

   EventMaskStruct *ms (_find_mask_struct_from_event_type (Type));

   if (ms) { _to_internal_event_mask (InValue, outValue, ms); result = True; }

   return result;
}



dmz::Boolean
dmz::NetModuleAttributeMapBasic::to_net_event_type (
      const EventType &Type,
      ArrayUInt32 &array) {

   Boolean result (False);

   InternalEventStruct *is (_internalEventTypeTable.lookup (Type.get_handle ()));

   if (is) { array = is->NetType; result = True; }

   return result;
}


dmz::Boolean
dmz::NetModuleAttributeMapBasic::to_internal_event_type (
      const ArrayUInt32 &Array,
      EventType &type) {

   Boolean result (False);

   const Int32 Size (Array.get_size ());

   Int32 count (0);

   NetEventStruct *current (_netEventTypeTable.lookup (Array.get (count)));

   if (current) {

      count++;

      Boolean done (False);

      while (!done) {

         if (count >= Size) { done = True; }
         else {

            NetEventStruct *next (current->table.lookup (Array.get(count)));

            if (!next) { done = True; }
            else { current = next; count++; }
         }
      }

      if (current) { type = current->type; result = True; }
   }

   return result;
}


dmz::NetModuleAttributeMapBasic::ObjectMaskStruct *
dmz::NetModuleAttributeMapBasic::_find_mask_struct_from_object_type (
      const ObjectType &Type) {

   ObjectMaskStruct *ms (_objectMaskTable.lookup (Type.get_handle ()));

   if (!ms) {

      ObjectType current (Type); current.become_parent ();

      while (current && !ms) {

         ms = _objectMaskTable.lookup (current.get_handle ());
         current.become_parent ();
      }
   }

   return ms;
}


void
dmz::NetModuleAttributeMapBasic::_to_net_object_mask (
      const Mask &InData,
      ArrayUInt32 &outData,
      ObjectMaskStruct *ms) {

   while (ms) {

      if (InData.contains (ms->Value)) {

         UInt32 value (outData.get (ms->Offset));
         value |= ms->NetValue;
         outData.set (ms->Offset, value);
      }

      ms = ms->next;
   }
}


void
dmz::NetModuleAttributeMapBasic::_to_internal_object_mask (
      const ArrayUInt32 &InData,
      Mask &outData,
      ObjectMaskStruct *ms) {

   Mask masterMask;
   Mask result;

   while (ms) {

      masterMask |= ms->Value;

      if ((InData.get (ms->Offset) & ms->NetValue) == ms->NetValue) {

         result |= ms->Value;
      }

      ms = ms->next;
   }

   outData.unset (masterMask);
   outData |= result;
}


dmz::NetModuleAttributeMapBasic::EventMaskStruct *
dmz::NetModuleAttributeMapBasic::_find_mask_struct_from_event_type (
      const EventType &Type) {

   EventMaskStruct *ms (_eventMaskTable.lookup (Type.get_handle ()));

   if (!ms) {

      EventType current (Type); current.become_parent ();

      while (current && !ms) {

         ms = _eventMaskTable.lookup (current.get_handle ());
         current.become_parent ();
      }
   }

   return ms;
}


void
dmz::NetModuleAttributeMapBasic::_to_net_event_mask (
      const Mask &InData,
      ArrayUInt32 &outData,
      EventMaskStruct *ms) {

   while (ms) {

      if (InData.contains (ms->Value)) {

         UInt32 value (outData.get (ms->Offset));
         value |= ms->NetValue;
         outData.set (ms->Offset, value);
      }

      ms = ms->next;
   }
}


void
dmz::NetModuleAttributeMapBasic::_to_internal_event_mask (
      const ArrayUInt32 &InData,
      Mask &outData,
      EventMaskStruct *ms) {

   Mask masterMask;
   Mask result;

   while (ms) {

      masterMask |= ms->Value;

      if ((InData.get (ms->Offset) & ms->NetValue) == ms->NetValue) {

         result |= ms->Value;
      }

      ms = ms->next;
   }

   outData.unset (masterMask);
   outData |= result;
}


void
dmz::NetModuleAttributeMapBasic::_init (Config &local) {

   const ObjectType RootObjectType (_defs.get_root_object_type ());

   if (RootObjectType) { _process_object_type (RootObjectType); }

   const EventType RootEventType (_defs.get_root_event_type ());

   if (RootEventType) { _process_event_type (RootEventType); }
}


void
dmz::NetModuleAttributeMapBasic::_process_object_type (const ObjectType &Type) {

   _process_net_object_mask (Type);
   _process_net_object_type (Type);

   ObjectTypeIterator it;
   ObjectType next;

   Boolean found (Type.get_first_child (it, next));

   while (found) {

      _process_object_type (next);
      found = Type.get_next_child (it, next);
   }
}


void
dmz::NetModuleAttributeMapBasic::_process_net_object_mask (const ObjectType &Type) {

   Config stateList;

   if (Type.get_config ().lookup_all_config ("net.state", stateList)) {

      ConfigIterator it;
      Config config;

      Boolean found (stateList.get_first_config (it, config));

      ObjectMaskStruct *head (0);
      ObjectMaskStruct *current (0);

      while (found) {

         Mask state;

         const UInt32 BitMask (config_to_uint32 ("mask", config));
         const String StateName (config_to_string ("name", config));

         if (BitMask && _defs.lookup_state (StateName, state)) {

            const UInt32 Offset (config_to_int32 ("offset", config));

            ObjectMaskStruct *next (new ObjectMaskStruct (BitMask, Offset, state));
            if (current) { current->next = next; current = next; }
            else { head = current = next; }
         }
         else if (!BitMask) {

            _log.error << "Net bit mask not defined in object type: "
               << Type.get_name () << " for state: "
               << (StateName ? StateName : "<NULL>") << endl;
         }
         else if (!state) {

            _log.error << "State not defined in object type: "
               << Type.get_name () << " for state: "
               << (StateName ? StateName : "<NULL>") << endl;
         }

         found = stateList.get_next_config (it, config);
      }

      if (head) {

         if (!_objectMaskTable.store (Type.get_handle (), head)) {

            delete head; head = 0;

            _log.error << "Unable to store state map for object type: "
               << Type.get_name () << endl;
         }
      }
   }
}


void
dmz::NetModuleAttributeMapBasic::_process_net_object_type (const ObjectType &Type) {

   Config typeMap;

   if (Type.get_config ().lookup_config ("net.enum", typeMap)) {

      const String NetEnum (config_to_string ("value", typeMap));
      const Int32 Depth (config_to_int32 ("depth", typeMap));

      if (NetEnum) {

         Int32 count = 0;
         ArrayUInt32 array;
         StringTokenizer st (NetEnum, '.');

         String valueStr (st.get_next ());

         NetObjectStruct *current (0);

         while (valueStr) {

            const UInt32 Value (string_to_uint32 (valueStr));

            array.set (count, Value);

            NetObjectStruct *next (0);

            if (!current) { next = _netObjectTypeTable.lookup (Value); }
            else { next = current->table.lookup (Value); }

            if (!next) {

               const ObjectType EmptyType;

               next = new NetObjectStruct (
                  Value,
                  (((Depth > 0) && (count >= Depth)) ? Type : EmptyType));

               if (next) {

                  if (!current) {

                     if (!_netObjectTypeTable.store (Value, next)) {

                        delete next; next = 0;
                     }
                  }
                  else if (!current->table.store (Value, next)) { delete next; next = 0; }

                  if (!next) {

                     _log.error << "Unable to map value: " << Value << " at depth: "
                        << count << " in net enum: " << local_array_to_enum (array)
                        << " for type: " << Type.get_name () << endl;
                  }
               }
            }

            if (next) {

               if (Depth && (count > Depth)) {

                  if (!next->type) { next->type = Type; }
                  else if (next->type != Type) {

                     _log.error << "Unable to map type: " << Type.get_name ()
                        << " to net type enum: " << local_array_to_enum (array)
                        << " because it is already mapped to type: "
                        << next->type.get_name () << endl;
                  }
               }

               current = next;
               count++;
               valueStr = st.get_next ();
            }
            else {

               valueStr.empty (); //Stop the while loop
               current = 0;
            }
         }

         if (current) {

            // If there is no depth then the last instance in the chain needs to be set
            // to the correct type
            if (!Depth) {

               if (!current->type) { current->type = Type; }
               else if (current->type != Type) {

                  _log.error << "Unable to map type: " << Type.get_name ()
                     << " to net type enum: " << NetEnum
                     << " because it is already mapped to type: "
                     << current->type.get_name () << endl;
               }
            }

            InternalObjectStruct *ls (new InternalObjectStruct (array, Type));

            if (ls && !_internalObjectTypeTable.store (Type.get_handle (), ls)) {

               delete ls; ls = 0;
            }
         }
      }
   }
}


void
dmz::NetModuleAttributeMapBasic::_process_event_type (const EventType &Type) {

   _process_net_event_mask (Type);
   _process_net_event_type (Type);

   EventTypeIterator it;
   EventType next;

   Boolean found (Type.get_first_child (it, next));

   while (found) {

      _process_event_type (next);
      found = Type.get_next_child (it, next);
   }
}


void
dmz::NetModuleAttributeMapBasic::_process_net_event_mask (const EventType &Type) {

   Config stateList;

   if (Type.get_config ().lookup_all_config ("net.state", stateList)) {

      ConfigIterator it;
      Config config;

      Boolean found (stateList.get_first_config (it, config));

      EventMaskStruct *head (0);
      EventMaskStruct *current (0);

      while (found) {

         Mask state;

         const UInt32 BitMask (config_to_uint32 ("mask", config));
         const String StateName (config_to_string ("name", config));

         if (BitMask && _defs.lookup_state (StateName, state)) {

            const UInt32 Offset (config_to_int32 ("offset", config));

            EventMaskStruct *next (new EventMaskStruct (BitMask, Offset, state));
            if (current) { current->next = next; current = next; }
            else { head = current = next; }
         }
         else if (!BitMask) {

            _log.error << "Net bit mask not defined in event type: "
               << Type.get_name () << " for state: "
               << (StateName ? StateName : "<NULL>") << endl;
         }
         else if (!state) {

            _log.error << "State not defined in event type: "
               << Type.get_name () << " for state: "
               << (StateName ? StateName : "<NULL>") << endl;
         }

         found = stateList.get_next_config (it, config);
      }

      if (head) {

         if (!_eventMaskTable.store (Type.get_handle (), head)) {

            delete head; head = 0;

            _log.error << "Unable to store state map for event type: "
               << Type.get_name () << endl;
         }
      }
   }
}


void
dmz::NetModuleAttributeMapBasic::_process_net_event_type (const EventType &Type) {

   Config typeMap;

   if (Type.get_config ().lookup_config ("net.type", typeMap)) {

      const String NetEnum (config_to_string ("enum", typeMap));
      const Int32 Depth (config_to_int32 ("depth", typeMap));

      if (NetEnum) {

         Int32 count = 0;
         ArrayUInt32 array;
         StringTokenizer st (NetEnum, '.');

         String valueStr (st.get_next ());

         NetEventStruct *current (0);

         while (valueStr) {

            const UInt32 Value (string_to_uint32 (valueStr));

            array.set (count, Value);

            NetEventStruct *next (0);

            if (!current) { next = _netEventTypeTable.lookup (Value); }
            else { next = current->table.lookup (Value); }

            if (!next) {

               const EventType EmptyType;

               next = new NetEventStruct (
                  Value,
                  (((Depth > 0) && (count >= Depth)) ? Type : EmptyType));

               if (next) {

                  if (!current) {

                     if (!_netEventTypeTable.store (Value, next)) {

                        delete next; next = 0;
                     }
                  }
                  else if (!current->table.store (Value, next)) { delete next; next = 0; }

                  if (!next) {

                     _log.error << "Unable to map value: " << Value << " at depth: "
                        << count << " in net enum: " << local_array_to_enum (array)
                        << " for type: " << Type.get_name () << endl;
                  }
               }
            }

            if (next) {

               if (Depth && (count > Depth)) {

                  if (!next->type) { next->type = Type; }
                  else if (next->type != Type) {

                     _log.error << "Unable to map type: " << Type.get_name ()
                        << " to net type enum: " << local_array_to_enum (array)
                        << " because it is already mapped to type: "
                        << next->type.get_name () << endl;
                  }
               }

               current = next;
               count++;
               valueStr = st.get_next ();
            }
            else {

               valueStr.empty (); //Stop the while loop
               current = 0;
            }
         }

         if (current) {

            // If there is no depth then the last instance in the chain needs to be set
            // to the correct type
            if (!Depth) {

               if (!current->type) { current->type = Type; }
               else if (current->type != Type) {

                  _log.error << "Unable to map type: " << Type.get_name ()
                     << " to net type enum: " << NetEnum
                     << " because it is already mapped to type: "
                     << current->type.get_name () << endl;
               }
            }

            InternalEventStruct *ls (new InternalEventStruct (array, Type));

            if (ls && !_internalEventTypeTable.store (Type.get_handle (), ls)) {

               delete ls; ls = 0;
            }
         }
      }
   }
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzNetModuleAttributeMapBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::NetModuleAttributeMapBasic (Info, local);
}

};
