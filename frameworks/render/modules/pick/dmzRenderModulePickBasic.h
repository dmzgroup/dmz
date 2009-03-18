#ifndef DMZ_RENDER_MODULE_PICK_BASIC_DOT_H
#define DMZ_RENDER_MODULE_PICK_BASIC_DOT_H

#include <dmzRenderModulePick.h>
#include <dmzRenderPick.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {


   class RenderModulePickBasic :
      public Plugin,
      private RenderModulePick {

      //! \cond
      public:
         RenderModulePickBasic (const PluginInfo &Info, Config &local);
         ~RenderModulePickBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // RenderModulePick Interface
         virtual Boolean register_pick (const Handle Source, RenderPick &pick);
         virtual Boolean release_pick (const Handle Source, RenderPick &pick);

         virtual Boolean screen_to_world (
            const Handle Source,
            const Int32 ScreenPosX,
            const Int32 ScreenPosY,
            Vector &worldPosition,
            Vector &normal,
            Handle &objectHandle);

         virtual Boolean world_to_screen (
            const Handle Source,
            const Vector &WorldPosition,
            Int32 &screenPosX,
            Int32 &screenPosY);

         virtual Boolean source_to_world (
            const Handle Source,
            const Int32 ScreenPosX,
            const Int32 ScreenPosY,
            Vector &worldPosition,
            Vector &normal,
            Handle &objectHandle);

         virtual Boolean world_to_source (
            const Handle Source,
            const Vector &WorldPosition,
            Int32 &screenPosX,
            Int32 &screenPosY);

      protected:
         void _init (Config &local);

         Log _log;

         HashTableHandleTemplate<RenderPick> _pickTable;
         //! \endcond

      private:
         RenderModulePickBasic ();
         RenderModulePickBasic (const RenderModulePickBasic &);
         RenderModulePickBasic &operator= (const RenderModulePickBasic &);

   };
};

#endif // DMZ_RENDER_MODULE_PICK_BASIC_DOT_H
