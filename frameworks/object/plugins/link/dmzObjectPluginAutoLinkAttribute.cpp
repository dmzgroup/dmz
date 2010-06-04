#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include "dmzObjectPluginAutoLinkAttribute.h"
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::ObjectPluginAutoLinkAttribute
\ingroup Object
\brief Automatically creates an attribute object when two objects are linked.
\details
The ObjectType of the link attribute object is determined by attribute handle
used to create the link.
\code
<dmzObjectPluginAutoLinkAttribute>
   <attribute name="Attribute Name" type="Object Type Name"/>
   <attribute name="Attribute Name" type="Object Type Name"/>
   ...
</dmzObjectPluginAutoLinkAttribute>
\endcode

*/

//! \cond
dmz::ObjectPluginAutoLinkAttribute::ObjectPluginAutoLinkAttribute (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info) {

   _init (local);
}


dmz::ObjectPluginAutoLinkAttribute::~ObjectPluginAutoLinkAttribute () {

   _typeTable.empty ();
}


// Plugin Interface
void
dmz::ObjectPluginAutoLinkAttribute::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::ObjectPluginAutoLinkAttribute::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Object Observer Interface
void
dmz::ObjectPluginAutoLinkAttribute::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   ObjectType *type = _typeTable.lookup (AttributeHandle);

   if (type) {

      ObjectModule *module (get_object_module ());

      if (module) {

         const Handle Object = module->create_object (*type, ObjectLocal);

         if (Object) {

            module->activate_object (Object);
            module->store_link_attribute_object (LinkHandle, Object);
         }
      }
   }
}


void
dmz::ObjectPluginAutoLinkAttribute::update_link_attribute_object (
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

   if (_attrObjects.remove_handle (PrevAttributeObjectHandle)) {

      ObjectModule *module (get_object_module ());

      if (module) { module->destroy_object (PrevAttributeObjectHandle); }
   }
}


// ObjectPluginAutoLinkAttribute Interface
void
dmz::ObjectPluginAutoLinkAttribute::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   Config list;

   if (local.lookup_all_config ("attribute", list)) {

      ConfigIterator it;
      Config attr;

      while (list.get_next_config (it, attr)) {

         const Handle AttrHandle = config_to_named_handle ("name", attr, context);
         const ObjectType Type = config_to_object_type ("type", attr, context);

         if (AttrHandle && Type) {

            ObjectType *ptr = new ObjectType (Type);

            if (ptr && _typeTable.store (AttrHandle, ptr)) {

               activate_object_attribute (
                  AttrHandle,
                  ObjectLinkMask | ObjectLinkAttributeMask);
            }
            else if (ptr) { delete ptr; ptr = 0; }
         }
      }
   }
}
//! \endcond

extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzObjectPluginAutoLinkAttribute (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ObjectPluginAutoLinkAttribute (Info, local);
}

};
