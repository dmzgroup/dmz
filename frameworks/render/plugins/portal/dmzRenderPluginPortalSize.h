#ifndef DMZ_RENDER_PLUGIN_PORTAL_SIZE_DOT_H
#define DMZ_RENDER_PLUGIN_PORTAL_SIZE_DOT_H

#include <dmzRenderPortalSize.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   //! \cond
   class RenderPluginPortalSize :
         public Plugin,
         public PortalSizeObserver {

      public:
         RenderPluginPortalSize (const PluginInfo &Info, Config &local);
         ~RenderPluginPortalSize ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         void update_portal_size (
            const Handle WindowHandle,
            const Int32 SizeX,
            const Int32 SizeY);

      protected:
         void _init (Config &local);

         Log _log;

      private:
         RenderPluginPortalSize ();
         RenderPluginPortalSize (const RenderPluginPortalSize &);
         RenderPluginPortalSize &operator= (const RenderPluginPortalSize &);

   };
   //! \endcond
};

#endif // DMZ_RENDER_PLUGIN_PORTAL_SIZE_DOT_H
