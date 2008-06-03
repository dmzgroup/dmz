#ifndef DMZ_INPUT_OBSERVER_DOT_H
#define DMZ_INPUT_OBSERVER_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

#define DMZ_INPUT_OBSERVER_INTERFACE_NAME "InputObserverInteface"

namespace dmz {

   class Data;
   class InputEventAxis;
   class InputEventButton;
   class InputEventKey;
   class InputEventMouse;
   class InputEventSwitch;
   class InputModule;

   class InputObserver {

      public:
         static InputObserver *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         static Boolean is_valid (const Handle ObserverHandle, RuntimeContext *context);

         Handle get_input_observer_handle ();
         String get_input_observer_name ();

         virtual void store_input_module_channels (
            const String &Name,
            InputModule &module) = 0;

         virtual void remove_input_module_channels (
            const String &Name,
            InputModule &module) = 0;

         virtual void update_channel_state (
            const Handle Channel,
            const Boolean State) = 0;

         virtual void receive_axis_event (
            const Handle Channel,
            const InputEventAxis &Value) = 0;

         virtual void receive_button_event (
            const Handle Channel,
            const InputEventButton &Value) = 0;

         virtual void receive_switch_event (
            const Handle Channel,
            const InputEventSwitch &Value) = 0;

         virtual void receive_key_event (
            const Handle Channel,
            const InputEventKey &Value) = 0;

         virtual void receive_mouse_event (
            const Handle Channel,
            const InputEventMouse &Value) = 0;

         virtual void receive_data_event (
            const Handle Channel,
            const Handle Source,
            const Data &Value) = 0;

      protected:
         InputObserver (const PluginInfo &Info);
         ~InputObserver ();

      private:
         InputObserver ();
         InputObserver (const InputObserver &);
         InputObserver &operator= (const InputObserver &);

         const PluginInfo &__Info;
   };
};


inline dmz::InputObserver *
dmz::InputObserver::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (InputObserver *)lookup_rtti_interface (
      DMZ_INPUT_OBSERVER_INTERFACE_NAME,
      PluginName,
      PluginPtr);
}


inline dmz::Boolean
dmz::InputObserver::is_valid (const Handle ObserverHandle, RuntimeContext *context) {

   return lookup_rtti_interface (
      DMZ_INPUT_OBSERVER_INTERFACE_NAME,
      ObserverHandle,
      context) != 0;
}


inline
dmz::InputObserver::InputObserver (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (DMZ_INPUT_OBSERVER_INTERFACE_NAME, __Info, (void *)this);
} 


inline
dmz::InputObserver::~InputObserver () {

   remove_rtti_interface (DMZ_INPUT_OBSERVER_INTERFACE_NAME, __Info);
}


inline dmz::Handle
dmz::InputObserver::get_input_observer_handle () { return __Info.get_handle (); }


inline dmz::String
dmz::InputObserver::get_input_observer_name () { return __Info.get_name (); }

#endif // DMZ_INPUT_OBSERVER_DOT_H

