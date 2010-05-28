#include "dmzRenderPluginObjectLoaderOSG.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::RenderPluginObjectLoaderOSG::RenderPluginObjectLoaderOSG (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info) {

   _init (local);
}


dmz::RenderPluginObjectLoaderOSG::~RenderPluginObjectLoaderOSG () {

}


// Plugin Interface
void
dmz::RenderPluginObjectLoaderOSG::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::RenderPluginObjectLoaderOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Object Observer Interface
void
dmz::RenderPluginObjectLoaderOSG::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

}


void
dmz::RenderPluginObjectLoaderOSG::remove_object_attribute (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &AttrMask) {

}


void
dmz::RenderPluginObjectLoaderOSG::update_object_text (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const String &Value,
      const String *PreviousValue) {

}


void
dmz::RenderPluginObjectLoaderOSG::_init (Config &local) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginObjectLoaderOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginObjectLoaderOSG (Info, local);
}

};
