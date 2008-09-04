#ifndef DMZ_OBJECT_OBSERVER_DOT_H
#define DMZ_OBJECT_OBSERVER_DOT_H

#include <dmzObjectConsts.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

#define DMZ_OBJECT_OBSERVER_INTERFACE_NAME "ObjectObserverInteface"

namespace dmz {

   class Data;
   class Mask;
   class ObjectObserver;
   class ObjectModule;
   class ObjectType;
   class Matrix;
   class RuntimeContext;
   class UUID;
   class Vector;

   class ObjectObserver {

      public:
         static ObjectObserver *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         static Boolean is_valid (const Handle ObserverHandle, RuntimeContext *context);

         String get_object_observer_name ();
         Handle get_object_observer_handle ();

         virtual void store_object_module (const String &Name, ObjectModule &module) = 0;
         virtual void remove_object_module (const String &Name, ObjectModule &module) = 0;

         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality) = 0;

         virtual void destroy_object (
            const UUID &Identity,
            const Handle ObjectHandle) = 0;

         virtual void update_object_locality (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectLocalityEnum Locality,
            const ObjectLocalityEnum PrevLocality) = 0;

         virtual void update_object_uuid (
            const Handle ObjectHandle,
            const UUID &Identity,
            const UUID &PrevIdentity) = 0;

         virtual void remove_object_attribute (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &AttrMask) = 0;

         virtual void link_objects (
            const Handle LinkHandle,
            const Handle AttributeHandle,
            const UUID &SuperIdentity,
            const Handle SuperHandle,
            const UUID &SubIdentity,
            const Handle SubHandle) = 0;

         virtual void unlink_objects (
            const Handle LinkHandle,
            const Handle AttributeHandle,
            const UUID &SuperIdentity,
            const Handle SuperHandle,
            const UUID &SubIdentity,
            const Handle SubHandle) = 0;

         virtual void update_link_attribute_object (
            const Handle LinkHandle,
            const Handle AttributeHandle,
            const UUID &SuperIdentity,
            const Handle SuperHandle,
            const UUID &SubIdentity,
            const Handle SubHandle,
            const UUID &AttributeIdentity,
            const Handle AttributeObjectHandle,
            const UUID &PrevAttributeIdentity,
            const Handle PrevAttributeObjectHandle) = 0;

         virtual void update_object_counter (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Int64 Value,
            const Int64 *PreviousValue) = 0;

         virtual void update_object_counter_minimum (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Int64 Value,
            const Int64 *PreviousValue) = 0;

         virtual void update_object_counter_maximum (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Int64 Value,
            const Int64 *PreviousValue) = 0;

         virtual void update_object_alternate_type (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const ObjectType &Value,
            const ObjectType *PreviousValue) = 0;

         virtual void update_object_state (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &Value,
            const Mask *PreviousValue) = 0;

         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue) = 0;

         virtual void update_object_time_stamp (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue) = 0;

         virtual void update_object_position (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue) = 0;

         virtual void update_object_orientation (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Matrix &Value,
            const Matrix *PreviousValue) = 0;

         virtual void update_object_velocity (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue) = 0;

         virtual void update_object_acceleration (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue) = 0;

         virtual void update_object_scale (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue) = 0;

         virtual void update_object_vector (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue) = 0;

         virtual void update_object_scalar (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue) = 0;

         virtual void update_object_text (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const String &Value,
            const String *PreviousValue) = 0;

         virtual void update_object_data (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Data &Value,
            const Data *PreviousValue) = 0;

      protected:
         ObjectObserver (const PluginInfo &Info);
         ~ObjectObserver ();

      private:
         ObjectObserver ();
         ObjectObserver (const ObjectObserver &);
         ObjectObserver &operator= (const ObjectObserver &);

         const PluginInfo &__Info;
   };
};


inline dmz::ObjectObserver *
dmz::ObjectObserver::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (ObjectObserver *)lookup_rtti_interface (
      DMZ_OBJECT_OBSERVER_INTERFACE_NAME,
      PluginName,
      PluginPtr);
}


inline dmz::Boolean
dmz::ObjectObserver::is_valid (const Handle ObserverHandle, RuntimeContext *context) {

   return (ObjectObserver *)lookup_rtti_interface (
      DMZ_OBJECT_OBSERVER_INTERFACE_NAME,
      ObserverHandle,
      context) != 0;
}


inline
dmz::ObjectObserver::ObjectObserver (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (DMZ_OBJECT_OBSERVER_INTERFACE_NAME, __Info, (void *)this);
}


inline
dmz::ObjectObserver::~ObjectObserver () {

   remove_rtti_interface (DMZ_OBJECT_OBSERVER_INTERFACE_NAME, __Info);
}


inline dmz::String
dmz::ObjectObserver::get_object_observer_name () { return __Info.get_name (); }


inline dmz::Handle
dmz::ObjectObserver::get_object_observer_handle () { return __Info.get_handle (); }

#endif // DMZ_OBJECT_OBSERVER_DOT_H

