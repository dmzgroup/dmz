#ifndef DMZ_OBJECT_MODULE_DOT_H
#define DMZ_OBJECT_MODULE_DOT_H

#include <dmzObjectConsts.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesString.h>

namespace dmz {

   //! \cond
   const char ObjectModuleInterfaceName[] = "ObjectModuleInterface";
   //! \endcond

   class Data;
   class Mask;
   class ObjectObserver;
   class ObjectType;
   class Matrix;
   class RuntimeContext;
   class UUID;
   class Vector;

   class ObjectModule {

      public:
         static ObjectModule *cast (
            const Plugin *PluginPtr,
            const String &PluginName ="");

         virtual Boolean register_global_object_observer (ObjectObserver &observer) = 0;
         virtual Boolean release_global_object_observer (ObjectObserver &observer) = 0;

         virtual Boolean register_object_observer (
            const Handle AttributeHandle,
            const Mask &AttributeMask,
            ObjectObserver &observer) = 0;

         virtual Boolean release_object_observer (
            const Handle AttributeHandle,
            const Mask &AttributeMask,
            ObjectObserver &observer) = 0;

         virtual Boolean release_object_observer_all (ObjectObserver &observer) = 0;

         virtual Boolean is_object (const Handle ObjectHandle) = 0;
         virtual Boolean is_link (const Handle LinkHandle) = 0;

         virtual Boolean get_object_handles (HandleContainer &container) = 0;

         virtual Boolean dump_object_attributes (
            const Handle ObjectHandle,
            ObjectObserver &Observer) = 0;

         virtual Boolean dump_all_object_attributes (
            const Handle ObjectHandle,
            ObjectObserver &Observer) = 0;

         virtual Handle create_object (
            const ObjectType &Type,
            const ObjectLocalityEnum Locality) = 0;

         virtual Boolean activate_object (const Handle ObjectHandle) = 0;

         virtual Boolean destroy_object (const Handle ObjectHandle) = 0;

         virtual Handle clone_object (
            const Handle ObjectHandle,
            const ObjectLinkRetentionEnum LinkRetention) = 0;

         virtual ObjectType lookup_object_type (const Handle ObjectHandle) = 0;
 
         virtual Boolean store_locality (
            const Handle ObjectHandle,
            const ObjectLocalityEnum Locality) = 0;

         virtual ObjectLocalityEnum lookup_locality (const Handle ObjectHandle) = 0;
         virtual ObjectLocalityEnum lookup_locality (const UUID &Identity) = 0;

         virtual Boolean store_uuid (const Handle ObjectHandle, const UUID &Value) = 0;
         virtual Boolean lookup_uuid (const Handle ObjectHandle, UUID &value) = 0;
         virtual Handle lookup_handle_from_uuid (const UUID &Value) = 0;

         virtual Boolean remove_attribute (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &AttrMask) = 0;

         virtual Handle link_objects (
            const Handle AttributeHandle,
            const Handle SuperHandle,
            const Handle SubHandle) = 0;

         virtual Handle lookup_link_handle (
            const Handle AttributeHandle,
            const Handle SuperHandle,
            const Handle SubHandle) = 0;

         virtual Boolean lookup_linked_objects (
            const Handle LinkHandle,
            Handle &attributeHandle,
            Handle &superHandle,
            Handle &subHandle) = 0;

         virtual Boolean unlink_objects (const Handle LinkHandle) = 0;

         virtual Boolean unlink_super_links (
            const Handle ObjectHandle,
            const Handle AttributeHandle) = 0;

         virtual Boolean unlink_sub_links (
            const Handle ObjectHandle,
            const Handle AttributeHandle) = 0;

         virtual Boolean store_link_attribute_object (
            const Handle LinkHandle,
            const Handle AttributeObjectHandle) = 0;

         virtual Handle lookup_link_attribute_object (const Handle LinkHandle) = 0;

         virtual Boolean lookup_super_links (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            HandleContainer &container) = 0;

         virtual Boolean lookup_sub_links (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            HandleContainer &container) = 0;

         virtual Boolean store_counter (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Int64 Value) = 0;

         virtual Boolean lookup_counter (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Int64 &value) = 0;

         virtual Int64 add_to_counter (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Int64 Value = 1) = 0;

         virtual Boolean store_counter_minimum (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Int64 Value) = 0;

         virtual Boolean lookup_counter_minimum (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Int64 &value) = 0;

         virtual Boolean store_counter_maximum (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Int64 Value) = 0;

         virtual Boolean lookup_counter_maximum (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Int64 &value) = 0;

         virtual Boolean store_counter_rollover (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value) = 0;

         virtual Boolean lookup_counter_rollover (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Boolean &value) = 0;

         virtual Boolean store_alternate_object_type (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const ObjectType &Value) = 0;

         virtual Boolean lookup_alternate_object_type (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            ObjectType &value) = 0;

         virtual Boolean store_state (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &Value) = 0;

         virtual Boolean lookup_state (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Mask &value) = 0;

         virtual Boolean store_flag (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value) = 0;

         virtual Boolean lookup_flag (
            const Handle ObjectHandle,
            const Handle AttributeHandle) = 0;

         virtual Boolean store_time_stamp (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value) = 0;

         virtual Boolean lookup_time_stamp (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Float64 &value) = 0;

         virtual Boolean store_position (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value) = 0;

         virtual Boolean lookup_position (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Vector &value) = 0;

         virtual Boolean store_orientation (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Matrix &Value) = 0;

         virtual Boolean lookup_orientation (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Matrix &value) = 0;

         virtual Boolean store_velocity (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value) = 0;

         virtual Boolean lookup_velocity (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Vector &value) = 0;

         virtual Boolean store_acceleration (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value) = 0;

         virtual Boolean lookup_acceleration (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Vector &value) = 0;

         virtual Boolean store_scale (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value) = 0;

         virtual Boolean lookup_scale (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Vector &value) = 0;

         virtual Boolean store_vector (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value) = 0;

         virtual Boolean lookup_vector (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Vector &value) = 0;

         virtual Boolean store_scalar (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value) = 0;

         virtual Boolean lookup_scalar (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Float64 &value) = 0;

         virtual Boolean store_text (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const String &Value) = 0;

         virtual Boolean lookup_text (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            String &value) = 0;

         virtual Boolean store_data (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Data &Value) = 0;

         virtual Boolean lookup_data (
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            Data &value) = 0;

      protected:
         ObjectModule (const PluginInfo &Info);
         ~ObjectModule ();

      private:
         ObjectModule ();
         ObjectModule (const ObjectModule &);
         ObjectModule &operator= (const ObjectModule &);

         const PluginInfo &__Info;
   };
};


inline dmz::ObjectModule *
dmz::ObjectModule::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (ObjectModule *)lookup_rtti_interface (
      ObjectModuleInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::ObjectModule::ObjectModule (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (ObjectModuleInterfaceName, __Info, (void *)this);
}


inline
dmz::ObjectModule::~ObjectModule () {

   remove_rtti_interface (ObjectModuleInterfaceName, __Info);
}

#endif // DMZ_OBJECT_MODULE_DOT_H
