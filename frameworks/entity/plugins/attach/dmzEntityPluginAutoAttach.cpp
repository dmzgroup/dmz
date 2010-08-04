#include <dmzEntityConsts.h>
#include "dmzEntityPluginAutoAttach.h"
#include <dmzInputEventMasks.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::EntityPluginAutoAttach
\ingroup Entity
\brief Automatically attaches to entities.

*/

//! \cond
dmz::EntityPluginAutoAttach::EntityPluginAutoAttach (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      MessageObserver (Info),
      InputObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _convert (Info),
      _active (0),
      _attached (0) {

   _init (local);
}


dmz::EntityPluginAutoAttach::~EntityPluginAutoAttach () {

}


// Plugin Interface
void
dmz::EntityPluginAutoAttach::update_plugin_state (
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
dmz::EntityPluginAutoAttach::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Message Observer Interface
void
dmz::EntityPluginAutoAttach::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   if (Type == _attachMsg) {

      _attached = _convert.to_handle (InData);
   }
   else if (Type == _nextMsg) {

      if (_list.contains (_attached)) {

         if (_list.get_count () > 1) {

            Handle found (0);

            if (_list.get_last () == _attached) { found = _list.get_first (); }
            else {

               HandleContainerIterator it;
               Handle next (0);

               while (!found && _list.get_next (it, next)) {

                  if (next == _attached) { _list.get_next (it, found); }
               }
            }

            if (found) {

               Data out = _convert.to_data (found);
               _attachMsg.send (&out);
            }
         }
      }
      else if (_list.get_count () > 0) { _attached = 0; _find_object (); }
   }
}


// Input Observer Interface
void
dmz::EntityPluginAutoAttach::update_channel_state (
      const Handle Channel,
      const Boolean State) {

   _active += State ? 1 : -1;

  if (_active == 1) { _find_object (); }
}


// Object Observer Interface
void
dmz::EntityPluginAutoAttach::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (_types.contains_type (Type)) {

      _list.add (ObjectHandle);

      if (!_attached) { _find_object (); }
   }
}


void
dmz::EntityPluginAutoAttach::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   _list.remove (ObjectHandle);

   if (ObjectHandle == _attached) { _attached = 0; _find_object (); }
}


void
dmz::EntityPluginAutoAttach::_find_object () {

   if (!_attached && _active) {

      const Handle Next = _list.get_first ();

      if (Next) {

         Data out = _convert.to_data (Next);
         _attachMsg.send (&out);
      }
   }
}


void
dmz::EntityPluginAutoAttach::_init (Config &local) {

   RuntimeContext *context = get_plugin_runtime_context ();

   init_input_channels (local, InputEventChannelStateMask, &_log);

   _nextMsg = config_create_message (
      "next-message.name",
      local,
      "DMZ_Entity_Next_Message",
      context);

   subscribe_to_message (_nextMsg);

   _attachMsg = config_create_message (
      "attach-message.name",
      local,
      EntityMessageAttachName,
      context);

   subscribe_to_message (_attachMsg);

   activate_default_object_attribute (ObjectCreateMask | ObjectDestroyMask);

   _types = config_to_object_type_set (local, context);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginAutoAttach (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginAutoAttach (Info, local);
}

};
