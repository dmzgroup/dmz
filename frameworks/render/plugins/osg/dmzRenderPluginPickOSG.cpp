#include <dmzRenderConsts.h>
#include <dmzRenderModuleCoreOSG.h>
#include <dmzRenderObjectDataOSG.h>
#include "dmzRenderPluginPickOSG.h"
#include <dmzRenderUtilOSG.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <osgUtil/IntersectVisitor>

dmz::RenderPluginPickOSG::RenderPluginPickOSG (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      RenderPickUtil (Info, local),
      _log (Info),
      _core (0),
      _viewerName (RenderMainPortalName) {

   _init (local);
}


dmz::RenderPluginPickOSG::~RenderPluginPickOSG () {

}


// Plugin Interface
void
dmz::RenderPluginPickOSG::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

      if (!_viewer.valid () && _core) {

         _viewer = _core->lookup_viewer (_viewerName);
         if (_viewer.valid ()) {

            osg::Camera *camera = _viewer->getCamera ();

            if (camera) { _viewport = camera->getViewport (); }
         }
      }
   }
   else if (State == PluginStateStop) {

      if (_viewer.valid ()) { _viewer = 0; _viewport = 0; }
   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::RenderPluginPickOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) { _core = dmz::RenderModuleCoreOSG::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == dmz::RenderModuleCoreOSG::cast (PluginPtr))) { _core = 0; }
   }
}


// RenderPickUtil Interface
dmz::Boolean
dmz::RenderPluginPickOSG::screen_to_world (
      const Int32 ScreenPosX,
      const Int32 ScreenPosY,
      Vector &worldPosition,
      Handle &objectHandle) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderPluginPickOSG::world_to_screen (
      const Vector &WorldPosition,
      Int32 &screenPosX,
      Int32 &screenPosY) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderPluginPickOSG::source_to_world (
      const Int32 SourcePosX,
      const Int32 SourcePosY,
      Vector &worldPosition,
      Handle &objectHandle) {

   Boolean result (False);

   if (_core && _viewer.valid () && _viewport.valid ()) {

      const float Height = _viewport->height ();

      osgUtil::LineSegmentIntersector::Intersections isect;

      if (_viewer->computeIntersections (
            (float)SourcePosX,
            Height - (float)SourcePosY,
            isect)) {

         osgUtil::LineSegmentIntersector::Intersections::iterator it = isect.begin ();

         while (!result && (it != isect.end ())) {

            Boolean disabled (False);

            osg::NodePath path = it->nodePath;

            for (osg::NodePath::iterator node = path.begin ();
                  (node != path.end ()) && !disabled;
                  node++) {

               if (*node) {

                  osg::Referenced *ref ((*node)->getUserData ());

                  if (ref) {

                     RenderObjectDataOSG *data (
                        dynamic_cast<RenderObjectDataOSG *> (ref));

                     if (data) {

                        if (!data->do_isect ()) { disabled = True; }
                        else { objectHandle = data->get_handle (); }
                     }
                  }
               }
            }

            if (!disabled) {

               worldPosition = to_dmz_vector (it->getWorldIntersectPoint ());
               result = True;
            }
            else { it++; }
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::RenderPluginPickOSG::world_to_source (
      const Vector &WorldPosition,
      Int32 &sourcePosX,
      Int32 &sourcePosY) {

   Boolean result (False);

   return result;
}


void
dmz::RenderPluginPickOSG::_init (Config &local) {

   _viewerName = config_to_string ("portal.name", local, _viewerName);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginPickOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginPickOSG (Info, local);
}

};
