#ifndef DMZ_RENDER_MODULE_ISECT_OSG_DOT_H
#define DMZ_RENDER_MODULE_ISECT_OSG_DOT_H

#include <dmzRenderModuleCoreOSG.h>
#include <dmzRuntimeLog.h>
#include <dmzRenderModuleIsect.h>
#include <dmzRuntimePlugin.h>
#include <dmzRenderIsect.h>

namespace dmz {

/*   class IsectParameters;
   class IsectResult;
   class IsectTestContainer;
   class Isect;
   class Vector;*/

   class RenderModuleIsectOSG : public Plugin, private RenderModuleIsect {

      public:
         RenderModuleIsectOSG (const PluginInfo &Info, const Config &Local);
         ~RenderModuleIsectOSG ();

         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         virtual Boolean do_isect (
            const IsectParameters &Parameters,
            const IsectTestContainer &TestValues,
            IsectResultContainer &resultContainer);

         virtual UInt32 enable_isect (const Handle ObjectHandle);
         virtual UInt32 disable_isect (const Handle ObjectHandle);

      protected:
         void _init (const Config &Local);

         RenderModuleCoreOSG *_core;
         Log _log;
   };
};

#endif //  DMZ_RENDER_MODULE_ISECT_OSG_DOT_H
