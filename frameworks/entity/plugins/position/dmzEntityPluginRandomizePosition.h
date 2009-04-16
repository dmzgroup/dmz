#ifndef DMZ_ENTITY_PLUGIN_RANDOMIZE_POSITION_DOT_H
#define DMZ_ENTITY_PLUGIN_RANDOMIZE_POSITION_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesVector.h>

namespace dmz {

   class RenderModuleIsect;

   class EntityPluginRandomizePosition :
         public Plugin,
         public ObjectObserverUtil {

      public:
         EntityPluginRandomizePosition (const PluginInfo &Info, Config &local);
         ~EntityPluginRandomizePosition ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Object Observer Interface
         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

      protected:
         void _init (Config &local);

         Log _log;

         RenderModuleIsect *_isect;

         Handle _defaultAttrHandle;
         Vector _center;
         Float64 _radius;
         Float64 _heading;

      private:
         EntityPluginRandomizePosition ();
         EntityPluginRandomizePosition (const EntityPluginRandomizePosition &);
         EntityPluginRandomizePosition &operator= (const EntityPluginRandomizePosition &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_RANDOMIZE_POSITION_DOT_H
