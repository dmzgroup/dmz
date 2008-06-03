#include "dmzExPluginObject.h"
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesVector.h>
#include <dmzTypesUUID.h>


dmz::ExPluginObject::ExPluginObject (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defs (Info, &_log),
      _defaultAttributeHandle (0),
      _link1AttributeHandle (0),
      _link2AttributeHandle (0),
      _flagAttributeHandle (0) {

   _init (local);
}


dmz::ExPluginObject::~ExPluginObject () {

}


// Plugin Interface
void
dmz::ExPluginObject::discover_plugin (const Plugin *PluginPtr) {

}


void
dmz::ExPluginObject::start_plugin () {

}


void
dmz::ExPluginObject::stop_plugin () {

}


void
dmz::ExPluginObject::shutdown_plugin () {

}


void
dmz::ExPluginObject::remove_plugin (const Plugin *PluginPtr) {

}


// Object Observer Interface
void
dmz::ExPluginObject::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   _log.out << "[create_object] handle: " << ObjectHandle
            << " type: " << Type.get_name ()
            << " uuid: " << Identity.to_string () << endl;
}


void
dmz::ExPluginObject::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   _log.out << "[destroy_object] handle: " << ObjectHandle
            << " uuid: " << Identity.to_string () << endl;
}


void
dmz::ExPluginObject::store_object_uuid (
      const Handle ObjectHandle,
      const UUID &Identity,
      const UUID &PrevIdentity) {

   _log.out << "[store_object_uuid] handle: " << ObjectHandle
            << " uuid: " << Identity.to_string () << endl;
}


void
dmz::ExPluginObject::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   String attributeName (_defs.lookup_named_handle_name (AttributeHandle));
   
   if (AttributeHandle == _link1AttributeHandle ||
       AttributeHandle == _link2AttributeHandle) {
      
      _log.out << "[link_objects] super: " << SuperHandle << " sub: " << SubHandle
         << " for attribute '" << attributeName << "'" << endl;
   }
}


void
dmz::ExPluginObject::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   String attributeName (_defs.lookup_named_handle_name (AttributeHandle));

   if (AttributeHandle == _link1AttributeHandle ||
       AttributeHandle == _link2AttributeHandle) {

      _log.out << "[unlink_objects] super: " << SuperHandle << " sub: " << SubHandle
         << " for attribute '" << attributeName << "'" << endl;
   }
}


void
dmz::ExPluginObject::store_object_type (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const ObjectType &Value,
      const ObjectType *PreviousValue) {

   if (AttributeHandle == _defaultAttributeHandle) {
      
      _log.out << "[store_object_type] handle: " << ObjectHandle 
               << " type: " << Value.get_name () << endl;
   }
}


void
dmz::ExPluginObject::store_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   String attributeName (_defs.lookup_named_handle_name (AttributeHandle));
   
   if (AttributeHandle == _flagAttributeHandle) {

      _log.out << "[store_object_flag] handle: " << ObjectHandle 
               << " flag: " << (Value ? "True" : "False")
               << " for attribute '" << attributeName << "'" << endl;
   }
}


void
dmz::ExPluginObject::store_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (AttributeHandle == _defaultAttributeHandle) {

      _log.out << "[store_object_position] handle: " << ObjectHandle 
               << " pos: " << Value << endl;
   }
}


void
dmz::ExPluginObject::_init (Config &local) {

   _defaultAttributeHandle = activate_default_object_attribute (
      ObjectCreateMask |
      ObjectDestroyMask |
      ObjectUUIDMask |
      ObjectTypeMask |
      ObjectPositionMask);
   
   _link1AttributeHandle = activate_object_attribute (
      "Ex_Link1",
      ObjectLinkMask | ObjectUnlinkMask);
      
   _link2AttributeHandle = activate_object_attribute (
      "Ex_Link2",
      ObjectLinkMask | ObjectUnlinkMask);
   
   _flagAttributeHandle = activate_object_attribute (
      "Ex_Flag_Visible",
      ObjectFlagMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzExPluginObject (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ExPluginObject (Info, local);
}

};
