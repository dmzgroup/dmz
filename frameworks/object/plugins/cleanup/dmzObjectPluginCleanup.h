#ifndef DMZ_OBJECT_PLUGIN_CLEANUP_DOT_H
#define DMZ_OBJECT_PLUGIN_CLEANUP_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHandleContainer.h>

namespace dmz {

   class ObjectPluginCleanup :
         public Plugin,
         public MessageObserver,
         public ObjectObserverUtil {

      public:
         //! \cond
         ObjectPluginCleanup (const PluginInfo &Info, Config &local);
         ~ObjectPluginCleanup ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // Message Observer Interface
         void receive_message (
            const MessageType &Msg,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_locality (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectLocalityEnum Locality,
            const ObjectLocalityEnum PrevLocality);

         virtual void update_object_type (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const ObjectType &Value,
            const ObjectType *PreviousValue);

      protected:
         void _get_type_set (const String &Name, Config &config, ObjectTypeSet &set);
         void _init (Config &local);

         MessageType _cleanupMessage;
         HandleContainer _cleanupTable;
         ObjectTypeSet _ignoreSet;
         ObjectTypeSet _cleanupSet;

         Log _log;
         //! \endcond

      private:
         ObjectPluginCleanup ();
         ObjectPluginCleanup (const ObjectPluginCleanup &);
         ObjectPluginCleanup &operator= (const ObjectPluginCleanup &);
   };
};

#endif // DMZ_OBJECT_PLUGIN_CLEANUP_DOT_H
