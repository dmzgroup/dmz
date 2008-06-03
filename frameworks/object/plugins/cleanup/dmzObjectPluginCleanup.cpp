#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include "dmzObjectPluginCleanup.h"
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::ObjectPluginCleanup
\ingroup Object
\brief Destroys all objects of a predefined type.
\details This Plugin destroys objects when it receives a predefined message. This
Plugin is useful for emptying the ObjectModule of objects of predefined ObjectTypes.
The ignore option allows sub object types to not be cleaned up when their parent type
is specified for cleanup.
The XML format:
\code
<dmz>
<dmzObjectPluginCleanup>
   <message name="CleanupObjectMessage"/>
   <!-- Object types to ignore. -->
   <ignore>
      <!--
         Multiple types may be defined.
      -->
      <type name="Object Type Name"/>
   </ignore>
   <!-- Object types to cleanup. -->
   <cleanup>
      <!--
         Multiple types may be defined.
      -->
      <type name="Object Type Name"/>
   </cleanup>
</dmzObjectPluginCleanup>
</dmz>
\endcode
\sa dmz::ObjectType dmz::ObjectModule
*/

//! \cond
dmz::ObjectPluginCleanup::ObjectPluginCleanup (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      MessageObserver (Info),
      ObjectObserverUtil (Info, local),
      _log (Info) {

   _init (local);
}


dmz::ObjectPluginCleanup::~ObjectPluginCleanup () {

}


// Plugin Interface
void
dmz::ObjectPluginCleanup::discover_plugin (const Plugin *PluginPtr) {

}


void
dmz::ObjectPluginCleanup::start_plugin () {

}


void
dmz::ObjectPluginCleanup::stop_plugin () {

}


void
dmz::ObjectPluginCleanup::shutdown_plugin () {

}


void
dmz::ObjectPluginCleanup::remove_plugin (const Plugin *PluginPtr) {

}


// Message Observer Interface
void
dmz::ObjectPluginCleanup::receive_message (
      const Message &Msg,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   if (Msg == _cleanupMessage) {

      ObjectModule *objMod (get_object_module ());

      if (objMod) {

         Handle object (_cleanupTable.get_first ());

         while (object) {

            objMod->destroy_object (object);

            object = _cleanupTable.get_next ();
         }
      }
   }
}


// Object Observer Interface
void
dmz::ObjectPluginCleanup::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (!_ignoreSet.contains_type (Type) && _cleanupSet.contains_type (Type)) {

      _cleanupTable.add_handle (ObjectHandle);
   }
}


void
dmz::ObjectPluginCleanup::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   _cleanupTable.remove_handle (ObjectHandle);
}


void
dmz::ObjectPluginCleanup::update_object_locality (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectLocalityEnum Locality,
      const ObjectLocalityEnum PrevLocality) {

}


void
dmz::ObjectPluginCleanup::update_object_type (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const ObjectType &Value,
      const ObjectType *PreviousValue) {

   if (!_ignoreSet.contains_type (Value) && _cleanupSet.contains_type (Value)) {

      _cleanupTable.add_handle (ObjectHandle);
   }
   else { _cleanupTable.remove_handle (ObjectHandle); }
}


void
dmz::ObjectPluginCleanup::_get_type_set (
      const String &Name,
      Config &config,
      ObjectTypeSet &set) {

   RuntimeContext *context (get_plugin_runtime_context ());

   Config typeList;

   if (config.lookup_all_config (Name + ".type", typeList)) {

      ConfigIterator it;
      Config type;

      while (typeList.get_next_config (it, type)) {

         set.add_object_type (config_to_string ("name", type), context);
      }
   }
}


void
dmz::ObjectPluginCleanup::_init (Config &local) {

   activate_default_object_attribute (
      ObjectCreateMask | ObjectDestroyMask | ObjectTypeMask);

   RuntimeContext *context (get_plugin_runtime_context ());

   _cleanupMessage = config_create_message_type (
      "message.name",
      local,
      "CleanupObjectsMessage",
      context);

   if (_cleanupMessage) { subscribe_to_message (_cleanupMessage); }
   else { _log.error << "No cleanup message specified" << endl; }

   _get_type_set ("ignore", local, _ignoreSet);
   _get_type_set ("cleanup", local, _cleanupSet);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzObjectPluginCleanup (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ObjectPluginCleanup (Info, local);
}

};
