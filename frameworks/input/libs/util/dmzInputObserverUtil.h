#ifndef DMZ_INPUT_OBSERVER_UTIL_DOT_H
#define DMZ_INPUT_OBSERVER_UTIL_DOT_H

#include <dmzInputObserver.h>
#include <dmzInputUtilExport.h>
#include <dmzTypesHandleContainer.h>

namespace dmz {

   class Config;
   class HandleContainer;
   class Log;

   class DMZ_INPUT_UTIL_LINK_SYMBOL InputObserverUtil : public InputObserver {

      public:
         void init_input_channels (
            const Config &Init,
            const Mask &EventMask,
            Log *log = 0);

         void activate_input_channel (const Handle Channel, const Mask &EventMask);
         Handle activate_input_channel (const String &ChannelName, const Mask &EventMask);

         Handle activate_default_input_channel (const Mask &EventMask);

         void deactivate_input_channel (const String &ChannelName, const Mask &EventMask);
         void deactivate_input_channel (const Handle Channel, const Mask &EventMask);
         void deactivate_default_input_channel (const Mask &EventMask);
         void deactivate_all_input_channels ();

         void get_channels (HandleContainer &channels);

         InputModule *get_input_module ();

         virtual void store_input_module (
            const String &Name,
            InputModule &module);

         virtual void remove_input_module (
            const String &Name,
            InputModule &module);

         virtual void update_channel_state (const Handle Channel, const Boolean State);

         virtual void receive_axis_event (
            const Handle Channel,
            const InputEventAxis &Value);

         virtual void receive_button_event (
            const Handle Channel,
            const InputEventButton &Value);

         virtual void receive_switch_event (
            const Handle Channel,
            const InputEventSwitch &Value);

         virtual void receive_key_event (
            const Handle Channel,
            const InputEventKey &Value);

         virtual void receive_mouse_event (
            const Handle Channel,
            const InputEventMouse &Value);

         virtual void receive_data_event (
            const Handle Channel,
            const Handle Source,
            const Data &Value);

      protected:
         InputObserverUtil (const PluginInfo &Info, const Config &Init);
         ~InputObserverUtil ();

         virtual void _store_input_module (InputModule &module) {;}
         virtual void _remove_input_module (InputModule &module) {;}

      private:
         struct State;
         InputObserverUtil ();
         InputObserverUtil (const InputObserverUtil &);
         InputObserverUtil &operator= (const InputObserverUtil &);
         State &__state;
   };

   DMZ_INPUT_UTIL_LINK_SYMBOL HandleContainer config_to_input_channels (
      const Config &Source,
      RuntimeContext *context);
};

#endif // DMZ_INPUT_OBSERVER_UTIL_DOT_H

