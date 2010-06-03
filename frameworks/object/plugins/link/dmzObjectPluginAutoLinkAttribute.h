#ifndef DMZ_OBJECT_PLUGIN_AUTO_LINK_ATTRIBUTE_DOT_H
#define DMZ_OBJECT_PLUGIN_AUTO_LINK_ATTRIBUTE_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class ObjectPluginAutoLinkAttribute :
         public Plugin,
         public ObjectObserverUtil {

      public:
         ObjectPluginAutoLinkAttribute (const PluginInfo &Info, Config &local);
         ~ObjectPluginAutoLinkAttribute ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Object Observer Interface
         virtual void link_objects (
            const Handle LinkHandle,
            const Handle AttributeHandle,
            const UUID &SuperIdentity,
            const Handle SuperHandle,
            const UUID &SubIdentity,
            const Handle SubHandle);

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
            const Handle PrevAttributeObjectHandle);

      protected:
         // ObjectPluginAutoLinkAttribute Interface
         void _init (Config &local);

         HandleContainer _attrObjects;
         HashTableHandleTemplate<ObjectType> _typeTable;

         Log _log;

      private:
         ObjectPluginAutoLinkAttribute ();
         ObjectPluginAutoLinkAttribute (const ObjectPluginAutoLinkAttribute &);
         ObjectPluginAutoLinkAttribute &operator= (const ObjectPluginAutoLinkAttribute &);

   };
};

#endif // DMZ_OBJECT_PLUGIN_AUTO_LINK_ATTRIBUTE_DOT_H
