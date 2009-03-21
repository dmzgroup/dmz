#ifndef DMZ_INPUT_PLUGIN_CONTROLLER_KEY_DOT_H
#define DMZ_INPUT_PLUGIN_CONTROLLER_KEY_DOT_H

#include <dmzInputEventController.h>
#include <dmzInputObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHashTableUInt32Template.h>

namespace dmz {

   class Definitions;

   class InputPluginControllerKey : public Plugin, public InputObserverUtil {

      public:
         //! \cond
         InputPluginControllerKey (const PluginInfo &Info, Config &local);
         ~InputPluginControllerKey ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // Input Observer Interface
         virtual void update_channel_state (const UInt32 Channel, const Boolean State);

         virtual void receive_key_event (
            const UInt32 Channel,
            const InputEventKey &Value);

      protected:
         struct KeyStruct {

            const UInt32 Key;
            const Float32 Dir;
            Boolean pressed;

            KeyStruct (const UInt32 TheKey, const Float32 TheDir) :
                  Key (TheKey),
                  Dir (TheDir),
                  pressed (False) {;}
         };

         struct AxisStruct {

            KeyStruct negative;
            KeyStruct positive;
            InputEventAxis axis;

            AxisStruct (
                  const UInt32 NegKey,
                  const UInt32 PosKey,
                  const UInt32 Controller,
                  const UInt32 Axis) :
                  negative (NegKey, -1.0),
                  positive (PosKey, 1.0),
                  axis (Controller, Axis) {;}
         };

         struct ControllerStruct {

            const Handle SourceHandle;
            HashTableUInt32Template<AxisStruct> axisTable;
            HashTableUInt32Template<InputEventButton> buttonTable;

            ControllerStruct (const Handle &TheSource) : SourceHandle (TheSource) {;}
            ~ControllerStruct () { axisTable.empty (); buttonTable.empty (); }
         };

         struct ScaleStruct {

            const Float32 Scale;
            const Boolean Sticky;

            ScaleStruct (const Float32 TheScale, const Boolean IsSticky) :
               Scale (TheScale),
               Sticky (IsSticky) {;}
         };

         void _init (Config &local);
         void _default_setup (Definitions &defs);

         void _add_axis (
            const UInt32 Handle,
            const UInt32 Negative,
            const UInt32 Positive,
            ControllerStruct &cs);

         void _add_button (
            const UInt32 Handle,
            const UInt32 Value,
            ControllerStruct &cs);

         void _add_scale (const UInt32 Value, const Float32 Scale);

         Log _log;
         Float32 _scale;
         Int32 _activeChannelCount;

         HashTableHandleTemplate<ControllerStruct> _controllerTable;
         HashTableUInt32Template<AxisStruct> _axisKeyTable;
         HashTableUInt32Template<InputEventButton> _buttonTable;
         HashTableUInt32Template<ScaleStruct> _scaleTable;
         //! \endcond

      private:
         InputPluginControllerKey (const InputPluginControllerKey &);
         InputPluginControllerKey &operator= (const InputPluginControllerKey &);
   };
};

#endif // DMZ_INPUT_PLUGIN_CONTROLLER_KEY_DOT_H
