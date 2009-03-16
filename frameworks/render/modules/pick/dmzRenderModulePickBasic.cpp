#include "dmzRenderModulePickBasic.h"
#include <dmzRenderPick.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>


/*!

\class dmz::RenderModulePickBasic
\ingroup Render
\brief Basic implementation of the RenderModulePick

*/

//! \cond
dmz::RenderModulePickBasic::RenderModulePickBasic (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      RenderModulePick (Info),
      _log (Info) {

   _init (local);
}


dmz::RenderModulePickBasic::~RenderModulePickBasic () {

   HashTableHandleIterator it;

   RuntimeContext *context (_PluginInfoData.get_context ());

   RenderPick *pick (_pickTable.get_first (it));

   while (pick) {

      if (RenderPick::is_valid (pick->get_pick_handle (), context)) {

         pick->remove_render_module_pick (get_plugin_name (), *this);
      }

      pick = _pickTable.get_next (it);
   }

   _pickTable.clear ();
}


// Plugin Interface
void
dmz::RenderModulePickBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   RenderPick *pick (RenderPick::cast (PluginPtr));

   if (Mode == PluginDiscoverAdd) {

      if (pick) { pick->store_render_module_pick (get_plugin_name (), *this); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (pick) { pick->remove_render_module_pick (get_plugin_name (), *this); }
   }
}


// RenderModulePick Interface
dmz::Boolean
dmz::RenderModulePickBasic::register_pick (
      const Handle SourceHandle,
      RenderPick &pick) {

   Boolean result (False);

   if (!_pickTable.lookup (SourceHandle)) {

      if (_pickTable.store (SourceHandle, &pick)) { result = True; }
   }

   return result;
}


dmz::Boolean
dmz::RenderModulePickBasic::release_pick (
      const Handle SourceHandle,
      RenderPick &pick) {

   Boolean result (False);

   RenderPick *pickStored (_pickTable.lookup (SourceHandle));

   if (pickStored) {

      if (pickStored->get_pick_handle () == pick.get_pick_handle ()) {

         if (_pickTable.remove (SourceHandle)) { result = True; }
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
      Vector &normal,
      Handle &objectHandle) {

   Boolean retVal (False);

   RenderPick *pick (_pickTable.lookup (Source));

   if (pick) {

      retVal =
         pick->screen_to_world (
            ScreenPosX,
            ScreenPosY,
            worldPosition,
            normal,
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

   RenderPick *pick (_pickTable.lookup (Source));

   if (pick) {

      retVal = pick->world_to_screen (WorldPosition, screenPosX, screenPosY);
   }

   return retVal;
}


dmz::Boolean
dmz::RenderModulePickBasic::source_to_world (
      const Handle Source,
      const Int32 SourcePosX,
      const Int32 SourcePosY,
      Vector &worldPosition,
      Vector &normal,
      Handle &objectHandle) {

   Boolean retVal (False);

   RenderPick *pick (_pickTable.lookup (Source));

   if (pick) {

      retVal =
         pick->source_to_world (
            SourcePosX,
            SourcePosY,
            worldPosition,
            normal,
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

   RenderPick *pick (_pickTable.lookup (Source));

   if (pick) {

      retVal = pick->world_to_source (WorldPosition, sourcePosX, sourcePosY);
   }

   return retVal;
}


void
dmz::RenderModulePickBasic::_init (Config &local) {

}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderModulePickBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderModulePickBasic (Info, local);
}

};
