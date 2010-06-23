#include <dmzInputEventMasks.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModuleSelect.h>
#include "dmzObjectPluginSelectIsect.h"
#include <dmzRenderModuleIsect.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::ObjectPluginSelectIsect::ObjectPluginSelectIsect (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      InputObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _select (0),
      _isect (0),
      _active (0) {

   _init (local);
}


dmz::ObjectPluginSelectIsect::~ObjectPluginSelectIsect () {

}


// Plugin Interface
void
dmz::ObjectPluginSelectIsect::update_plugin_state (
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
dmz::ObjectPluginSelectIsect::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_select) { _select = ObjectModuleSelect::cast (PluginPtr); }
      if (!_isect) { _isect = RenderModuleIsect::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_select && (_select == ObjectModuleSelect::cast (PluginPtr))) { _select = 0; }
      if (_isect && (_isect == RenderModuleIsect::cast (PluginPtr))) { _isect = 0; }
   }
}


// Input Observer Interface
void
dmz::ObjectPluginSelectIsect::update_channel_state (
      const Handle Channel,
      const Boolean State) {

   _active += State ? 1 : -1;

   if (_isect && _select) {

      HandleContainer list;

      _select->get_selected_objects (list);

      HandleContainerIterator it;
      Handle object (0);

      if (_active == 1) {

         while (list.get_next (it, object)) { _isect->disable_isect (object); }
      }
      else if (_active == 0) {

         while (list.get_next (it, object)) { _isect->enable_isect (object); }
      }
   }
}


// Object Observer Interface
void
dmz::ObjectPluginSelectIsect::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (_isect && _active) {

      if (Value) { _isect->disable_isect (ObjectHandle); }
      else { _isect->enable_isect (ObjectHandle); }
   }
}


// ObjectPluginSelectIsect Interface
void
dmz::ObjectPluginSelectIsect::_init (Config &local) {

   activate_object_attribute (ObjectAttributeSelectName, ObjectFlagMask);

   Config list;

   if (local.lookup_all_config ("channel", list)) {

      ConfigIterator it;
      Config init;

      while (list.get_next_config (it, init)) {

         if (!config_to_boolean ("isect", init, True)) {

            const String Name = config_to_string ("name", init);

            if (Name) {

               activate_input_channel (Name, InputEventChannelStateMask);
            }
         }
      }
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzObjectPluginSelectIsect (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ObjectPluginSelectIsect (Info, local);
}

};
