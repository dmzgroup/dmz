#ifndef DMZ_NET_MODULE_ATTRIBUTE_MAP_DOT_H
#define DMZ_NET_MODULE_ATTRIBUTE_MAP_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesArrays.h>
#include <dmzTypesBase.h>

#define DMZ_NET_MODULE_ATTRIBUTE_MAP_INTERFACE_NAME "NetModuleAttributeMapInterface"

namespace dmz {

   class Mask;
   class ObjectType;
   class EventType;

   class NetModuleAttributeMap {

      public:
         static NetModuleAttributeMap *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         virtual Boolean to_net_object_mask (
            const ObjectType &Type,
            const Mask &InValue,
            ArrayUInt32 &outValue) = 0;

         virtual Boolean to_internal_object_mask (
            const ObjectType &Type,
            const ArrayUInt32 &InValue,
            Mask &outValue) = 0;

         virtual Boolean to_net_object_type (
            const ObjectType &Type,
            ArrayUInt32 &array) = 0;

         virtual Boolean to_internal_object_type (
            const ArrayUInt32 &Array,
            ObjectType &type) = 0;

         virtual Boolean to_net_event_mask (
            const EventType &Type,
            const Mask &InValue,
            ArrayUInt32 &outValue) = 0;

         virtual Boolean to_internal_event_mask (
            const EventType &Type,
            const ArrayUInt32 &InValue,
            Mask &outValue) = 0;

         virtual Boolean to_net_event_type (
            const EventType &Type,
            ArrayUInt32 &array) = 0;

         virtual Boolean to_internal_event_type (
            const ArrayUInt32 &Array,
            EventType &type) = 0;

      protected:
         NetModuleAttributeMap (const PluginInfo &Info);
         ~NetModuleAttributeMap ();

      private:
         NetModuleAttributeMap ();
         NetModuleAttributeMap (const NetModuleAttributeMap &);
         NetModuleAttributeMap &operator= (const NetModuleAttributeMap &);
         const PluginInfo &__Info;
   };
};


inline dmz::NetModuleAttributeMap *
dmz::NetModuleAttributeMap::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (NetModuleAttributeMap *)lookup_rtti_interface (
      DMZ_NET_MODULE_ATTRIBUTE_MAP_INTERFACE_NAME,
      PluginName,
      PluginPtr);
}


inline
dmz::NetModuleAttributeMap::NetModuleAttributeMap (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (
      DMZ_NET_MODULE_ATTRIBUTE_MAP_INTERFACE_NAME,
      __Info,
     (void *)this);
}


inline
dmz::NetModuleAttributeMap::~NetModuleAttributeMap () {

   remove_rtti_interface (DMZ_NET_MODULE_ATTRIBUTE_MAP_INTERFACE_NAME, __Info);
}

#endif //  DMZ_NET_MODULE_ATTRIBUTE_MAP_DOT_H
