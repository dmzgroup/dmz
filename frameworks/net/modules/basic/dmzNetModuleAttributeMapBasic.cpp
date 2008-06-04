#include "dmzNetModuleAttributeMapBasic.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeInit.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesStringTokenizer.h>

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

   _netTypeTable.empty ();
   _internalTypeTable.empty ();
   _maskTable.empty ();
   _whichTable.empty ();
}


// NetModuleAttributeMap Interface
dmz::UInt32
dmz::NetModuleAttributeMapBasic::lookup_which_handle (const String &Name) {

   UInt32 handle (0);

   return handle;
}


dmz::Boolean
dmz::NetModuleAttributeMapBasic::to_net_mask (
      const Int32 Which,
      const Mask &InValue,
      ArrayUInt32 &outValue) {

   Boolean result (False);

   MaskStruct *ms (_maskTable.lookup (Which));

   if (ms) {

      _to_net_mask (InValue, outValue, ms);
      result = True;
   }

   return result;
}


dmz::Boolean
dmz::NetModuleAttributeMapBasic::to_internal_mask (
      const Int32 Which,
      const ArrayUInt32 &InValue,
      Mask &outValue) {

   Boolean result (False);

   MaskStruct *ms (_maskTable.lookup (Which));

   if (ms) {

      _to_internal_mask (InValue, outValue, ms);
      result = True;
   }

   return result;
}


dmz::Boolean
dmz::NetModuleAttributeMapBasic::to_net_mask (
      const ObjectType &Type,
      const Mask &InValue,
      ArrayUInt32 &outValue) {

   Boolean result (False);

   MaskStruct *ms (_find_mask_struct_from_type (Type));

   if (ms) { _to_net_mask (InValue, outValue, ms); result = True; }

   return result;
}


dmz::Boolean
dmz::NetModuleAttributeMapBasic::to_internal_mask (
      const ObjectType &Type,
      const ArrayUInt32 &InValue,
      Mask &outValue) {

   Boolean result (False);

   MaskStruct *ms (_find_mask_struct_from_type (Type));

   if (ms) { _to_internal_mask (InValue, outValue, ms); result = True; }

   return result;
}


dmz::Boolean
dmz::NetModuleAttributeMapBasic::to_net_type (
      const ObjectType &Type,
      ArrayUInt32 &array) {

   Boolean result (False);

   InternalStruct *is (_internalTypeTable.lookup (Type.get_handle ()));

   if (is) { array = is->NetType; result = True; }

   return result;
}


dmz::Boolean
dmz::NetModuleAttributeMapBasic::to_internal_type (
      const ArrayUInt32 &Array,
      ObjectType &type) {

   Boolean result (False);

   const Int32 Size (Array.get_size ());

   Int32 count (0);

   NetStruct *current (_netTypeTable.lookup (Array.get (count)));

   if (current) {

      count++;

      Boolean done (False);

      while (!done) {

         if (count >= Size) { done = True; }
         else {
            NetStruct *next (current->table.lookup (Array.get(count)));

            if (!next) { done = True; }
            else { current = next; count++; }
         }
      }

      if (current) { type = current->type; result = True; }
   }

   return result;
}


dmz::NetModuleAttributeMapBasic::MaskStruct *
dmz::NetModuleAttributeMapBasic::_find_mask_struct_from_type (const ObjectType &Type) {

   MaskStruct *ms (_maskTable.lookup (Type.get_handle ()));

   if (!ms) {

      ObjectType current (Type); current.become_parent ();

      while (current && !ms) {

         ms = _maskTable.lookup (current.get_handle ());
         current.become_parent ();
      }
   }

   return ms;
}


void
dmz::NetModuleAttributeMapBasic::_to_net_mask (
      const Mask &InData,
      ArrayUInt32 &outData,
      MaskStruct *ms) {

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
dmz::NetModuleAttributeMapBasic::_to_internal_mask (
      const ArrayUInt32 &InData,
      Mask &outData,
      MaskStruct *ms) {

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

   ObjectType type (_defs.get_root_object_type ());

   if (type) { _process_type (type); }
}


void
dmz::NetModuleAttributeMapBasic::_process_type (ObjectType &type) {

   _process_net_mask (type);
   _process_net_type (type);

   ObjectTypeIterator it;
   ObjectType next;

   Boolean found (type.get_first_child (it, next));

   while (found) {

      _process_type (next);
      found = type.get_next_child (it, next);
   }
}


void
dmz::NetModuleAttributeMapBasic::_process_net_mask (ObjectType &type) {

   Config stateList;

   if (type.get_config ().lookup_all_config ("net.state", stateList)) {

      ConfigIterator it;
      Config config;

      Boolean found (stateList.get_first_config (it, config));

      MaskStruct *head (0);
      MaskStruct *current (0);

      while (found) {

         Mask state;

         const UInt32 BitMask (config_to_uint32 ("mask", config));
         const String StateName (config_to_string ("name", config));

         if (BitMask && _defs.lookup_state (StateName, state)) {

            const UInt32 Offset (config_to_int32 ("offset", config));

            MaskStruct *next (new MaskStruct (BitMask, Offset, state));
            if (current) { current->next = next; current = next; }
            else { head = current = next; }
         }
         else if (!BitMask) {

            _log.error << "Net bit mask not defined in object type: "
               << type.get_name () << " for state: "
               << (StateName ? StateName : "<NULL>") << endl;
         }
         else if (!state) {

            _log.error << "State not defined in object type: "
               << type.get_name () << " for state: "
               << (StateName ? StateName : "<NULL>") << endl;
         }

         found = stateList.get_next_config (it, config);
      }

      if (head) {

         if (!_maskTable.store (type.get_handle (), head)) {

            delete head; head = 0;

            _log.error << "Unable to store state map for object type: "
               << type.get_name () << endl;
         }
      }
   }
}


void
dmz::NetModuleAttributeMapBasic::_process_net_type (ObjectType &type) {

   Config typeMap;

   if (type.get_config ().lookup_config ("net.type", typeMap)) {

      const String NetEnum (config_to_string ("enum", typeMap));
      const Int32 Depth (config_to_int32 ("depth", typeMap));

      if (NetEnum) {

         Int32 count = 0;
         ArrayUInt32 array;
         StringTokenizer st (NetEnum, '.');

         String valueStr (st.get_next ());

         NetStruct *current (0);

         while (valueStr) {

            const UInt32 Value (string_to_uint32 (valueStr));

            array.set (count, Value);

            NetStruct *next (0);

            if (!current) { next = _netTypeTable.lookup (Value); }
            else { next = current->table.lookup (Value); }

            if (!next) {

               const ObjectType EmptyType;

               next = new NetStruct (
                  Value,
                  (((Depth > 0) && (count >= Depth)) ? type : EmptyType));

               if (next) {

                  if (!current) {

                     if (!_netTypeTable.store (Value, next)) { delete next; next = 0; }
                  }
                  else if (!current->table.store (Value, next)) { delete next; next = 0; }

                  if (!next) {

                     _log.error << "Unable to map value: " << Value << " at depth: "
                        << count << " in net enum: " << local_array_to_enum (array)
                        << " for type: " << type.get_name () << endl;
                  }
               }
            }

            if (next) {

               if (Depth && (count > Depth)) {

                  if (!next->type) { next->type = type; }
                  else if (next->type != type) {

                     _log.error << "Unable to map type: " << type.get_name ()
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

               if (!current->type) { current->type = type; }
               else if (current->type != type) {

                  _log.error << "Unable to map type: " << type.get_name ()
                     << " to net type enum: " << NetEnum
                     << " because it is already mapped to type: "
                     << current->type.get_name () << endl;
               }
            }

            InternalStruct *ls (new InternalStruct (array, type));

            if (ls && !_internalTypeTable.store (type.get_handle (), ls)) {

               delete ls; ls = 0;
            }
         }
      }
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzNetModuleAttributeMapBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::NetModuleAttributeMapBasic (Info, local);
}

};
