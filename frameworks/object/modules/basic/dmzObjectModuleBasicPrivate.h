#ifndef DMZ_OBJECT_MODULE_BASIC_PRIVATE_DOT_H
#define DMZ_OBJECT_MODULE_BASIC_PRIVATE_DOT_H

#include "dmzObjectModuleBasic.h"
#include <dmzRuntimeHandle.h>
#include <dmzTypesBase.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeObjectType.h>
#include <dmzTypesMask.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesString.h>
#include <dmzTypesVector.h>
#include <dmzTypesUUID.h>

using namespace dmz;

namespace {

   typedef ObjectModuleBasic::ObsUpdateStruct ostruct;

   struct ReleaseGlobalObserverStruct : public ostruct {

      ObjectObserver &observer;

      ReleaseGlobalObserverStruct (ObjectObserver &theObserver) :
            observer (theObserver) {;}

      virtual void update (ObjectModuleBasic &module) {

         module.immediate_release_global_object_observer (observer);
      }
   };

   struct ReleaseObserverStruct : public ostruct {

      const Handle AttributeHandle;
      const Mask AttributeMask;
      ObjectObserver &observer;

      ReleaseObserverStruct (
            const Handle TheAttributeHandle,
            const Mask TheAttributeMask,
            ObjectObserver &theObserver) :
            AttributeHandle (TheAttributeHandle),
            AttributeMask (TheAttributeMask),
            observer (theObserver) {;}

      virtual void update (ObjectModuleBasic &module) {

         module.immediate_release_object_observer (
            AttributeHandle,
            AttributeMask,
            observer);
      }
   };

   struct ReleaseObserverAllStruct : public ostruct {

      ObjectObserver &observer;

      ReleaseObserverAllStruct (ObjectObserver &theObserver) : observer (theObserver) {;}

      virtual void update (ObjectModuleBasic &module) {

         module.immediate_release_object_observer_all (observer);
      }
   };

   struct CreateObjectStruct : public ostruct {

      const Handle ObjectHandle;

      CreateObjectStruct (const Handle TheHandle) : ObjectHandle (TheHandle) {;}

      virtual void update (ObjectModuleBasic &module) {

         module.activate_created_object (ObjectHandle);
      }
   };

   struct DestroyObjectStruct : public ostruct {

      const Handle ObjectHandle;

      DestroyObjectStruct (const Handle TheHandle) : ObjectHandle (TheHandle) {;}

      virtual void update (ObjectModuleBasic &module) {

         module.immediate_destroy_object (ObjectHandle);
      }
   };

   struct ObjectUUIDStruct : public ostruct {

      const Handle ObjectHandle;
      const UUID Identity;
      const UUID PrevIdentity;

      ObjectUUIDStruct (
            const Handle TheHandle,
            const UUID &TheIdentity,
            const UUID &ThePrevIdentity) :
            ObjectHandle (TheHandle),
            Identity (TheIdentity),
            PrevIdentity (ThePrevIdentity) {;}

      virtual void update (ObjectModuleBasic &module) {

         module.update_object_uuid (ObjectHandle, Identity, PrevIdentity);
      }
   };

   struct RemoveObjectAttrStruct : public ostruct {

      const UUID Identity;
      const Handle ObjectHandle;
      const Handle AttributeHandle;
      const Mask AttrMask;

      RemoveObjectAttrStruct (
            const UUID &TheIdentity,
            const Handle TheObjectHandle,
            const Handle TheAttributeHandle,
            const Mask &TheAttrMask) :
            Identity (TheIdentity),
            ObjectHandle (TheObjectHandle),
            AttributeHandle (TheAttributeHandle),
            AttrMask (TheAttrMask) {;}

      virtual void update (ObjectModuleBasic &module) {

         module.remove_object_attribute (
            Identity,
            ObjectHandle,
            AttributeHandle,
            AttrMask);
      }
   };

   struct ObjectLocalityStruct : public ostruct {

      const UUID Identity;
      const Handle ObjectHandle;
      const ObjectLocalityEnum Locality;
      const ObjectLocalityEnum PrevLocality;

      ObjectLocalityStruct (
            const UUID &TheIdentity,
            const Handle TheObjectHandle,
            const ObjectLocalityEnum TheLocality,
            const ObjectLocalityEnum ThePrevLocality) :
            Identity (TheIdentity),
            ObjectHandle (TheObjectHandle),
            Locality (TheLocality),
            PrevLocality (ThePrevLocality) {;}

      virtual void update (ObjectModuleBasic &module) {

         module.update_object_locality (Identity, ObjectHandle, Locality, PrevLocality);
      }
   };

   struct LinkObjectsStruct : public ostruct {

      const Handle LinkHandle;
      const Handle AttributeHandle;
      const UUID SuperIdentity;
      const Handle SuperHandle;
      const UUID SubIdentity;
      const Handle SubHandle;

      LinkObjectsStruct (
            const Handle TheLinkHandle,
            const Handle TheAttributeHandle,
            const UUID &TheSuperIdentity,
            const Handle TheSuperHandle,
            const UUID &TheSubIdentity,
            const Handle TheSubHandle) :
            LinkHandle (TheLinkHandle),
            AttributeHandle (TheAttributeHandle),
            SuperIdentity (TheSuperIdentity),
            SuperHandle (TheSuperHandle),
            SubIdentity (TheSubIdentity),
            SubHandle (TheSubHandle) {;}

      virtual void update (ObjectModuleBasic &module) {

         module.link_objects (
            LinkHandle,
            AttributeHandle,
            SuperIdentity,
            SuperHandle,
            SubIdentity,
            SubHandle);
      }
   };

   struct UnlinkObjectsStruct : public ostruct {

      const Handle LinkHandle;
      const Handle AttributeHandle;
      const UUID SuperIdentity;
      const Handle SuperHandle;
      const UUID SubIdentity;
      const Handle SubHandle;

      UnlinkObjectsStruct (
            const Handle TheLinkHandle,
            const Handle TheAttributeHandle,
            const UUID &TheSuperIdentity,
            const Handle TheSuperHandle,
            const UUID &TheSubIdentity,
            const Handle TheSubHandle) :
            LinkHandle (TheLinkHandle),
            AttributeHandle (TheAttributeHandle),
            SuperIdentity (TheSuperIdentity),
            SuperHandle (TheSuperHandle),
            SubIdentity (TheSubIdentity),
            SubHandle (TheSubHandle) {;}

      virtual void update (ObjectModuleBasic &module) {

         module.unlink_objects (
            LinkHandle,
            AttributeHandle,
            SuperIdentity,
            SuperHandle,
            SubIdentity,
            SubHandle);
      }
   };

   struct LinkObjectAttrStruct : public ostruct {

      const Handle LinkHandle;
      const Handle AttributeHandle;
      const UUID SuperIdentity;
      const Handle SuperHandle;
      const UUID SubIdentity;
      const Handle SubHandle;
      const UUID AttributeIdentity;
      const Handle AttributeObjectHandle;
      const UUID PrevAttributeIdentity;
      const Handle PrevAttributeObjectHandle;

      LinkObjectAttrStruct (
            const Handle TheLinkHandle,
            const Handle TheAttributeHandle,
            const UUID &TheSuperIdentity,
            const Handle TheSuperHandle,
            const UUID &TheSubIdentity,
            const Handle TheSubHandle,
            const UUID &TheAttributeIdentity,
            const Handle TheAttributeObjectHandle,
            const UUID &ThePrevAttributeIdentity,
            const Handle ThePrevAttributeObjectHandle) :
            LinkHandle (TheLinkHandle),
            AttributeHandle (TheAttributeHandle),
            SuperIdentity (TheSuperIdentity),
            SuperHandle (TheSuperHandle),
            SubIdentity (TheSubIdentity),
            SubHandle (TheSubHandle),
            AttributeIdentity (TheAttributeIdentity),
            AttributeObjectHandle (TheAttributeObjectHandle),
            PrevAttributeIdentity (ThePrevAttributeIdentity),
            PrevAttributeObjectHandle (ThePrevAttributeObjectHandle) {;}

      virtual void update (ObjectModuleBasic &module) {

         module.update_link_attribute_object (
            LinkHandle,
            AttributeHandle,
            SuperIdentity,
            SuperHandle,
            SubIdentity,
            SubHandle,
            AttributeIdentity,
            AttributeObjectHandle,
            PrevAttributeIdentity,
            PrevAttributeObjectHandle);
      }
   };

   enum CounterStructEnum {
      CounterValue,
      CounterMin,
      CounterMax,
   };

   struct UpdateCounterStruct : public ostruct {

      const CounterStructEnum Type;
      const UUID Identity;
      const Handle ObjectHandle;
      const Handle AttributeHandle;
      const Int64 Value;
      const Int64 *PrevValue;

      UpdateCounterStruct (
            const CounterStructEnum TheType,
            const UUID &TheIdentity,
            const Handle TheObjectHandle,
            const Handle TheAttributeHandle,
            const Int64 &TheValue,
            const Int64 *ThePrevValue) :
            Type (TheType),
            Identity (TheIdentity),
            ObjectHandle (TheObjectHandle),
            AttributeHandle (TheAttributeHandle),
            Value (TheValue),
            PrevValue (ThePrevValue ? new Int64 (*ThePrevValue) : 0) {;}

      virtual ~UpdateCounterStruct () {

         if (PrevValue) { delete PrevValue; PrevValue = 0; }
      }

      virtual void update (ObjectModuleBasic &module) {

         if (Type == CounterValue) {

            module.update_object_counter (
               Identity,
               ObjectHandle,
               AttributeHandle,
               Value,
               PrevValue);
         }
         else if (Type == CounterMin) {

            module.update_object_counter_minimum (
               Identity,
               ObjectHandle,
               AttributeHandle,
               Value,
               PrevValue);
         }
         else if (Type == CounterMax) {

            module.update_object_counter_maximum (
               Identity,
               ObjectHandle,
               AttributeHandle,
               Value,
               PrevValue);
         }
      }
   };

   struct AltObjectTypeStruct : public ostruct {

      const UUID Identity;
      const Handle ObjectHandle;
      const Handle AttributeHandle;
      const ObjectType Value;
      const ObjectType *PrevValue;

      AltObjectTypeStruct (
            const UUID &TheIdentity,
            const Handle TheObjectHandle,
            const Handle TheAttributeHandle,
            const ObjectType &TheValue,
            const ObjectType *ThePrevValue) :
            Identity (TheIdentity),
            ObjectHandle (TheObjectHandle),
            AttributeHandle (TheAttributeHandle),
            Value (TheValue),
            PrevValue (ThePrevValue ? new ObjectType (*ThePrevValue) : 0) {;}

      virtual ~AltObjectTypeStruct () {

         if (PrevValue) { delete PrevValue; PrevValue = 0; }
      }

      virtual void update (ObjectModuleBasic &module) {

         module.update_object_alternate_type (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PrevValue);
      }
   };

   struct ObjectStateStruct : public ostruct {

      const UUID Identity;
      const Handle ObjectHandle;
      const Handle AttributeHandle;
      const Mask Value;
      const Mask *PrevValue;

      ObjectStateStruct (
            const UUID &TheIdentity,
            const Handle TheObjectHandle,
            const Handle TheAttributeHandle,
            const Mask &TheValue,
            const Mask *ThePrevValue) :
            Identity (TheIdentity),
            ObjectHandle (TheObjectHandle),
            AttributeHandle (TheAttributeHandle),
            Value (TheValue),
            PrevValue (ThePrevValue ? new Mask (*ThePrevValue) : 0) {;}

      virtual ~ObjectStateStruct () {

         if (PrevValue) { delete PrevValue; PrevValue = 0; }
      }

      virtual void update (ObjectModuleBasic &module) {

         module.update_object_state (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PrevValue);
      }
   };

   struct ObjectFlagStruct : public ostruct {

      const UUID Identity;
      const Handle ObjectHandle;
      const Handle AttributeHandle;
      const Boolean Value;
      const Boolean *PrevValue;

      ObjectFlagStruct (
            const UUID &TheIdentity,
            const Handle TheObjectHandle,
            const Handle TheAttributeHandle,
            const Boolean &TheValue,
            const Boolean *ThePrevValue) :
            Identity (TheIdentity),
            ObjectHandle (TheObjectHandle),
            AttributeHandle (TheAttributeHandle),
            Value (TheValue),
            PrevValue (ThePrevValue ? new Boolean (*ThePrevValue) : 0) {;}

      virtual ~ObjectFlagStruct () {

         if (PrevValue) { delete PrevValue; PrevValue = 0; }
      }

      virtual void update (ObjectModuleBasic &module) {

         module.update_object_flag (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PrevValue);
      }
   };

   struct ObjectTimeStampStruct : public ostruct {

      const UUID Identity;
      const Handle ObjectHandle;
      const Handle AttributeHandle;
      const Float64 Value;
      const Float64 *PrevValue;

      ObjectTimeStampStruct (
            const UUID &TheIdentity,
            const Handle TheObjectHandle,
            const Handle TheAttributeHandle,
            const Float64 &TheValue,
            const Float64 *ThePrevValue) :
            Identity (TheIdentity),
            ObjectHandle (TheObjectHandle),
            AttributeHandle (TheAttributeHandle),
            Value (TheValue),
            PrevValue (ThePrevValue ? new Float64 (*ThePrevValue) : 0) {;}

      virtual ~ObjectTimeStampStruct () {

         if (PrevValue) { delete PrevValue; PrevValue = 0; }
      }

      virtual void update (ObjectModuleBasic &module) {

         module.update_object_time_stamp (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PrevValue);
      }
   };

   struct ObjectPositionStruct : public ostruct {

      const UUID Identity;
      const Handle ObjectHandle;
      const Handle AttributeHandle;
      const Vector Value;
      const Vector *PrevValue;

      ObjectPositionStruct (
            const UUID &TheIdentity,
            const Handle TheObjectHandle,
            const Handle TheAttributeHandle,
            const Vector &TheValue,
            const Vector *ThePrevValue) :
            Identity (TheIdentity),
            ObjectHandle (TheObjectHandle),
            AttributeHandle (TheAttributeHandle),
            Value (TheValue),
            PrevValue (ThePrevValue ? new Vector (*ThePrevValue) : 0) {;}

      virtual ~ObjectPositionStruct () {

         if (PrevValue) { delete PrevValue; PrevValue = 0; }
      }

      virtual void update (ObjectModuleBasic &module) {

         module.update_object_position (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PrevValue);
      }
   };

   struct ObjectOrientationStruct : public ostruct {

      const UUID Identity;
      const Handle ObjectHandle;
      const Handle AttributeHandle;
      const Matrix Value;
      const Matrix *PrevValue;

      ObjectOrientationStruct (
            const UUID &TheIdentity,
            const Handle TheObjectHandle,
            const Handle TheAttributeHandle,
            const Matrix &TheValue,
            const Matrix *ThePrevValue) :
            Identity (TheIdentity),
            ObjectHandle (TheObjectHandle),
            AttributeHandle (TheAttributeHandle),
            Value (TheValue),
            PrevValue (ThePrevValue ? new Matrix (*ThePrevValue) : 0) {;}

      virtual ~ObjectOrientationStruct () {

         if (PrevValue) { delete PrevValue; PrevValue = 0; }
      }

      virtual void update (ObjectModuleBasic &module) {

         module.update_object_orientation (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PrevValue);
      }
   };

   struct ObjectVelocityStruct : public ostruct {

      const UUID Identity;
      const Handle ObjectHandle;
      const Handle AttributeHandle;
      const Vector Value;
      const Vector *PrevValue;

      ObjectVelocityStruct (
            const UUID &TheIdentity,
            const Handle TheObjectHandle,
            const Handle TheAttributeHandle,
            const Vector &TheValue,
            const Vector *ThePrevValue) :
            Identity (TheIdentity),
            ObjectHandle (TheObjectHandle),
            AttributeHandle (TheAttributeHandle),
            Value (TheValue),
            PrevValue (ThePrevValue ? new Vector (*ThePrevValue) : 0) {;}

      virtual ~ObjectVelocityStruct () {

         if (PrevValue) { delete PrevValue; PrevValue = 0; }
      }

      virtual void update (ObjectModuleBasic &module) {

         module.update_object_velocity (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PrevValue);
      }
   };

   struct ObjectAccelerationStruct : public ostruct {

      const UUID Identity;
      const Handle ObjectHandle;
      const Handle AttributeHandle;
      const Vector Value;
      const Vector *PrevValue;

      ObjectAccelerationStruct (
            const UUID &TheIdentity,
            const Handle TheObjectHandle,
            const Handle TheAttributeHandle,
            const Vector &TheValue,
            const Vector *ThePrevValue) :
            Identity (TheIdentity),
            ObjectHandle (TheObjectHandle),
            AttributeHandle (TheAttributeHandle),
            Value (TheValue),
            PrevValue (ThePrevValue ? new Vector (*ThePrevValue) : 0) {;}

      virtual ~ObjectAccelerationStruct () {

         if (PrevValue) { delete PrevValue; PrevValue = 0; }
      }

      virtual void update (ObjectModuleBasic &module) {

         module.update_object_acceleration (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PrevValue);
      }
   };

   struct ObjectScaleStruct : public ostruct {

      const UUID Identity;
      const Handle ObjectHandle;
      const Handle AttributeHandle;
      const Vector Value;
      const Vector *PrevValue;

      ObjectScaleStruct (
            const UUID &TheIdentity,
            const Handle TheObjectHandle,
            const Handle TheAttributeHandle,
            const Vector &TheValue,
            const Vector *ThePrevValue) :
            Identity (TheIdentity),
            ObjectHandle (TheObjectHandle),
            AttributeHandle (TheAttributeHandle),
            Value (TheValue),
            PrevValue (ThePrevValue ? new Vector (*ThePrevValue) : 0) {;}

      virtual ~ObjectScaleStruct () {

         if (PrevValue) { delete PrevValue; PrevValue = 0; }
      }

      virtual void update (ObjectModuleBasic &module) {

         module.update_object_scale (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PrevValue);
      }
   };

   struct ObjectVectorStruct : public ostruct {

      const UUID Identity;
      const Handle ObjectHandle;
      const Handle AttributeHandle;
      const Vector Value;
      const Vector *PrevValue;

      ObjectVectorStruct (
            const UUID &TheIdentity,
            const Handle TheObjectHandle,
            const Handle TheAttributeHandle,
            const Vector &TheValue,
            const Vector *ThePrevValue) :
            Identity (TheIdentity),
            ObjectHandle (TheObjectHandle),
            AttributeHandle (TheAttributeHandle),
            Value (TheValue),
            PrevValue (ThePrevValue ? new Vector (*ThePrevValue) : 0) {;}

      virtual ~ObjectVectorStruct () {

         if (PrevValue) { delete PrevValue; PrevValue = 0; }
      }

      virtual void update (ObjectModuleBasic &module) {

         module.update_object_vector (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PrevValue);
      }
   };

   struct ObjectScalarStruct : public ostruct {

      const UUID Identity;
      const Handle ObjectHandle;
      const Handle AttributeHandle;
      const Float64 Value;
      const Float64 *PrevValue;

      ObjectScalarStruct (
            const UUID &TheIdentity,
            const Handle TheObjectHandle,
            const Handle TheAttributeHandle,
            const Float64 &TheValue,
            const Float64 *ThePrevValue) :
            Identity (TheIdentity),
            ObjectHandle (TheObjectHandle),
            AttributeHandle (TheAttributeHandle),
            Value (TheValue),
            PrevValue (ThePrevValue ? new Float64 (*ThePrevValue) : 0) {;}

      virtual ~ObjectScalarStruct () {

         if (PrevValue) { delete PrevValue; PrevValue = 0; }
      }

      virtual void update (ObjectModuleBasic &module) {

         module.update_object_scalar (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PrevValue);
      }
   };

   struct ObjectTextStruct : public ostruct {

      const UUID Identity;
      const Handle ObjectHandle;
      const Handle AttributeHandle;
      const String Value;
      const String *PrevValue;

      ObjectTextStruct (
            const UUID &TheIdentity,
            const Handle TheObjectHandle,
            const Handle TheAttributeHandle,
            const String &TheValue,
            const String *ThePrevValue) :
            Identity (TheIdentity),
            ObjectHandle (TheObjectHandle),
            AttributeHandle (TheAttributeHandle),
            Value (TheValue),
            PrevValue (ThePrevValue ? new String (*ThePrevValue) : 0) {;}

      virtual ~ObjectTextStruct () {

         if (PrevValue) { delete PrevValue; PrevValue = 0; }
      }

      virtual void update (ObjectModuleBasic &module) {

         module.update_object_text (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PrevValue);
      }
   };

   struct ObjectDataStruct : public ostruct {

      const UUID Identity;
      const Handle ObjectHandle;
      const Handle AttributeHandle;
      const Data Value;
      const Data *PrevValue;

      ObjectDataStruct (
            const UUID &TheIdentity,
            const Handle TheObjectHandle,
            const Handle TheAttributeHandle,
            const Data &TheValue,
            const Data *ThePrevValue) :
            Identity (TheIdentity),
            ObjectHandle (TheObjectHandle),
            AttributeHandle (TheAttributeHandle),
            Value (TheValue),
            PrevValue (ThePrevValue ? new Data (*ThePrevValue) : 0) {;}

      virtual ~ObjectDataStruct () {

         if (PrevValue) { delete PrevValue; PrevValue = 0; }
      }

      virtual void update (ObjectModuleBasic &module) {

         module.update_object_data (
            Identity,
            ObjectHandle,
            AttributeHandle,
            Value,
            PrevValue);
      }
   };
}
//! \endcond

#endif // DMZ_OBJECT_MODULE_BASIC_PRIVATE_DOT_H
