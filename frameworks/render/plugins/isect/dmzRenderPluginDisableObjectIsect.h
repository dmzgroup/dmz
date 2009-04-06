#ifndef DMZ_RENDER_PLUGIN_DISABLE_OBJECT_ISECT_DOT_H
#define DMZ_RENDER_PLUGIN_DISABLE_OBJECT_ISECT_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class RenderModuleIsect;

   class RenderPluginDisableObjectIsect :
         public Plugin,
         public ObjectObserverUtil {

      public:
         RenderPluginDisableObjectIsect (const PluginInfo &Info, Config &local);
         ~RenderPluginDisableObjectIsect ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

      protected:
         void _init (Config &local);

         Log _log;

         RenderModuleIsect *_isect;

         HashTableHandleTemplate<Boolean> _isectTable;

      private:
         RenderPluginDisableObjectIsect ();
         RenderPluginDisableObjectIsect (const RenderPluginDisableObjectIsect &);
         RenderPluginDisableObjectIsect &operator= (const RenderPluginDisableObjectIsect &);

   };
};

#endif // DMZ_RENDER_PLUGIN_DISABLE_OBJECT_ISECT_DOT_H
