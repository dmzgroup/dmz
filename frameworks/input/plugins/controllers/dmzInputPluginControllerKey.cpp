#include <dmzInputEventController.h>
#include <dmzInputEventKey.h>
#include <dmzInputEventMasks.h>
#include <dmzInputModule.h>
#include "dmzInputPluginControllerKey.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzTypesBase.h>

/*!

\class dmz::InputPluginControllerKey
\ingroup Input
\brief Converts key events to controller events.

*/

//! \cond
dmz::InputPluginControllerKey::InputPluginControllerKey (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      InputObserverUtil (Info, local),
      _log (Info.get_name (), Info.get_context ()),
      _scale (1.0f),
      _activeChannelCount (0) {

   _init (local);
}


dmz::InputPluginControllerKey::~InputPluginControllerKey () {

   _axisKeyTable.clear ();
   _buttonTable.clear ();
   _scaleTable.empty ();
   _controllerTable.clear ();
}


// Input Observer Interface
void
dmz::InputPluginControllerKey::update_channel_state (
      const UInt32 Channel,
      const Boolean State) {

   if (State) { _activeChannelCount++; }
   else { _activeChannelCount--; }

   if (_activeChannelCount == 0) {

      InputModule *module (get_input_module_channels ());

      if (module) {

         HashTableHandleIterator it;

         ControllerStruct *cs (_controllerTable.get_first (it));

         while (cs) {

            HashTableUInt32Iterator axisIt;

            AxisStruct *axis (cs->axisTable.get_first (axisIt));

            while (axis) {

               if (axis->axis.update_axis_value (0.0f)) {

                  module->send_axis_event (axis->axis);
               }

               axis  = cs->axisTable.get_next (axisIt);
            }

            HashTableUInt32Iterator buttonIt;

            InputEventButton *button (cs->buttonTable.get_first (buttonIt));

            while (button) {

               if (button->get_button_value ()) {

                  button->set_button_value (False);
                  module->send_button_event (*button);
               }

               button  = cs->buttonTable.get_next (buttonIt);
            }

            cs  = _controllerTable.get_next (it);
         }
      }
   }
}


void
dmz::InputPluginControllerKey::receive_key_event (
      const UInt32 Channel,
      const InputEventKey &Value) {

   InputModule *module (get_input_module_channels ());

   if (module) {

      const UInt32 Key (Value.get_key ());
      const Boolean KeyState (Value.get_key_state ());

      AxisStruct *axis (_axisKeyTable.lookup (Key));

      if (axis) {

         KeyStruct &major (axis->positive.Key == Key ? axis->positive : axis->negative);
         KeyStruct &minor (axis->negative.Key == Key ? axis->positive : axis->negative);

         major.pressed = KeyState;

         Float32 axisValue (0.0);

         if (KeyState) { axisValue = major.Dir * _scale; }
         else if (minor.pressed) { axisValue = minor.Dir * _scale; }

         if (axis->axis.update_axis_value (axisValue)) {

            module->send_axis_event (axis->axis);
         }
      }
      else {

         InputEventButton *button (_buttonTable.lookup (Key));

         if (button) {

            if (KeyState != button->get_button_value ()) {

               button->set_button_value (Value.get_key_state ());

               module->send_button_event (*button);
            }
         }
         else {

            Float32 *value (_scaleTable.lookup (Key));

            if (value) {

               _scale = *value;
            }
         }
      }
   }
}


void
dmz::InputPluginControllerKey::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   init_input_channels (local, InputEventKeyMask, &_log);

   Config controllerList;

   if (local.lookup_all_config ("controller", controllerList)) {

      ConfigIterator it;

      Config controller;

      Boolean found (controllerList.get_first_config (it, controller));

      while (found) {

         const String ControllerName (
            config_to_string ("name", controller, "controller_default"));

         const Handle Controller (defs.create_named_handle (ControllerName));

         ControllerStruct *cs (_controllerTable.lookup (Controller));

         if (!cs) {

            cs = new ControllerStruct (Controller);

            if (!_controllerTable.store (Controller, cs)) {

               delete cs; cs = 0;
            }
         }

         ConfigIterator elementIt;

         Config cd;

         Boolean elementFound (controller.get_first_config (elementIt, cd));

         while (elementFound) {

            if (cs) {

               const String Type (cd.get_name ().get_lower ());

               if (Type == "axis") {

                  const UInt32 NegKey (config_to_key_value ("negative", cd, 0, &_log));
                  const UInt32 PosKey (config_to_key_value ("positive", cd, 0, &_log));
                  const UInt32 AxisHandle (config_to_uint32 ("id", cd, 1));

                  _add_axis (AxisHandle, NegKey, PosKey, *cs);
               }
               else if (Type == "button") {
 
                  const UInt32 Key (config_to_key_value (cd, &_log));
                  InputEventButton *button (_buttonTable.lookup (Key));

                  if (!button) {

                     const UInt32 ButtonHandle (config_to_uint32 ("id", cd, 1));

                     if (!cs->buttonTable.lookup (ButtonHandle)) {

                        button = new InputEventButton (cs->SourceHandle, ButtonHandle);

                        if (button) {

                           if (cs->buttonTable.store (ButtonHandle, button)) {

                              _buttonTable.store (Key, button);
                           }
                           else { delete button; button = 0; }
                        }
                     }
                  }
               }
               else if (Type == "scale") {

                  const UInt32 Key (config_to_key_value (cd, &_log));
                  Float32 *scale (_scaleTable.lookup (Key));

                  if (!scale) {

                      Float32 *value (
                         new Float32 (config_to_float32 ("value", cd, 1.0f)));

                     if (value) {

                        if (*value < 0.0f) { *value = 0.0f; }
                        else if (*value > 1.0f) { *value = 1.0f; }

                        if (!_scaleTable.store (Key, value)) {

                           delete value; value = 0;
                        }
                     }
                  }
               }
               else {

                  _log.error << "Unknown controller type: " << Type << endl;
               }
            }

            elementFound = controller.get_next_config (elementIt, cd);
         }

         found = controllerList.get_next_config (it, controller);
      }
   }
   else { _default_setup (defs); }
}


void
dmz::InputPluginControllerKey::_default_setup (Definitions &defs) {

   ControllerStruct *cs (
      new ControllerStruct (defs.create_named_handle ("controller_key")));

   if (cs) {

      if (_controllerTable.store (cs->SourceHandle, cs)) {

         _add_axis (2, 'w', 's', *cs);
         _add_axis (2, 'W', 'S', *cs);
         _add_axis (1, KeyLeftArrow, KeyRightArrow, *cs);
         _add_axis (6, 'a', 'd', *cs);
         _add_axis (6, 'A', 'D', *cs);
         _add_axis (7, KeyUpArrow, KeyDownArrow, *cs);

         _add_button (1, ' ', *cs);
         _add_button (2, ',', *cs);
         _add_button (3, '.', *cs);
         _add_button (4, KeyEnter, *cs);

         _add_scale ('1', 0.1f);
         _add_scale ('2', 0.2f);
         _add_scale ('3', 0.3f);
         _add_scale ('4', 0.4f);
         _add_scale ('5', 0.5f);
         _add_scale ('6', 0.6f);
         _add_scale ('7', 0.7f);
         _add_scale ('8', 0.8f);
         _add_scale ('9', 0.9f);
         _add_scale ('0', 1.0f);
      }
      else { delete cs; cs = 0; }
   }
}


void
dmz::InputPluginControllerKey::_add_axis (
      const UInt32 Handle,
      const UInt32 Negative,
      const UInt32 Positive,
      ControllerStruct &cs) {

   AxisStruct *axis (cs.axisTable.lookup (Handle));

   if (!axis) {

      AxisStruct *axis (new AxisStruct (Negative, Positive, cs.SourceHandle, Handle));

      if (cs.axisTable.store (Handle, axis)) {

         _axisKeyTable.store (Negative, axis);
         _axisKeyTable.store (Positive, axis);
      }
      else {

         delete axis; axis = 0;
      }
   }
   else {

   }
}


void
dmz::InputPluginControllerKey::_add_button (
      const UInt32 Handle,
      const UInt32 Value,
      ControllerStruct &cs) {

   InputEventButton *button (cs.buttonTable.lookup (Handle));

   if (!button) {

      button = new InputEventButton (cs.SourceHandle, Handle);

      if (button && !cs.buttonTable.store (Handle, button)) { delete button; button = 0; }
   }

   if (button) { _buttonTable.store (Value, button); }
}


void
dmz::InputPluginControllerKey::_add_scale (const UInt32 Value, const Float32 Scale) {

   Float32 *scalePtr (new Float32 (Scale));

   if (scalePtr && !_scaleTable.store (Value, scalePtr)) {

      delete scalePtr; scalePtr = 0;
   }
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzInputPluginControllerKey (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::InputPluginControllerKey (Info, local);
}

};

