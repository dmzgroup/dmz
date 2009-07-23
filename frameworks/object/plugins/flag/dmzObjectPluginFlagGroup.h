#ifndef DMZ_OBJECT_PLUGIN_FLAG_GROUP_DOT_H
#define DMZ_OBJECT_PLUGIN_FLAG_GROUP_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeUndo.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class ObjectPluginFlagGroup :
         public Plugin,
         public ObjectObserverUtil {

      public:
         ObjectPluginFlagGroup (const PluginInfo &Info, Config &local);
         ~ObjectPluginFlagGroup ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // Object Observer Interface
         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void remove_object_attribute (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &AttrMask);

         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

      protected:
         void _init (Config &local);

         Log _log;
         Undo _undo;

         Int32 _maxGroupSize;
         Handle _flagAttrHandle;
         HashTableHandleTemplate<Int32> _flagGroup;

      private:
         ObjectPluginFlagGroup ();
         ObjectPluginFlagGroup (const ObjectPluginFlagGroup &);
         ObjectPluginFlagGroup &operator= (const ObjectPluginFlagGroup &);

   };
};

#endif // DMZ_OBJECT_PLUGIN_FLAG_GROUP_DOT_H
