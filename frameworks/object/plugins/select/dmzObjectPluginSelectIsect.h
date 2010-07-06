#ifndef DMZ_OBJECT_PLUGIN_SELECT_ISECT_DOT_H
#define DMZ_OBJECT_PLUGIN_SELECT_ISECT_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class ObjectModuleSelect;
   class RenderModuleIsect;

   class ObjectPluginSelectIsect :
         public Plugin,
         public InputObserverUtil,
         public ObjectObserverUtil {

      public:
         ObjectPluginSelectIsect (const PluginInfo &Info, Config &local);
         ~ObjectPluginSelectIsect ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State);

         // Object Observer Interface
         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

      protected:
         // ObjectPluginSelectIsect Interface
         void _init (Config &local);

         Log _log;

         ObjectModuleSelect *_select;
         RenderModuleIsect *_isect;

         Int32 _active;

      private:
         ObjectPluginSelectIsect ();
         ObjectPluginSelectIsect (const ObjectPluginSelectIsect &);
         ObjectPluginSelectIsect &operator= (const ObjectPluginSelectIsect &);

   };
};

#endif // DMZ_OBJECT_PLUGIN_SELECT_ISECT_DOT_H
