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

         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

      protected:
         void _init (const Config &Local);

         RenderModuleCoreOSG * _core;
         HashTableStringTemplate<String> _fileNameTable;
         HashTableStringTemplate<Vector> _positionTable;
         HashTableStringTemplate<Matrix> _rotationTable;
         Log _log;
   };
};

#endif //  DMZ_RENDER_PLUGIN_LOAD_MODEL_OSG_DOT_H
