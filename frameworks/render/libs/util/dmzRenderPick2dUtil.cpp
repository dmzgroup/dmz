#include <dmzRenderModulePick.h>
#include <dmzRenderPick2dUtil.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>


struct dmz::RenderPick2dUtil::State {

   const String RenderModulePickName;
   const String SourceName;
   Log log;
   Definitions defs;
   RenderModulePick *module;

   State (const PluginInfo &Info, const Config &Init) :
         RenderModulePickName (config_to_string ("module.pick.name", Init)),
         SourceName (config_to_string ("source.name", Init)),
         log (Info.get_name () + ".RenderPick2dUtil", Info.get_context ()),
         defs (Info, &log),
         module (0) {

      if (RenderModulePickName) {

         log.info << "Looking for Render Module Pick: " << RenderModulePickName << endl;
      }

      if (SourceName) {

         log.info << "Looking for Pick Source: " << SourceName
            << "[" << defs.lookup_named_handle (SourceName) << "]" << endl;
      }
   }

   ~State () {;}
};


dmz::RenderPick2dUtil::RenderPick2dUtil (
      const PluginInfo &Info,
      const Config &Init) :
      RenderPick2d (Info),
      __state (*(new State (Info, Init))) {;}


dmz::RenderPick2dUtil::~RenderPick2dUtil () {

   if (__state.module) { remove_render_module_pick ("", *(__state.module)); }
   delete &__state;
}


dmz::RenderModulePick *
dmz::RenderPick2dUtil::get_render_module_pick () { return __state.module; }


// RenderPick2d Interface.
void
dmz::RenderPick2dUtil::store_render_module_pick (
      const String &Name,
      RenderModulePick &module) {

   if (!__state.RenderModulePickName || (Name == __state.RenderModulePickName)) {

      if (!__state.module) {

         __state.module = &module;

         __state.module->register_pick_2d (
            __state.defs.lookup_named_handle (__state.SourceName),
            *this);

         _store_render_module_pick (module);
      }
   }
}


void
dmz::RenderPick2dUtil::remove_render_module_pick (
      const String &Name,
      RenderModulePick &module) {

   if (__state.module == &module) {

      _remove_render_module_pick (module);

      __state.module->release_pick_2d (
         __state.defs.lookup_named_handle (__state.SourceName),
         *this);

      __state.module = 0;
   }
}


dmz::Boolean
dmz::RenderPick2dUtil::screen_to_world (
      const Int32 ScreenPosX,
      const Int32 ScreenPosY,
      Vector &worldPosition,
      Handle &objectHandle) {

   __state.log.warn << "Base screen_to_world called."
      << " Function should have been overridden?" << endl;

   return False;
}


dmz::Boolean
dmz::RenderPick2dUtil::world_to_screen (
      const Vector &WorldPosition,
      Int32 &screenPosX,
      Int32 &screenPosY) {

   __state.log.warn << "Base world_to_screen called."
      << " Function should have been overridden?" << endl;

   return False;
}


dmz::Boolean
dmz::RenderPick2dUtil::source_to_world (
      const Int32 SourcePosX,
      const Int32 SourcePosY,
      Vector &worldPosition,
      Handle &objectHandle) {

   __state.log.warn << "Base source_to_world called."
      << " Function should have been overridden?" << endl;

   return False;
}


dmz::Boolean
dmz::RenderPick2dUtil::world_to_source (
      const Vector &WorldPosition,
      Int32 &sourcePosX,
      Int32 &sourcePosY) {

   __state.log.warn << "Base world_to_source called."
      << " Function should have been overridden?" << endl;

   return False;
}

