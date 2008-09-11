#ifndef DMZ_NET_MODULE_ATTRIBUTE_MAP_BASIC_DOT_H
#define DMZ_NET_MODULE_ATTRIBUTE_MAP_BASIC_DOT_H

#include <dmzNetModuleAttributeMap.h>
#include <dmzRuntimeHandle.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableUInt32Template.h>
#include <dmzTypesMask.h>

namespace dmz {

   class NetModuleAttributeMapBasic :
         public Plugin,
         public  NetModuleAttributeMap {

      public:
         NetModuleAttributeMapBasic (const PluginInfo &Info, Config &local);
         ~NetModuleAttributeMapBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // NetModuleAttributeMap Interface
         virtual Boolean to_net_object_mask (
            const ObjectType &Type,
            const Mask &InValue,
            ArrayUInt32 &outValue);

         virtual Boolean to_internal_object_mask (
            const ObjectType &Type,
            const ArrayUInt32 &InValue,
            Mask &outValue);

         virtual Boolean to_net_object_type (
            const ObjectType &Type,
            ArrayUInt32 &array);

         virtual Boolean to_internal_object_type (
            const ArrayUInt32 &Array,
            ObjectType &type);

         virtual Boolean to_internal_event_mask (
            const EventType &Type,
            const ArrayUInt32 &InValue,
            Mask &outValue);

         virtual Boolean to_net_event_mask (
            const EventType &Type,
            const Mask &InValue,
            ArrayUInt32 &outValue);

         virtual Boolean to_net_event_type (
            const EventType &Type,
            ArrayUInt32 &array);

         virtual Boolean to_internal_event_type (
            const ArrayUInt32 &Array,
            EventType &type);

      protected:
         struct ObjectMaskStruct {

            const UInt32 NetValue;
            const Int32 Offset;
            const Mask Value;

            ObjectMaskStruct *next;

            ObjectMaskStruct (
                  const UInt32 TheNetValue,
                  const Int32 TheOffset,
                  const Mask &TheValue) :
                  NetValue (TheNetValue),
                  Offset (TheOffset),
                  Value (TheValue),
                  next (0) {;}

            ~ObjectMaskStruct () { if (next) { delete next; next = 0; } }
         };

         struct NetObjectStruct {

            const UInt32 NetValue;
            ObjectType type;

            HashTableUInt32Template<NetObjectStruct> table;

            NetObjectStruct (
                  const UInt32 TheValue,
                  const ObjectType &TheType) :
                  NetValue (TheValue),
                  type (TheType) {;}

            ~NetObjectStruct () { table.empty (); }
         };

         struct InternalObjectStruct {

            const ArrayUInt32 NetType;
            const ObjectType Type;

            InternalObjectStruct (
                  const ArrayUInt32 &TheNetType,
                  const ObjectType &TheType) :
                  NetType (TheNetType),
                  Type (TheType) {;}
         };

         struct EventMaskStruct {

            const UInt32 NetValue;
            const Int32 Offset;
            const Mask Value;

            EventMaskStruct *next;

            EventMaskStruct (
                  const UInt32 TheNetValue,
                  const Int32 TheOffset,
                  const Mask &TheValue) :
                  NetValue (TheNetValue),
                  Offset (TheOffset),
                  Value (TheValue),
                  next (0) {;}

            ~EventMaskStruct () { if (next) { delete next; next = 0; } }
         };

         struct NetEventStruct {

            const UInt32 NetValue;
            EventType type;

            HashTableUInt32Template<NetEventStruct> table;

            NetEventStruct (
                  const UInt32 TheValue,
                  const EventType &TheType) :
                  NetValue (TheValue),
                  type (TheType) {;}

            ~NetEventStruct () { table.empty (); }
         };

         struct InternalEventStruct {

            const ArrayUInt32 NetType;
            const EventType Type;

            InternalEventStruct (
                  const ArrayUInt32 &TheNetType,
                  const EventType &TheType) :
                  NetType (TheNetType),
                  Type (TheType) {;}
         };

         ObjectMaskStruct *_find_mask_struct_from_object_type (const ObjectType &Type);

         void _to_net_object_mask (
            const Mask &InData,
            ArrayUInt32 &outData,
            ObjectMaskStruct *ms);

         void _to_internal_object_mask (
            const ArrayUInt32 &InData,
            Mask &outData,
            ObjectMaskStruct *ms);

         EventMaskStruct *_find_mask_struct_from_event_type (const EventType &Type);

         void _to_net_event_mask (
            const Mask &InData,
            ArrayUInt32 &outData,
            EventMaskStruct *ms);

         void _to_internal_event_mask (
            const ArrayUInt32 &InData,
            Mask &outData,
            EventMaskStruct *ms);

         void _init (Config &local);
         void _process_object_type (const ObjectType &Type);
         void _process_net_object_type (const ObjectType &Type);
         void _process_net_object_mask (const ObjectType &Type);
         void _process_event_type (const EventType &Type);
         void _process_net_event_type (const EventType &Type);
         void _process_net_event_mask (const EventType &Type);

         Log _log;
         Definitions _defs;

         HashTableUInt32Template<NetObjectStruct> _netObjectTypeTable;
         HashTableUInt32Template<InternalObjectStruct> _internalObjectTypeTable;
         HashTableUInt32Template<ObjectMaskStruct> _objectMaskTable;
         HashTableUInt32Template<NetEventStruct> _netEventTypeTable;
         HashTableUInt32Template<InternalEventStruct> _internalEventTypeTable;
         HashTableUInt32Template<EventMaskStruct> _eventMaskTable;

      private:
         NetModuleAttributeMapBasic ();
         NetModuleAttributeMapBasic (const NetModuleAttributeMapBasic &);
         NetModuleAttributeMapBasic &operator= (const NetModuleAttributeMapBasic &);

   };
};

#endif // DMZ_NET_MODULE_ATTRIBUTE_MAP_BASIC_DOT_H
