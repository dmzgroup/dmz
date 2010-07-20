#include <dmzObjectModule.h>
#include <dmzObjectModuleSelect.h>
#include "dmzObjectPluginSelectSimilar.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeObjectType.h>

/*!

\class dmz::ObjectPluginSelectSimilar
\ingroup Object
\brief Selects objects similar to the currently selected objects.
\details
\code
<scope>
   <select-similar-message name="String"/>
   <select-exact-message name="String"/>
</scope>
\endcode

- \b select-similar-message.name String containing the name of the message that signals the plugin to select objects that are similar to objects that are currently selected. Similar objects will be dmz::ObjectType::is_of_type(). The default message name is "Object_Select_Similar_Message".
- \b select-exact-message.name String containing the name of the message that signals the plugin to select objects that are exactly the same type as objects that are currently selected. Exact objects will be dmz::ObjectType::is_of_exact_type(). The default message name is "Object_Select_Exact_Message".

*/

//! \cond
dmz::ObjectPluginSelectSimilar::ObjectPluginSelectSimilar (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      MessageObserver (Info),
      _log (Info),
      _objMod (0),
      _select (0) {

   _init (local);
}


dmz::ObjectPluginSelectSimilar::~ObjectPluginSelectSimilar () {

}


// Plugin Interface
void
dmz::ObjectPluginSelectSimilar::update_plugin_state (
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
dmz::ObjectPluginSelectSimilar::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_objMod) { _objMod = ObjectModule::cast (PluginPtr); }
      if (!_select) { _select = ObjectModuleSelect::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_objMod && (_objMod == ObjectModule::cast (PluginPtr))) { _objMod = 0; }
      if (_select && (_select == ObjectModuleSelect::cast (PluginPtr))) { _select = 0; }
   }
}


// Message Observer Interface
void
dmz::ObjectPluginSelectSimilar::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   Boolean exact = False;

   if (Type == _selectSimilarMsg) { exact = False; }
   else if (Type == _selectExactMsg) { exact = True; }

   if (_objMod && _select) {

      HandleContainer all;
      HandleContainer selected;

      if (_objMod->get_object_handles (all)) {

         _select->get_selected_objects (selected);

         ObjectTypeSet set;

         HandleContainerIterator it;
         Handle current (0);

         while (selected.get_next (it, current)) {

            const ObjectType Type = _objMod->lookup_object_type (current);
            set.add_object_type (Type);
         }

         it.reset ();
         current = 0;

         while (all.get_next (it, current)) {

            const ObjectType Type = _objMod->lookup_object_type (current);

            Boolean match (False);

            if (exact) { if (set.contains_exact_type (Type)) { match = True; } }
            else { if (set.contains_type (Type)) { match = True; } }

            if (match) { _select->select_object (current, ObjectSelectAdd); }
         }
      }
   }
}


// ObjectPluginSelectSimilar Interface
void
dmz::ObjectPluginSelectSimilar::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   _selectSimilarMsg = config_create_message (
      "select-similar-message.name",
      local,
      "Object_Select_Similar_Message",
      context);

   _selectExactMsg = config_create_message (
      "select-exact-message.name",
      local,
      "Object_Select_Exact_Message",
      context);

   subscribe_to_message (_selectSimilarMsg);
   subscribe_to_message (_selectExactMsg);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzObjectPluginSelectSimilar (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ObjectPluginSelectSimilar (Info, local);
}

};
