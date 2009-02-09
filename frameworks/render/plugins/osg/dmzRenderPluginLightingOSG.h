#ifndef DMZ_RENDER_PLUGIN_LIGHTING_OSG_DOT_H
#define DMZ_RENDER_PLUGIN_LIGHTING_OSG_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableUInt32Template.h>
#include <osg/ref_ptr>

namespace osg { class Light; }

namespace dmz {

   class RenderModuleCoreOSG;

   class RenderPluginLightingOSG :
         public Plugin {

      public:
         RenderPluginLightingOSG (const PluginInfo &Info, Config &local);
         ~RenderPluginLightingOSG ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

      protected:
         struct LightStruct {

            osg::ref_ptr<osg::Light> light;
         };

         void _init (Config &local);
         void _init_light (const Int32 MaxLights, Config &light);
         void _add_lights ();

         Log _log;

         HashTableUInt32Template<LightStruct> _lightTable;

         RenderModuleCoreOSG *_core;

      private:
         RenderPluginLightingOSG ();
         RenderPluginLightingOSG (const RenderPluginLightingOSG &);
         RenderPluginLightingOSG &operator= (const RenderPluginLightingOSG &);

   };
};

#endif // DMZ_RENDER_PLUGIN_LIGHTING_OSG_DOT_H
