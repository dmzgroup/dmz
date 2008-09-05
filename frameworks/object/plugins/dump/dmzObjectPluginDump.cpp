#include <dmzObjectAttributeMasks.h>
#include "dmzObjectPluginDump.h"
#include <dmzRuntimeData.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesUUID.h>
#include <dmzTypesVector.h>

/*!

\class dmz::ObjectPluginDump
\brief Prints object attribute values when updated
\ingroup Object
\details The plugin is useful for debugging object attribute updates. Any time an
object attribute is updated in the ObjectModule, that updated value is sent to the
plugin's Log. This class should not be create explicitly but instead should be loaded
as a Plugin by the runtime. This Plugin currently has no configuration parameters.

*/


//! \cond
dmz::ObjectPluginDump::ObjectPluginDump (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defs (Info, &_log) {

   activate_global_object_observer ();
   _init (local);
}


dmz::ObjectPluginDump::~ObjectPluginDump () {

}


// Object Observer Interface
void
dmz::ObjectPluginDump::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   _log.debug << "Creating " << (Locality == ObjectLocal ? "Local" : "Remote")
      << "[" << ObjectHandle << "]" << " object of type: "
      << (Type.get_name () ? Type.get_name () : "<UNKNOWN>") << endl;
}


void
dmz::ObjectPluginDump::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   _log.debug << "Destroying object[" << ObjectHandle << "]:" << Identity << endl;
}


void
dmz::ObjectPluginDump::update_object_uuid (
      const Handle ObjectHandle,
      const UUID &Identity,
      const UUID &PrevIdentity) {

   _log.debug << "UUID[" << ObjectHandle << "]: " << Identity.to_string ();
   if (PrevIdentity) { _log.debug << " previous: " << PrevIdentity.to_string (); }
   _log.debug << endl;
}


void
dmz::ObjectPluginDump::remove_object_attribute (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &AttrMask) {

   _log.debug << "Removing Attributes[" << ObjectHandle << "]: "
      << _attr_name (AttributeHandle) << endl;
}


void
dmz::ObjectPluginDump::update_object_locality (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectLocalityEnum Locality,
      const ObjectLocalityEnum PrevLocality) {

}


void
dmz::ObjectPluginDump::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   _log.debug << "Linking object: " << SuperHandle << " to " << SubHandle
      << " for " << _attr_name (AttributeHandle)
      << " Link handle: " << LinkHandle << endl
      << "\t" << SuperIdentity << endl
      << "\t" << SubIdentity << endl;
}


void
dmz::ObjectPluginDump::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   _log.debug << "Unlinking object: " << SuperHandle << " from " << SubHandle
      << " for " << _attr_name (AttributeHandle)
      << " Link handle: " << LinkHandle << endl
      << "\t" << SuperIdentity << endl
      << "\t" << SubIdentity << endl;
}


void
dmz::ObjectPluginDump::update_link_attribute_object (
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

   _log.debug << "Link attribute object: " << AttributeObjectHandle << ":"
      << AttributeIdentity << endl
      << "was: " << PrevAttributeObjectHandle << ":" << PrevAttributeIdentity << endl
      << "In link: " << SuperHandle << " to " << SubHandle
      << " for " << _attr_name (AttributeHandle)
      << " Link handle: " << LinkHandle << endl
      << "\t" << SuperIdentity << endl
      << "\t" << SubIdentity << endl;
}


void
dmz::ObjectPluginDump::update_object_counter (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   String pstr ("<NULL>");
   if (PreviousValue) { pstr.flush () << *PreviousValue; }

   _log.debug << "Counter[" << ObjectHandle << "]:"
      << _attr_name (AttributeHandle)
      << ":" << Value << " previous: " << pstr << endl;
}


void
dmz::ObjectPluginDump::update_object_counter_minimum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   String pstr ("<NULL>");
   if (PreviousValue) { pstr.flush () << *PreviousValue; }

   _log.debug << "Counter Minimum[" << ObjectHandle << "]:"
      << _attr_name (AttributeHandle)
      << ":" << Value << " previous: " << pstr << endl;
}


void
dmz::ObjectPluginDump::update_object_counter_maximum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   String pstr ("<NULL>");
   if (PreviousValue) { pstr.flush () << *PreviousValue; }

   _log.debug << "Counter Maximum[" << ObjectHandle << "]:"
      << _attr_name (AttributeHandle)
      << ":" << Value << " previous: " << pstr << endl;
}


void
dmz::ObjectPluginDump::update_object_alternate_type (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const ObjectType &Value,
      const ObjectType *PreviousValue) {

   _log.debug << "Alternate ObjectType[" << ObjectHandle << "]:"
      << _attr_name (AttributeHandle)
      << ":" << Value.get_name () << " previous: "
      << (PreviousValue ? PreviousValue->get_name () : "<NULL>") << endl;
}


void
dmz::ObjectPluginDump::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   String name;
   _defs.lookup_state_name (Value, name);
   if (!name) { name = "<Empty>"; }

   String prevName ("<NULL>");
   if (PreviousValue) {

      _defs.lookup_state_name (*PreviousValue, prevName);
      if (!prevName) { prevName = "<Empty>"; }
   }

   _log.debug << "State[" << ObjectHandle << "]:" << _attr_name (AttributeHandle) << ": "
      << name << " previous: " << prevName << endl;
}


void
dmz::ObjectPluginDump::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   String prevName ("<NULL>");
   if (PreviousValue) { prevName.flush () << (*PreviousValue ? "True" : "False"); }
   _log.debug << "Flag[" << ObjectHandle << "]:" << _attr_name (AttributeHandle) << ": "
      << (Value ? "True" : "False") << " previous: " << prevName << endl;
}


void
dmz::ObjectPluginDump::update_object_time_stamp (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   String prevName ("<NULL>");
   if (PreviousValue) { prevName.flush () << *PreviousValue; }
   _log.debug << "TimeStamp[" << ObjectHandle << "]:" << _attr_name (AttributeHandle)
      << ": " << Value << " previous: " << prevName << endl;
}


void
dmz::ObjectPluginDump::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   String prevStr ("NULL"); if (PreviousValue) { prevStr.flush () << *PreviousValue; }
   _log.debug << "Position[" << ObjectHandle << "]:" << _attr_name (AttributeHandle)
      << ": " << Value << " previous: " << prevStr << endl;
}


void
dmz::ObjectPluginDump::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

   String prevStr ("NULL"); if (PreviousValue) { prevStr.flush () << *PreviousValue; }
   _log.debug << "Orientation[" << ObjectHandle << "]:" << _attr_name (AttributeHandle)
      << ": " << Value << " previous: " << prevStr << endl;
}


void
dmz::ObjectPluginDump::update_object_velocity (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   String prevStr ("NULL"); if (PreviousValue) { prevStr.flush () << *PreviousValue; }
   _log.debug << "Velocity[" << ObjectHandle << "]:" << _attr_name (AttributeHandle)
      << ": " << Value << " previous: " << prevStr << endl;
}


void
dmz::ObjectPluginDump::update_object_acceleration (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   String prevStr ("NULL"); if (PreviousValue) { prevStr.flush () << *PreviousValue; }
   _log.debug << "Acceleration[" << ObjectHandle << "]:" << _attr_name (AttributeHandle)
      << ": " << Value << " previous: " << prevStr << endl;
}


void
dmz::ObjectPluginDump::update_object_scale (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   String prevStr ("NULL"); if (PreviousValue) { prevStr.flush () << *PreviousValue; }
   _log.debug << "Scale[" << ObjectHandle << "]:" << _attr_name (AttributeHandle) << ": "
      << Value << " previous: " << prevStr << endl;
}


void
dmz::ObjectPluginDump::update_object_vector (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   String prevStr ("NULL"); if (PreviousValue) { prevStr.flush () << *PreviousValue; }
   _log.debug << "Vector[" << ObjectHandle << "]:" << _attr_name (AttributeHandle) << ": "
      << Value << " previous: " << prevStr << endl;
}


void
dmz::ObjectPluginDump::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   String prevStr ("NULL"); if (PreviousValue) { prevStr.flush () << *PreviousValue; }
   _log.debug << "Scalar[" << ObjectHandle << "]:" << _attr_name (AttributeHandle) << ": "
      << Value << " previous: " << prevStr << endl;
}


void
dmz::ObjectPluginDump::update_object_text (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const String &Value,
      const String *PreviousValue) {

   String prevStr ("NULL"); if (PreviousValue) { prevStr.flush () << *PreviousValue; }
   _log.debug << "Text[" << ObjectHandle << "]:" << _attr_name (AttributeHandle) << ": "
      << Value << " previous: " << prevStr << endl;
}


void
dmz::ObjectPluginDump::update_object_data (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Data &Value,
      const Data *PreviousValue) {

   _log.debug << "Data[" << ObjectHandle << "]:" << _attr_name (AttributeHandle) << ": "
      << Value << " previous: ";
   if (PreviousValue) { _log.debug << *PreviousValue; }
   else { _log.debug << "<NULL>"; }
   _log.debug << endl;
}


dmz::String
dmz::ObjectPluginDump::_attr_name (const Handle AttrHandle) {

   String result ("<UNKNOWN>");

   String name = _defs.lookup_named_handle_name (AttrHandle);

   if (name) { result = name; }

   return result;
}


void
dmz::ObjectPluginDump::_init (Config &local) {

}
//! \endcond

extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzObjectPluginDump (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ObjectPluginDump (Info, local);
}

};
