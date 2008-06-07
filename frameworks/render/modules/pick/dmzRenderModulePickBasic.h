#ifndef DMZ_RENDER_MODULE_PICK_BASIC_DOT_H
#define DMZ_RENDER_MODULE_PICK_BASIC_DOT_H

#include <dmzRenderModulePick.h>
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
         
         // RenderModule2dPick Interface
         virtual Boolean register_pick_2d (const Handle Source, RenderPick2d &pick);
         virtual Boolean release_pick_2d (const Handle Source, RenderPick2d &pick);

         virtual Boolean screen_to_world (
            const Handle Source,
            const Int32 ScreenPosX,
            const Int32 ScreenPosY,
            Vector &worldPosition,
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
            Handle &objectHandle);
         
         virtual Boolean world_to_source (
            const Handle Source,
            const Vector &WorldPosition,
            Int32 &screenPosX,
            Int32 &screenPosY);
         
      protected:
         void _init (Config &local);

         Log _log;

         HashTableHandleTemplate<RenderPick2d> _pick2dTable;
         //! \endcond

      private:
         RenderModulePickBasic ();
         RenderModulePickBasic (const RenderModulePickBasic &);
         RenderModulePickBasic &operator= (const RenderModulePickBasic &);

   };
};

#endif // DMZ_RENDER_MODULE_PICK_BASIC_DOT_H
