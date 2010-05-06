#ifndef DMZ_INPUT_MODULE_DOT_H
#define DMZ_INPUT_MODULE_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>

namespace dmz {

   //! \cond
   const char InputModuleInterfaceName[] = "InputModuleInterface";
   //! \endcond

   class Data;
   class InputEventAxis;
   class InputEventButton;
   class InputEventKey;
   class InputEventMouse;
   class InputEventSwitch;
   class InputObserver;
   class Mask;

   class InputModule {

      public:
         static InputModule *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         virtual void register_input_observer (
            const Handle Channel,
            const Mask EventMask,
            InputObserver &observer) = 0;

         virtual void release_input_observer (
            const Handle Channel,
            const Mask EventMask,
            InputObserver &observer) = 0;

         virtual Boolean create_channel (const Handle Channel) = 0;

         virtual Boolean set_channel_state (
            const Handle Channel,
            const Boolean Value) = 0;

         virtual Boolean get_channel_state (const Handle Channel) const = 0;

         virtual void send_axis_event (const InputEventAxis &Event) = 0;
         virtual void send_button_event (const InputEventButton &Event) = 0;
         virtual void send_switch_event (const InputEventSwitch &Event) = 0;
         virtual void send_key_event (const InputEventKey &Event) = 0;
         virtual void send_mouse_event (const InputEventMouse &Event) = 0;
         virtual void send_data_event (const Handle Source, const Data &Event) = 0;

      protected:
         InputModule (const PluginInfo &Info);
         ~InputModule ();

      private:
         const PluginInfo &__Info;
   };
};


inline dmz::InputModule *
dmz::InputModule::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (InputModule *)lookup_rtti_interface (
      InputModuleInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::InputModule::InputModule (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (InputModuleInterfaceName, __Info, (void *)this);
}


inline
dmz::InputModule::~InputModule () {

   remove_rtti_interface (InputModuleInterfaceName, __Info);
}

#endif // DMZ_INPUT_MODULE_DOT_H
