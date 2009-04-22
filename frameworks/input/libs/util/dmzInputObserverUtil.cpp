#include <dmzInputConsts.h>
#include <dmzInputEventMasks.h>
#include <dmzInputModule.h>
#include <dmzInputObserverUtil.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesMask.h>

namespace {

struct ChannelStruct {

   const dmz::Handle Channel;
   dmz::Mask mask;

   ChannelStruct (dmz::Handle TheHandle) : Channel (TheHandle) {;}
};

};

/*!

\class dmz::InputObserverUtil
\ingroup Input
\brief Input observer utility class.
\details Provides a base implementation of functionality shared with most input
observers.

*/

struct dmz::InputObserverUtil::State {

   const String InputModuleName;
   Log log;
   Definitions defs;
   InputModule *module;
   HashTableHandleTemplate <ChannelStruct> channelTable;
   Mask errorMask;

   State (
         const PluginInfo &Info,
         const Config &Data) :
         InputModuleName (config_to_string ("module.input.name", Data)),
         log (Info.get_name () + ".InputObserverUtil", Info.get_context ()),
         defs (Info, &log),
         module (0) {

      if (InputModuleName) {

         log.info << "Looking for input observer: " << InputModuleName << endl;
      }
   }

   ~State () { channelTable.empty (); }

   ChannelStruct *add_channel (const Handle Channel, const Mask &EventMask) {

      ChannelStruct *cs (channelTable.lookup (Channel));

      if (!cs) {

         cs = new ChannelStruct (Channel);
         if (!channelTable.store (Channel, cs)) { delete cs; cs = 0; }
      }

      if (cs) { cs->mask |= EventMask; }

      return cs;
   }

   void register_obs (ChannelStruct &cs, InputObserver &obs) {

      if (cs.Channel && module) {

         module->register_input_observer (cs.Channel, cs.mask, obs);
      }
   }

   void release_obs (const Mask &EventMask, ChannelStruct &cs, InputObserver &obs) {

      if (cs.Channel && module) {

         module->release_input_observer (cs.Channel, EventMask, obs);
      }

      cs.mask.unset (EventMask);
   }

   void release_all (InputObserver &obs) {

      if (module) {

         HashTableHandleIterator it;
         ChannelStruct *cs (0);

         while (channelTable.get_next (it, cs)) {

            module->release_input_observer (cs->Channel, cs->mask, obs);
         }
      }
   }
};


/*!

\brief Constructor
\details The InputObjserverUtil used the Config object passed in to determine which
InputModule to register with. This is done by giving the name of the InputModule.
The XML format is as follows:
\code
<module>
   <input name="Input Module Name"/>
</module>
\endcode
If the InputModule is not specifed, the observer will register witht he first InputModule
that discovers the observer.
\param[in] Info PluginInfo containing initialization data.
\param[in] Init Config used to initialize the class. This is most often the Config
object passed in as \a local to the Plugin.

*/
dmz::InputObserverUtil::InputObserverUtil (
      const PluginInfo &Info,
      const Config &Init) :
      InputObserver (Info),
      __state (*(new State (Info, Init))) {;}


//! Destructor.
dmz::InputObserverUtil::~InputObserverUtil () { delete &__state; }


/*!

\brief Activates event callbacks.
\details Used the channels specified in the Config object to activate the
event callbacks. If no channels are specified in the Config object, the default
input channel is used. The XML format is as follows:
\code
<input>
   <channel name="Input Channel Name"/>
   <!-- More input channels -->
</input>
\endcode
\param[in] Init Config object used to define the input channels.
\param[in] EventMask Mask of event callbacks to activate. The event masks are
defined in dmzInputEventMasks.h.
\param[in] log Pointer to the Log.

*/
void
dmz::InputObserverUtil::init_input_channels (
      const Config &Init,
      const Mask &EventMask,
      Log *log) {

   Config channels;

   if (Init.lookup_all_config ("input.channel", channels)) {

      ConfigIterator it;

      Config cd;

      Boolean found (channels.get_first_config (it, cd));

      while (found) {

         const String ChannelName (config_to_string ("name", cd));

         if (ChannelName) {

            activate_input_channel (ChannelName, EventMask);

            if (log) { log->info << "Activating input channel: " << ChannelName << endl; }
         }

         found = channels.get_next_config (it, cd);
      }
   }
   else {

      activate_default_input_channel (EventMask);

      if (log) { log->info << "Activating default input channel" << endl; }
   }
}


/*!

\brief Activate input channel callbacks.
\param[in] Channel Handle of channel to activate.
\param[in] EventMask Mask of event callbacks to activate. The event masks are
defined in dmzInputEventMasks.h.

*/
void
dmz::InputObserverUtil::activate_input_channel (
      const Handle Channel,
      const Mask &EventMask) {

   ChannelStruct *cs (__state.add_channel (Channel, EventMask));
   if (cs) { __state.register_obs (*cs, *this); }
}


/*!

\brief Activate input channel callbacks.
\param[in] ChannelName String containing name of channel to activate.
\param[in] EventMask Mask of event callbacks to activate. The event masks are
defined in dmzInputEventMasks.h.
\return Returns the Handle of the channel.

*/
dmz::Handle
dmz::InputObserverUtil::activate_input_channel (
      const String &ChannelName,
      const Mask &EventMask) {

   Handle result = __state.defs.create_named_handle (ChannelName);

   activate_input_channel (result, EventMask);

   return result;
}


/*!

\brief Activate default input channel callbacks.
\param[in] EventMask Mask of event callbacks to activate. The event masks are
defined in dmzInputEventMasks.h.
\return Returns the Handle of the default channel.

*/
dmz::Handle
dmz::InputObserverUtil::activate_default_input_channel (const Mask &EventMask) {

   return activate_input_channel (InputChannelDefaultName, EventMask);
}


/*!

\brief Deactivate input channel callbacks.
\param[in] ChannelName String containing name of channel to deactivate.
\param[in] EventMask Mask of event callbacks to deactivate. The event masks are
defined in dmzInputEventMasks.h.

*/
void
dmz::InputObserverUtil::deactivate_input_channel (
      const String &ChannelName,
      const Mask &EventMask) {

   deactivate_input_channel (__state.defs.lookup_named_handle (ChannelName), EventMask);
}


/*!

\brief Deactivate input channel callbacks.
\param[in] Channel Handle of the channel to deactivate.
\param[in] EventMask Mask of event callbacks to deactivate. The event masks are
defined in dmzInputEventMasks.h.

*/
void
dmz::InputObserverUtil::deactivate_input_channel (
      const Handle Channel,
      const Mask &EventMask) {

  ChannelStruct *cs (__state.channelTable.lookup (Channel));

  if (cs) { __state.release_obs (EventMask, *cs, *this); }
}


/*!

\brief Deactivate default input channel callbacks.
\param[in] EventMask Mask of event callbacks to deactivate. The event masks are
defined in dmzInputEventMasks.h.

*/
void
dmz::InputObserverUtil::deactivate_default_input_channel (const Mask &EventMask) {

   deactivate_input_channel (InputChannelDefaultName, EventMask);
}


void
dmz::InputObserverUtil::deactivate_all_input_channels () { __state.release_all (*this); }


void
dmz::InputObserverUtil::get_channels (HandleContainer &channels) {

   HashTableHandleIterator it;
   ChannelStruct *cs (0);

   while (__state.channelTable.get_next (it, cs)) {

      channels.add_handle (cs->Channel);
   }
}


//! Returns pointer to InputModule observer is currently registered with.
dmz::InputModule *
dmz::InputObserverUtil::get_input_module () { return __state.module; }


void
dmz::InputObserverUtil::store_input_module (
      const String &Name,
      InputModule &module) {

   if (!__state.InputModuleName || (Name == __state.InputModuleName)) {

      if (!__state.module) {

         __state.module = &module;

         HashTableHandleIterator it;

         ChannelStruct *cs (__state.channelTable.get_first (it));

         while (cs) {

            __state.register_obs (*cs, *this);
            cs = __state.channelTable.get_next (it);
         }

         _store_input_module (module);
      }
   }
}


void
dmz::InputObserverUtil::remove_input_module (
      const String &Name,
      InputModule &module) {

   if (__state.module == &module) {

      _remove_input_module (module);

      __state.module = 0;
   }
}


void
dmz::InputObserverUtil::update_channel_state (const Handle Channel, const Boolean State) {

   if (!__state.errorMask.contains (InputEventChannelStateMask)) {

      __state.errorMask |= InputEventChannelStateMask;
      __state.log.warn << "InputObserverUtil::store_channel_state called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::InputObserverUtil::receive_axis_event (
      const Handle Channel,
      const InputEventAxis &Value) {

   if (!__state.errorMask.contains (InputEventAxisMask)) {

      __state.errorMask |= InputEventAxisMask;
      __state.log.warn << "InputObserverUtil::receive_axis_event called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::InputObserverUtil::receive_button_event (
      const Handle Channel,
      const InputEventButton &Value) {

   if (!__state.errorMask.contains (InputEventButtonMask)) {

      __state.errorMask |= InputEventButtonMask;
      __state.log.warn << "InputObserverUtil::receive_button_event called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::InputObserverUtil::receive_switch_event (
      const Handle Channel,
      const InputEventSwitch &Value) {

   if (!__state.errorMask.contains (InputEventSwitchMask)) {

      __state.errorMask |= InputEventSwitchMask;
      __state.log.warn << "InputObserverUtil::receive_switch_event called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::InputObserverUtil::receive_key_event (
      const Handle Channel,
      const InputEventKey &Value) {

   if (!__state.errorMask.contains (InputEventKeyMask)) {

      __state.errorMask |= InputEventKeyMask;
      __state.log.warn << "InputObserverUtil::receive_key_event called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::InputObserverUtil::receive_mouse_event (
      const Handle Channel,
      const InputEventMouse &Value) {

   if (!__state.errorMask.contains (InputEventMouseMask)) {

      __state.errorMask |= InputEventMouseMask;
      __state.log.warn << "InputObserverUtil::receive_mouse_event called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::InputObserverUtil::receive_data_event (
      const Handle Channel,
      const Handle Source,
      const Data &Value) {

   if (!__state.errorMask.contains (InputEventDataMask)) {

      __state.errorMask |= InputEventDataMask;
      __state.log.warn << "InputObserverUtil::receive_data_event called."
         << " Function should have been overridden?" << endl;
   }
}


/*!

\fn void dmz::InputObserverUtil::_store_input_module (InputModule &module)
\brief Callback made when InputModule is discovered.

\fn void dmz::InputObserverUtil::_remove_input_module (InputModule &module) {;}
\brief Callback made when InputModule is removed.

*/
