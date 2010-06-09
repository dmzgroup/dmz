#ifndef DMZ_RENDER_PLUGIN_HEIGHT_MAP_OSG_DOT_H
#define DMZ_RENDER_PLUGIN_HEIGHT_MAP_OSG_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimeResources.h>
#include <dmzRuntimePlugin.h>

#include <osg/Geode>

namespace dmz {

   class RenderPluginHeightMapOSG :
         public Plugin {

      public:
         RenderPluginHeightMapOSG (const PluginInfo &Info, Config &local);
         ~RenderPluginHeightMapOSG ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

      protected:
         // RenderPluginHeightMapOSG Interface
         void _init_height_map (Config &local);
         void _init (Config &local);

         Log _log;
         Resources _rc;

         osg::ref_ptr<osg::Group> _terrain;

      private:
         RenderPluginHeightMapOSG ();
         RenderPluginHeightMapOSG (const RenderPluginHeightMapOSG &);
         RenderPluginHeightMapOSG &operator= (const RenderPluginHeightMapOSG &);

   };
};

#endif // DMZ_RENDER_PLUGIN_HEIGHT_MAP_OSG_DOT_H
