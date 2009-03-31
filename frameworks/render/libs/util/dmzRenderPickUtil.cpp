#include <dmzRenderModulePick.h>
#include <dmzRenderPickUtil.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>

/*!

\class dmz::RenderPickUtil
\ingroup Render
\brief Render Pick utility class

*/

struct dmz::RenderPickUtil::State {

   const String RenderModulePickName;
   const String SourceName;
   Log log;
   Definitions defs;
   RenderModulePick *module;

   State (const PluginInfo &Info, const Config &Init) :
         RenderModulePickName (config_to_string ("module.pick.name", Init)),
         SourceName (config_to_string ("source.name", Init, Info.get_name ())),
         log (Info.get_name () + ".RenderPickUtil", Info.get_context ()),
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


/*!

\brief Constructor.
\details The RenderPickUtil uses the Config object passed in to specify with
RenderModulePick to register with. This is done bye giving the name of the
RenderModulePick. The source name may also be specified. If a source name is not
specified, the name in the PluginInfo is used as the source name.
The format is as follows:
\code
<module>
   <pick name="Pick Module Name"/>
<module>
<source name="Source Name"/>
\endcode
\param[in] Info PluginInfo used to initialize the Plugin that is derived from this class.
\param[in] Init Config used to initialize the class. This is most often the Config object
passed in as local to the Plugin.

*/
dmz::RenderPickUtil::RenderPickUtil (
      const PluginInfo &Info,
      const Config &Init) :
      RenderPick (Info),
      __state (*(new State (Info, Init))) {;}


//! Destructor.
dmz::RenderPickUtil::~RenderPickUtil () {

   if (__state.module) { remove_render_module_pick ("", *(__state.module)); }
   delete &__state;
}


/*!

\brief Gets pick module with which the pick object is registered.
\return Returns a pointer to the RenderModulePick with which the pick object is
registered.
Returns NULL if the pick object is not registered with any RenderModulePick.

*/
dmz::RenderModulePick *
dmz::RenderPickUtil::get_render_module_pick () { return __state.module; }


// RenderPick Interface.
void
dmz::RenderPickUtil::store_render_module_pick (
      const String &Name,
      RenderModulePick &module) {

   if (!__state.RenderModulePickName || (Name == __state.RenderModulePickName)) {

      if (!__state.module) {

         __state.module = &module;

         __state.module->register_pick (
            __state.defs.lookup_named_handle (__state.SourceName),
            *this);

         _store_render_module_pick (module);
      }
   }
}


void
dmz::RenderPickUtil::remove_render_module_pick (
      const String &Name,
      RenderModulePick &module) {

   if (__state.module == &module) {

      _remove_render_module_pick (module);

      __state.module->release_pick (
         __state.defs.lookup_named_handle (__state.SourceName),
         *this);

      __state.module = 0;
   }
}


dmz::Boolean
dmz::RenderPickUtil::screen_to_world (
      const Int32 ScreenPosX,
      const Int32 ScreenPosY,
      Vector &worldPosition,
      Vector &normal,
      Handle &objectHandle) {

   __state.log.warn << "Base screen_to_world called."
      << " Function should have been overridden?" << endl;

   return False;
}


dmz::Boolean
dmz::RenderPickUtil::world_to_screen (
      const Vector &WorldPosition,
      Int32 &screenPosX,
      Int32 &screenPosY) {

   __state.log.warn << "Base world_to_screen called."
      << " Function should have been overridden?" << endl;

   return False;
}


dmz::Boolean
dmz::RenderPickUtil::source_to_world (
      const Int32 SourcePosX,
      const Int32 SourcePosY,
      Vector &worldPosition,
      Vector &normal,
      Handle &objectHandle) {

   __state.log.warn << "Base source_to_world called."
      << " Function should have been overridden?" << endl;

   return False;
}


dmz::Boolean
dmz::RenderPickUtil::world_to_source (
      const Vector &WorldPosition,
      Int32 &sourcePosX,
      Int32 &sourcePosY) {

   __state.log.warn << "Base world_to_source called."
      << " Function should have been overridden?" << endl;

   return False;
}

/*!

\fn void dmz::RenderPickUtil::_store_render_module_pick (RenderModulePick &pickMod)
\brief Callback made when pick module is discovered.

\fn void dmz::RenderPickUtil::_remove_render_module_pick (RenderModulePick &pickMod)
\brief Callback made when pick module is removed.

*/
