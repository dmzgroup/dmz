#ifndef DMZ_RENDER_MODULE_ISECT_OSG_DOT_H
#define DMZ_RENDER_MODULE_ISECT_OSG_DOT_H

#include <dmzRenderModuleCoreOSG.h>
#include <dmzRuntimeLog.h>
#include <dmzRenderModuleIsect.h>
#include <dmzRuntimePlugin.h>
#include <dmzRenderIsect.h>

namespace dmz {

   class RenderModuleIsectOSG : public Plugin, private RenderModuleIsect {

      public:
         RenderModuleIsectOSG (const PluginInfo &Info, const Config &Local);
         ~RenderModuleIsectOSG ();

         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         virtual Boolean do_isect (
            const IsectParameters &Parameters,
            const IsectTestContainer &TestValues,
            IsectResultContainer &resultContainer);

         virtual UInt32 enable_isect (const Handle ObjectHandle);
         virtual UInt32 disable_isect (const Handle ObjectHandle);

      protected:
         void _init (const Config &Local);
       
         Log _log;

         RenderModuleCoreOSG *_core;

         UInt32 _isectMask;
   };
};

#endif //  DMZ_RENDER_MODULE_ISECT_OSG_DOT_H
