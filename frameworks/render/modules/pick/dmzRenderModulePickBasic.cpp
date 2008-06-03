#include "dmzRenderModulePickBasic.h"
#include <dmzRenderPick2d.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>


dmz::RenderModulePickBasic::RenderModulePickBasic (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      RenderModulePick (Info),
      _log (Info),
      _pick2dTable () {
   
   _init (local);
}


dmz::RenderModulePickBasic::~RenderModulePickBasic () {

   HashTableHandleIterator it;

   RuntimeContext *context (_PluginInfoData.get_context ());

   RenderPick2d *pick2d (_pick2dTable.get_first (it));

   while (pick2d) {

      if (RenderPick2d::is_valid (pick2d->get_pick_2d_handle (), context)) {

         pick2d->remove_render_module_pick (get_plugin_name (), *this);
      }

      pick2d  = _pick2dTable.get_next (it);
   }

   _pick2dTable.clear ();
}


// Plugin Interface
void
dmz::RenderModulePickBasic::discover_plugin (const Plugin *PluginPtr) {

   RenderPick2d *pick2d (RenderPick2d::cast (PluginPtr));
   
   if (pick2d) { pick2d->store_render_module_pick (get_plugin_name (), *this); }
}


void
dmz::RenderModulePickBasic::start_plugin () {
   
}


void
dmz::RenderModulePickBasic::stop_plugin () {

}


void
dmz::RenderModulePickBasic::shutdown_plugin () {

}


void
dmz::RenderModulePickBasic::remove_plugin (const Plugin *PluginPtr) {

   RenderPick2d *pick2d (RenderPick2d::cast (PluginPtr));
   
   if (pick2d) { pick2d->remove_render_module_pick (get_plugin_name (), *this); }
}


// RenderModule2dPick Interface
dmz::Boolean
dmz::RenderModulePickBasic::register_pick_2d (
      const Handle SourceHandle,
      RenderPick2d &pick2d) {

   Boolean result (False);

   if (!_pick2dTable.lookup (SourceHandle)) {
   
      if (_pick2dTable.store (SourceHandle, &pick2d)) { result = True; }
   }

   return result;
}


dmz::Boolean
dmz::RenderModulePickBasic::release_pick_2d (
      const Handle SourceHandle,
      RenderPick2d &pick2d) {

   Boolean result (False);

   RenderPick2d *pick2dStored (_pick2dTable.lookup (SourceHandle));
   
   if (pick2dStored) {
      
      if (pick2dStored->get_pick_2d_handle () == pick2d.get_pick_2d_handle ()) {
         
         if (_pick2dTable.remove (SourceHandle)) { result = True; }
      }
   }

   return result;
}


dmz::Boolean
dmz::RenderModulePickBasic::screen_to_world (
      const Handle Source,
      const Int32 ScreenPosX,
      const Int32 ScreenPosY,
      Vector &worldPosition,
      Handle &objectHandle) {

   Boolean retVal (False);

   RenderPick2d *pick2d (_pick2dTable.lookup (Source));

   if (pick2d) {

      retVal = 
         pick2d->screen_to_world (
            ScreenPosX,
            ScreenPosY,
            worldPosition,
            objectHandle);
   }

   return retVal;
}


dmz::Boolean
dmz::RenderModulePickBasic::world_to_screen (
      const Handle Source,
      const Vector &WorldPosition,
      Int32 &screenPosX,
      Int32 &screenPosY) {

   Boolean retVal (False);

   RenderPick2d *pick2d (_pick2dTable.lookup (Source));

   if (pick2d) {

      retVal = pick2d->world_to_screen (WorldPosition, screenPosX, screenPosY);
   }

   return retVal;
}


dmz::Boolean
dmz::RenderModulePickBasic::source_to_world (
      const Handle Source,
      const Int32 SourcePosX,
      const Int32 SourcePosY,
      Vector &worldPosition,
      Handle &objectHandle) {

   Boolean retVal (False);

   RenderPick2d *pick2d (_pick2dTable.lookup (Source));

   if (pick2d) {

      retVal = 
         pick2d->source_to_world (
            SourcePosX,
            SourcePosY,
            worldPosition,
            objectHandle);
   }

   return retVal;
}


dmz::Boolean
dmz::RenderModulePickBasic::world_to_source (
      const Handle Source,
      const Vector &WorldPosition,
      Int32 &sourcePosX,
      Int32 &sourcePosY) {

   Boolean retVal (False);

   RenderPick2d *pick2d (_pick2dTable.lookup (Source));

   if (pick2d) {

      retVal = pick2d->world_to_source (WorldPosition, sourcePosX, sourcePosY);
   }

   return retVal;
}


void
dmz::RenderModulePickBasic::_init (Config &local) {
   
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderModulePickBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderModulePickBasic (Info, local);
}

};
