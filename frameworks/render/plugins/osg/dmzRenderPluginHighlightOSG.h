#ifndef DMZ_RENDER_PLUGIN_HIGHLIGHT_OSG_DOT_H
#define DMZ_RENDER_PLUGIN_HIGHLIGHT_OSG_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

#include <osg/StateSet>

namespace dmz {

   class RenderModuleCoreOSG;

   class RenderPluginHighlightOSG :
         public Plugin,
         public ObjectObserverUtil {

      public:
         RenderPluginHighlightOSG (const PluginInfo &Info, Config &local);
         ~RenderPluginHighlightOSG ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Object Observer Interface
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

         RenderModuleCoreOSG *_core;

         osg::ref_ptr<osg::StateSet> _yellow;

      private:
         RenderPluginHighlightOSG ();
         RenderPluginHighlightOSG (const RenderPluginHighlightOSG &);
         RenderPluginHighlightOSG &operator= (const RenderPluginHighlightOSG &);

   };
};

#endif // DMZ_RENDER_PLUGIN_HIGHLIGHT_OSG_DOT_H
