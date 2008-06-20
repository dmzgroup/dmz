#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableHandleTemplate.h>

/*!

\class dmz::ObjectObserverUtil
\ingroup Object
\brief Object observer utility class.
\details

*/

namespace {

struct attrStruct {

   const dmz::Handle AttributeHandle;
   dmz::Mask mask;

   attrStruct (const dmz::Handle TheHandle) : AttributeHandle (TheHandle) {;}
};

};


struct dmz::ObjectObserverUtil::State {

   const String ObjectModuleName;
   Log log;
   Definitions defs;
   ObjectModule *module;
   HashTableHandleTemplate<attrStruct> handleTable;
   Mask errorMask;
   Boolean global;

   State (const PluginInfo &Info, const Config &Init) :
         ObjectModuleName (config_to_string ("module.object.name", Init)),
         log (Info.get_name () + ".ObjectObserverUtil", Info.get_context ()),
         defs (Info, &log),
         module (0),
         global (False) {

      if (ObjectModuleName) {

         log.info << "Looking for Object Module: " << ObjectModuleName << endl;
      }
   }

   ~State () { handleTable.empty (); }

   void register_obs (attrStruct &as, ObjectObserver &obs) {

      if (as.AttributeHandle && module) {

         module->register_object_observer (as.AttributeHandle, as.mask, obs);
      }
   }

   void release_obs (const Mask &EventMask, attrStruct &as, ObjectObserver &obs) {

      if (as.AttributeHandle && module) {

         module->release_object_observer (as.AttributeHandle, EventMask, obs);
         as.mask.unset (EventMask);
      }
   }
};


/*!

\brief Constructor
\details The ObjectObserverUtil uses the Config object passed in to specify which
ObjectModule to register with. This is done by giving the name of the ObjectModule. The
format is as follows:
\code
<module>
   <object name="Object Module Name"/>
</module>
\endcode
If the ObjectModule name is not specified, the observer will register with the first
ObjectModule the discovers it.
\param[in] Info PluginInfo used to initialize the Plugin that is derived from this class.
\param[in] Init Config used to initialize the class. This is most often the Config object
passed in as local to the Plugin.

*/
dmz::ObjectObserverUtil::ObjectObserverUtil (
      const PluginInfo &Info,
      const Config &Init) :
      ObjectObserver (Info),
      __state (*(new State (Info, Init))) {;}


dmz::ObjectObserverUtil::~ObjectObserverUtil () {

   if (__state.module) { remove_object_module ("", *(__state.module)); }
   delete &__state;
}


/*!

\brief Activates object observer as a global observer.
\details When an object observer is activated as a global observer the object observer
will receive \b all object attribute updates.

*/
void
dmz::ObjectObserverUtil::activate_global_object_observer () {

   __state.global = True;

   if (__state.module) {

      __state.module->register_global_object_observer (*this);
   }
}


/*!

\brief Deactivates object observer as a global observer.
\details When an object observer is deactivated as a global observer the object observer
will only receive attributes updates that have been activated explicitly with
activate_object_attribute().

*/
void
dmz::ObjectObserverUtil::deactivate_global_object_observer () {

   __state.global = False;

   if (__state.module) {

      __state.module->release_global_object_observer (*this);
   }
}


/*!

\brief Activates callbacks for specific object attribute updates.
\details
\param[in] AttributeTypeName String containing the name of the attribute type to activate.
\param[in] AttributeMask Mask containing the attributes to activate. The masks defined
in dmzObjectAttributeMasks.h should be used to compose \a AttributeMask.
\return Returns the attribute handle.
\sa ObjectModule

*/
dmz::Handle
dmz::ObjectObserverUtil::activate_object_attribute (
      const String &AttributeTypeName,
      const Mask &AttributeMask) {

   Handle handle (__state.defs.create_named_handle (AttributeTypeName));

   return activate_object_attribute (handle, AttributeMask);
}


/*!

\brief Activates callbacks for specific object attribute updates.
\details
\param[in] AttributeType Handle containing the attribute type handle to activate.
\param[in] AttributeMask Mask containing the attributes to activate. The masks defined
in dmzObjectAttributeMasks.h should be used to compose \a AttributeMask.
\return Returns the attribute handle.
\sa ObjectModule

*/
dmz::Handle
dmz::ObjectObserverUtil::activate_object_attribute (
      const Handle AttributeType,
      const Mask &AttributeMask) {

   attrStruct *as (__state.handleTable.lookup (AttributeType));

   if (!as && AttributeType) {

      as = new attrStruct (AttributeType);

      if (!__state.handleTable.store (AttributeType, as)) {

         delete as; as = 0;
         __state.log.error << "Unable to activate object attribute: "
            << __state.defs.lookup_named_handle_name (AttributeType)
            << endl;
      }
   }

   if (as) {

      as->mask |= AttributeMask;
      __state.register_obs (*as, *this);
   }

   return AttributeType;
}


/*!

\brief Activates callbacks for default object attribute updates.
\details
\param[in] AttributeMask Mask containing the attributes to activate. The masks defined
in dmzObjectAttributeMasks.h should be used to compose \a AttributeMask.
\return Returns the default attribute handle.
\sa ObjectModule

*/
dmz::Handle
dmz::ObjectObserverUtil::activate_default_object_attribute (const Mask &AttributeMask) {

   return activate_object_attribute (ObjectAttributeDefaultName, AttributeMask);
}


/*!

\brief Deactivates callbacks for specific object attribute updates.
\details
\param[in] AttributeTypeName String containing the name of the attribute type to
deactivate.
\param[in] AttributeMask Mask containing the attributes to deactivate. The masks defined
in dmzObjectAttributeMasks.h should be used to compose \a AttributeMask.
\sa ObjectModule

*/
void
dmz::ObjectObserverUtil::deactivate_object_attribute (
      const String &AttributeTypeName,
      const Mask &AttributeMask) {

   deactivate_object_attribute (
      __state.defs.lookup_named_handle (AttributeTypeName),
      AttributeMask);
}


/*!

\brief Deactivates callbacks for specific object attribute updates.
\details
\param[in] AttributeHandle Handle containing the attribute type handle to deactivate.
\param[in] AttributeMask Mask containing the attributes to deactivate. The masks defined
in dmzObjectAttributeMasks.h should be used to compose \a AttributeMask.
\sa ObjectModule

*/
void
dmz::ObjectObserverUtil::deactivate_object_attribute (
      const Handle AttributeHandle,
      const Mask &AttributeMask) {

  attrStruct *as (__state.handleTable.lookup (AttributeHandle));

  if (as) { __state.release_obs (AttributeMask, *as, *this); }
}


/*!

\brief Deactivates callbacks for default object attribute updates.
\details
\param[in] AttributeMask Mask containing the attributes to deactivate. The masks defined
in dmzObjectAttributeMasks.h should be used to compose \a AttributeMask.
\sa ObjectModule

*/
void
dmz::ObjectObserverUtil::deactivate_default_object_attribute (const Mask &AttributeMask) {

   deactivate_object_attribute (ObjectAttributeDefaultName, AttributeMask);
}


/*!

\brief Gets object module with which the observer is registered.
\return Returns a pointer to the ObjectModule with which the observer is registered.
Returns NULL if the observer is not registered with any ObjectModule.

*/
dmz::ObjectModule *
dmz::ObjectObserverUtil::get_object_module () { return __state.module; }


// ObjectObserer Interface.
void
dmz::ObjectObserverUtil::store_object_module (const String &Name, ObjectModule &module) {

   if (!__state.ObjectModuleName || (Name == __state.ObjectModuleName)) {

      if (!__state.module) {

         __state.module = &module;

         HashTableHandleIterator it;

         attrStruct *as (__state.handleTable.get_first (it));

         while (as) {

            __state.register_obs (*as, *this);
            as = __state.handleTable.get_next (it);
         }

         if (__state.global) {

            __state.module->register_global_object_observer (*this);
         }

         _store_object_module (module);
      }
   }
}


void
dmz::ObjectObserverUtil::remove_object_module (const String &Name, ObjectModule &module) {

   if (__state.module == &module) {

      _remove_object_module (module);

      HashTableHandleIterator it;

      attrStruct *as (__state.handleTable.get_first (it));

      while (as) {

         __state.release_obs (ObjectAllMask, *as, *this);
         as = __state.handleTable.get_next (it);
      }

      if (__state.global) {

         __state.module->release_global_object_observer (*this);
      }

      __state.module = 0;
   }
}


void
dmz::ObjectObserverUtil::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (!(ObjectCreateMask & __state.errorMask)) {

      __state.errorMask |= ObjectCreateMask;
      __state.log.warn << "Base create_object called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   if (!(ObjectDestroyMask & __state.errorMask)) {

      __state.errorMask |= ObjectDestroyMask;
      __state.log.warn << "Base destroy_object called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::update_object_uuid (
      const Handle ObjectHandle,
      const UUID &Identity,
      const UUID &PrevIdentity) {

   if (!(ObjectUUIDMask & __state.errorMask)) {

      __state.errorMask |= ObjectUUIDMask;
      __state.log.warn << "Base update_object_uuid called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::remove_object_attribute (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &AttrMask) {

   if (!(ObjectRemoveAttributeMask & __state.errorMask)) {

      __state.errorMask |= ObjectRemoveAttributeMask;
      __state.log.warn << "Base remove_objectattribute called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::update_object_locality (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectLocalityEnum Locality,
      const ObjectLocalityEnum PrevLocality) {

   if (!(ObjectLocalityMask & __state.errorMask)) {

      __state.errorMask |= ObjectLocalityMask;
      __state.log.warn << "Base update_object_locality called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   if (!(ObjectLinkMask & __state.errorMask)) {

      __state.errorMask |= ObjectLinkMask;
      __state.log.warn << "Base link_objects called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   if (!(ObjectUnlinkMask & __state.errorMask)) {

      __state.errorMask |= ObjectUnlinkMask;
      __state.log.warn << "Base unlink_objects called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::update_link_attribute_object (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle,
      const UUID &AttributeIdentity,
      const Handle AttributeObjectHandle,
      const UUID &PrevAttributeIdentity,
      const Handle PrevAttributeObjectHandle) {

   if (!(ObjectLinkAttributeMask & __state.errorMask)) {

      __state.errorMask |= ObjectLinkAttributeMask;
      __state.log.warn << "Base update_link_attribute_object called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::update_object_type (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const ObjectType &Value,
      const ObjectType *PreviousValue) {

   if (!(ObjectTypeMask & __state.errorMask)) {

      __state.errorMask |= ObjectTypeMask;
      __state.log.warn << "Base update_object_type called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   if (!(ObjectStateMask & __state.errorMask)) {

      __state.errorMask |= ObjectStateMask;
      __state.log.warn << "Base update_object_state called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (!(ObjectFlagMask & __state.errorMask)) {

      __state.errorMask |= ObjectFlagMask;
      __state.log.warn << "Base update_object_flag called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::update_object_time_stamp (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   if (!(ObjectTimeStampMask & __state.errorMask)) {

      __state.errorMask |= ObjectTimeStampMask;
      __state.log.warn << "Base update_object_time_stamp called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (!(ObjectPositionMask & __state.errorMask)) {

      __state.errorMask |= ObjectPositionMask;
      __state.log.warn << "Base update_object_position called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

   if (!(ObjectOrientationMask & __state.errorMask)) {

      __state.errorMask |= ObjectOrientationMask;
      __state.log.warn << "Base update_object_orientation called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::update_object_velocity (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (!(ObjectVelocityMask & __state.errorMask)) {

      __state.errorMask |= ObjectVelocityMask;
      __state.log.warn << "Base update_object_velocity called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::update_object_acceleration (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (!(ObjectAccelerationMask & __state.errorMask)) {

      __state.errorMask |= ObjectAccelerationMask;
      __state.log.warn << "Base update_object_acceleration called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::update_object_scale (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (!(ObjectScaleMask & __state.errorMask)) {

      __state.errorMask |= ObjectScaleMask;
      __state.log.warn << "Base update_object_scale called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::update_object_vector (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (!(ObjectVectorMask & __state.errorMask)) {

      __state.errorMask |= ObjectVectorMask;
      __state.log.warn << "Base update_object_vector called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   if (!(ObjectScalarMask & __state.errorMask)) {

      __state.errorMask |= ObjectScalarMask;
      __state.log.warn << "Base update_object_scalar called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::update_object_text (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const String &Value,
      const String *PreviousValue) {

   if (!(ObjectTextMask & __state.errorMask)) {

      __state.errorMask |= ObjectTextMask;
      __state.log.warn << "Base update_object_text called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::ObjectObserverUtil::update_object_data (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Data &Value,
      const Data *PreviousValue) {

   if (!(ObjectDataMask & __state.errorMask)) {

      __state.errorMask |= ObjectDataMask;
      __state.log.warn << "Base update_object_data called."
         << " Function should have been overridden?" << endl;
   }
}

/*!

\fn void dmz::ObjectObserverUtil::_store_object_module (ObjectModule &objMod) {;}
\brief Callback made when object module is discovered.

\fn void dmz::ObjectObserverUtil::_remove_object_module (ObjectModule &objMod) {;}
\brief Callback made when object module is removed.

*/
