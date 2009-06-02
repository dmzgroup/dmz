#ifndef DMZ_RENDER_PLUGIN_STATIC_TERRAIN_OSG_DOT_H
#define DMZ_RENDER_PLUGIN_STATIC_TERRAIN_OSG_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeResources.h>

#include <osg/Node>

namespace dmz {

   class RenderModuleCoreOSG;

   class RenderPluginStaticTerrainOSG :
         public Plugin {

      public:
         RenderPluginStaticTerrainOSG (const PluginInfo &Info, Config &local);
         ~RenderPluginStaticTerrainOSG ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

      protected:
         struct ModelStruct {

            const Boolean Isect;
            ModelStruct *next;
            osg::ref_ptr<osg::Node> model;

            ModelStruct (const Boolean IsIsect) : Isect (IsIsect), next (0) {;}
            ~ModelStruct () { if (next) { delete next; next = 0; } }
         };

         void _init (Config &local);

         Log _log;
         Resources _rc;

         RenderModuleCoreOSG *_core;

         ModelStruct *_modelList;

      private:
         RenderPluginStaticTerrainOSG ();
         RenderPluginStaticTerrainOSG (const RenderPluginStaticTerrainOSG &);
         RenderPluginStaticTerrainOSG &operator= (const RenderPluginStaticTerrainOSG &);

   };
};

#endif // DMZ_RENDER_PLUGIN_STATIC_TERRAIN_OSG_DOT_H
