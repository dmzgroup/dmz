#ifndef DMZ_RENDER_PLUGIN_LOAD_MODEL_OSG_DOT_H
#define DMZ_RENDER_PLUGIN_LOAD_MODEL_OSG_DOT_H

#include <dmzRenderModuleCoreOSG.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableStringTemplate.h>

#include <osg/Node>

namespace dmz {

   class Vector;

   class RenderPluginLoadModelOSG : public Plugin {

      public:
         RenderPluginLoadModelOSG (const PluginInfo &Info, const Config &Local);
         ~RenderPluginLoadModelOSG ();

         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

      protected:
         void _init (const Config &Local);

         RenderModuleCoreOSG * _core;
         HashTableStringTemplate<String> _fileNameTable;
         Log _log;
   };
};

#endif //  DMZ_RENDER_PLUGIN_LOAD_MODEL_OSG_DOT_H
