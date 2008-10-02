#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzQtCanvasConsts.h>
#include "dmzQtPluginCanvasAutoLink.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>


dmz::QtPluginCanvasAutoLink::QtPluginCanvasAutoLink (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defaultAttrHandle (0),
      _activeAttrHandle (0),
      _linkAttrHandle (0),
      _activeLayer (0),
      _ignoreSet (),
      _linkSet () {

   _init (local);
}


dmz::QtPluginCanvasAutoLink::~QtPluginCanvasAutoLink () {

}


// Object Observer Interface
void
dmz::QtPluginCanvasAutoLink::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   ObjectModule *objMod (get_object_module ());

   if (_activeLayer && objMod) {

      if (!_ignoreSet.contains_type (Type) && _linkSet.contains_type (Type)) {

         objMod->link_objects (_linkAttrHandle, ObjectHandle, _activeLayer);
      }
   }
}


void
dmz::QtPluginCanvasAutoLink::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (AttributeHandle == _activeAttrHandle) {

      if (Value) {

         _activeLayer = ObjectHandle;
      }
      else {

         if (ObjectHandle == _activeLayer) { _activeLayer = 0; }
      }
   }
}


void
dmz::QtPluginCanvasAutoLink::_init (Config &local) {

   _defaultAttrHandle = activate_default_object_attribute (ObjectCreateMask);

   _activeAttrHandle = activate_object_attribute (
      config_to_string ("layer.active", local, ObjectAttributeLayerActiveName),
      ObjectFlagMask);

   _linkAttrHandle = config_to_named_handle (
      "layer.link",
      local,
      ObjectAttributeLayerLinkName,
      get_plugin_runtime_context ());

   _get_type_set ("ignore", local, _ignoreSet);
   _get_type_set ("link", local, _linkSet);
}


void
dmz::QtPluginCanvasAutoLink::_get_type_set (
      const String &Name,
      Config &config,
      ObjectTypeSet &set) {

   RuntimeContext *context (get_plugin_runtime_context ());

   Config typeList;

   if (config.lookup_all_config (Name + ".type", typeList)) {

      ConfigIterator it;
      Config type;

      while (typeList.get_next_config (it, type)) {

         set.add_object_type (config_to_string ("name", type), context);
      }
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginCanvasAutoLink (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginCanvasAutoLink (Info, local);
}

};
