#ifndef DMZ_NET_MODULE_ATTRIBUTE_MAP_BASIC_DOT_H
#define DMZ_NET_MODULE_ATTRIBUTE_MAP_BASIC_DOT_H

#include <dmzNetModuleAttributeMap.h>
#include <dmzRuntimeHandle.h>
#include <dmzRuntimeDefinitions.h>
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
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // NetModuleAttributeMap Interface
         virtual UInt32 lookup_which_handle (const String &Name);

         virtual Boolean to_net_mask (
            const Int32 Which,
            const Mask &InValue,
            ArrayUInt32 &outValue);

         virtual Boolean to_internal_mask (
            const Int32 Which,
            const ArrayUInt32 &InValue,
            Mask &outValue);

         virtual Boolean to_net_mask (
            const ObjectType &Type,
            const Mask &InValue,
            ArrayUInt32 &outValue);

         virtual Boolean to_internal_mask (
            const ObjectType &Type,
            const ArrayUInt32 &InValue,
            Mask &outValue);

         virtual Boolean to_net_type (
            const ObjectType &Type,
            ArrayUInt32 &array);

         virtual Boolean to_internal_type (
            const ArrayUInt32 &Array,
            ObjectType &type);

      protected:
         struct MaskStruct {

            const UInt32 NetValue;
            const Int32 Offset;
            const Mask Value;

            MaskStruct *next;

            MaskStruct (
                  const UInt32 TheNetValue,
                  const Int32 TheOffset,
                  const Mask &TheValue) :
                  NetValue (TheNetValue),
                  Offset (TheOffset),
                  Value (TheValue),
                  next (0) {;}

            ~MaskStruct () { if (next) { delete next; next = 0; } }
         };

         struct NetStruct {

            const UInt32 NetValue;
            ObjectType type;

            HashTableUInt32Template<NetStruct> table;

            NetStruct (
                  const UInt32 TheValue,
                  const ObjectType &TheType) :
                  NetValue (TheValue),
                  type (TheType) {;}

            ~NetStruct () { table.empty (); }
         };

         struct InternalStruct {

            const ArrayUInt32 NetType;
            const ObjectType Type;

            InternalStruct (
                  const ArrayUInt32 &TheNetType,
                  const ObjectType &TheType) :
                  NetType (TheNetType),
                  Type (TheType) {;}
         };

         MaskStruct *_find_mask_struct_from_type (const ObjectType &Type);

         void _to_net_mask (const Mask &InData, ArrayUInt32 &outData, MaskStruct *ms);

         void _to_internal_mask (
            const ArrayUInt32 &InData,
            Mask &outData,
            MaskStruct *ms);

         void _init (Config &local);
         void _process_type (ObjectType &type);
         void _process_net_mask (ObjectType &type);
         void _process_net_type (ObjectType &type);

         Log _log;
         Definitions _defs;

         HashTableUInt32Template<NetStruct> _netTypeTable;
         HashTableUInt32Template<InternalStruct> _internalTypeTable;
         HashTableUInt32Template<MaskStruct> _maskTable;
         HashTableUInt32Template<RuntimeHandle> _whichTable;

      private:
         NetModuleAttributeMapBasic ();
         NetModuleAttributeMapBasic (const NetModuleAttributeMapBasic &);
         NetModuleAttributeMapBasic &operator= (const NetModuleAttributeMapBasic &);

   };
};

#endif // DMZ_NET_MODULE_ATTRIBUTE_MAP_BASIC_DOT_H
