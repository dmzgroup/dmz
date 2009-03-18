#ifndef DMZ_RENDER_PLUGIN_LOGO_OSG_DOT_H
#define DMZ_RENDER_PLUGIN_LOGO_OSG_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeResources.h>
#include <dmzRuntimeTimeSlice.h>

#include <osg/Camera>

namespace dmz {

   class RenderModuleCoreOSG;

   class RenderPluginLogoOSG : public Plugin, public TimeSlice {

      public:
         RenderPluginLogoOSG (const PluginInfo &Info, Config &local);
         ~RenderPluginLogoOSG ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

      protected:
         void _create_logo ();
         void _init (Config &local);

         Log _log;
         Resources _rc;

         String _imageResource;

         osg::ref_ptr<osg::Camera> _camera;
         osg::ref_ptr<osg::Camera> _masterCamera;
         RenderModuleCoreOSG *_core;

      private:
         RenderPluginLogoOSG ();
         RenderPluginLogoOSG (const RenderPluginLogoOSG &);
         RenderPluginLogoOSG &operator= (const RenderPluginLogoOSG &);

   };
};

#endif // DMZ_RENDER_PLUGIN_LOGO_OSG_DOT_H
