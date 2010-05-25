#ifndef DMZ_RENDER_PLUGIN_PICK_OSG_DOT_H
#define DMZ_RENDER_PLUGIN_PICK_OSG_DOT_H

#include <dmzRenderPickUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

#include <osg/Viewport>
#include <osgViewer/View>

namespace dmz {

   class RenderModuleCoreOSG;

   class RenderPluginPickOSG :
         public Plugin,
         public RenderPickUtil {

      public:
         RenderPluginPickOSG (const PluginInfo &Info, Config &local);
         ~RenderPluginPickOSG ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // RenderPickUtil Interface
         virtual Boolean screen_to_world (
            const Int32 ScreenPosX,
            const Int32 ScreenPosY,
            Vector &worldPosition,
            Vector &normal,
            Handle &objectHandle);

         virtual Boolean world_to_screen (
            const Vector &WorldPosition,
            Int32 &screenPosX,
            Int32 &screenPosY);

         virtual Boolean source_to_world (
            const Int32 SourcePosX,
            const Int32 SourcePosY,
            Vector &worldPosition,
            Vector &normal,
            Handle &objectHandle);

         virtual Boolean world_to_source (
            const Vector &WorldPosition,
            Int32 &sourcePosX,
            Int32 &sourcePosY);

      protected:
         void _init (Config &local);

         Log _log;
         UInt32 _isectMask;
         RenderModuleCoreOSG *_core;
         osg::ref_ptr<osgViewer::View> _view;
         osg::ref_ptr<osg::Viewport> _viewport;
         String _viewName;

      private:
         RenderPluginPickOSG ();
         RenderPluginPickOSG (const RenderPluginPickOSG &);
         RenderPluginPickOSG &operator= (const RenderPluginPickOSG &);

   };
};

#endif // DMZ_RENDER_PLUGIN_PICK_OSG_DOT_H
