#ifndef DMZ_INPUT_MODULE_BASIC_DOT_H
#define DMZ_INPUT_MODULE_BASIC_DOT_H

#include <dmzInputEventController.h>
#include <dmzInputEventKey.h>
#include <dmzInputEventMouse.h>
#include <dmzInputModule.h>
#include <dmzInputObserver.h>
#include <dmzRuntimeHandle.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class InputModuleBasic : public Plugin, public InputModule {

      public:
         //! \cond
         struct EventQueStruct {

            EventQueStruct *next;

            EventQueStruct () : next (0) {;}
            virtual ~EventQueStruct () { if (next) { delete next; next = 0; } }
            virtual void send_event (InputModule &module) = 0;
         };

         InputModuleBasic (const PluginInfo &Info);
         ~InputModuleBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Input Module Interface
         virtual void register_input_observer (
            const Handle Channel,
            const Mask &EventMask,
            InputObserver &obs);

         virtual void release_input_observer (
            const Handle Channel,
            const Mask &EventMask,
            InputObserver &obs);

         virtual Boolean create_channel (const Handle Channel);

         virtual Boolean set_channel_state (
            const Handle Channel,
            const Boolean Value);

         virtual Boolean get_channel_state (const Handle Channel) const;

         virtual void send_axis_event (const InputEventAxis &Event);
         virtual void send_button_event (const InputEventButton &Event);
         virtual void send_switch_event (const InputEventSwitch &Event);
         virtual void send_key_event (const InputEventKey &Event);
         virtual void send_mouse_event (const InputEventMouse &Event);
         virtual void send_data_event (const Handle Source, const Data &Event);

      protected:
         struct ObsStruct {

            const Handle ObsHandle;
            InputObserver &obs;
            HashTableHandleTemplate<Mask> table;
            Int32 axisActiveCount;
            Int32 buttonActiveCount;
            Int32 switchActiveCount;
            Int32 keyActiveCount;
            Int32 mouseActiveCount;
            UInt32 eventCount;

            ObsStruct (InputObserver &theObs) :
                  ObsHandle (theObs.get_input_observer_handle ()),
                  obs (theObs),
                  axisActiveCount (0),
                  buttonActiveCount (0),
                  switchActiveCount (0),
                  keyActiveCount (0),
                  mouseActiveCount (0),
                  eventCount (0) {;}
            ~ObsStruct () { table.empty (); }
         };

         struct ChannelStruct {

            const Handle ChannelHandle;

            Boolean active;
            Boolean locked;

            HashTableHandleTemplate<InputObserver> stateTable;
            HashTableHandleTemplate<ObsStruct> axisTable;
            HashTableHandleTemplate<ObsStruct> buttonTable;
            HashTableHandleTemplate<ObsStruct> switchTable;
            HashTableHandleTemplate<ObsStruct> keyTable;
            HashTableHandleTemplate<ObsStruct> mouseTable;
            HashTableHandleTemplate<ObsStruct> dataTable;
            HashTableHandleTemplate<ObsStruct> activeTable;

            ChannelStruct (const Handle TheHandle) :
                  ChannelHandle (TheHandle),
                  active (True),
                  locked (False) {;}
         };

         struct ControllerStruct {

            const Handle SourceHandle;
            HashTableHandleTemplate<InputEventAxis> axisTable;
            HashTableHandleTemplate<InputEventButton> buttonTable;
            HashTableHandleTemplate<InputEventSwitch> switchTable;

            ControllerStruct (const Handle TheHandle) : SourceHandle (TheHandle) {;}

            ~ControllerStruct () {

               axisTable.empty ();
               buttonTable.empty ();
               switchTable.empty ();
            }
         };

         void _increment_active_count (
            const Handle Channel,
            const Mask &EventMask,
            ObsStruct &os);

         void _decrement_active_count (
            const Handle Channel,
            const Mask &EventMask,
            ObsStruct &os);

         void _que_event (EventQueStruct *event);
         void _do_qued_events ();

         ChannelStruct *_create_channel (const Handle Channel);

         ControllerStruct *_get_controller (const Handle Handle);

         Boolean _inEvent;
         Boolean _inQue;
         EventQueStruct *_eventQueHead;
         EventQueStruct *_eventQueTail;

         HashTableHandleTemplate<ChannelStruct> _channelTable;

         HashTableHandleTemplate<ObsStruct> _obsTable;

         HashTableHandleTemplate<InputEventKey> _keyCache;
         HashTableHandleTemplate<ControllerStruct> _controllerCache;

         InputEventMouse _mouseCache;

         UInt32 _eventCount;

         Log _log;
         Definitions _defs;
         //! \endcond
   };
};

#endif // DMZ_INPUT_MODULE_BASIC_DOT_H
