#ifndef DMZ_OBJECT_PLUGIN_HIGHLIGHT_DOT_H
#define DMZ_OBJECT_PLUGIN_HIGHLIGHT_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeDataConverterTypesBase.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHandleContainer.h>

namespace dmz {

   class ObjectPluginHighlight :
         public Plugin,
         public MessageObserver,
         public ObjectObserverUtil {

      public:
         ObjectPluginHighlight (const PluginInfo &Info, Config &local);
         ~ObjectPluginHighlight ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Message Observer Interface
         virtual void receive_message (
            const Message &Type,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

         // Object Observer Interface
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

         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

      protected:
         // ObjectPluginHighlight Interface
         void _init (Config &local);

         Log _log;
         DataConverterHandle _convert;

         Handle _current;
         Handle _highlightAttr;

         HandleContainer _attrObjects;
         ObjectType _attrObjectType;

         Message _mouseMoveMsg;

      private:
         ObjectPluginHighlight ();
         ObjectPluginHighlight (const ObjectPluginHighlight &);
         ObjectPluginHighlight &operator= (const ObjectPluginHighlight &);

   };
};

#endif // DMZ_OBJECT_PLUGIN_HIGHLIGHT_DOT_H
