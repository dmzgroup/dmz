#ifndef DMZ_INPUT_EVENT_CONTROLLER_DOT_H
#define DMZ_INPUT_EVENT_CONTROLLER_DOT_H

#include <dmzInputEventsExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class Stream;

   class DMZ_INPUT_EVENTS_LINK_SYMBOL InputEventAxis {

      public:
         InputEventAxis ();
         InputEventAxis (const Handle SourceHandle, const UInt32 Axis);
         InputEventAxis (const InputEventAxis &Value);
         ~InputEventAxis ();

         InputEventAxis &operator= (const InputEventAxis &Value);
         Boolean operator== (const InputEventAxis &Value) const;
         Boolean operator!= (const InputEventAxis &Value) const;

         void set_source_handle (const Handle SourceHandle);
         Handle get_source_handle () const;

         void set_axis_id (const UInt32 ID);
         UInt32 get_axis_id () const;

         // Only updates value if different from current value
         Boolean update_axis_value (const Float32 Value);

         void set_axis_value (const Float32 Value);
         Float32 get_axis_value () const;

         void set_axis_previous_value (const Float32 Value);
         Float32 get_axis_previous_value () const;

         Float32 get_axis_delta () const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };

   class DMZ_INPUT_EVENTS_LINK_SYMBOL InputEventButton {

      public:
         InputEventButton ();
         InputEventButton (const Handle SourceHandle, const UInt32 Button);
         InputEventButton (const InputEventButton &Value);
         ~InputEventButton ();

         InputEventButton &operator= (const InputEventButton &Value);
         Boolean operator== (const InputEventButton &Value) const;
         Boolean operator!= (const InputEventButton &Value) const;

         void set_source_handle (const Handle SourceHandle);
         Handle get_source_handle () const;

         void set_button_id (const UInt32 ID);
         UInt32 get_button_id () const;

         Boolean update_button_value (const Boolean Value);
         void set_button_value (const Boolean Value);
         Boolean get_button_value () const;

         void set_button_previous_value (const Boolean Value);
         Boolean get_button_previous_value () const;

         Boolean get_button_changed () const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };

   class DMZ_INPUT_EVENTS_LINK_SYMBOL InputEventSwitch {

      public:
         InputEventSwitch ();
         InputEventSwitch (const Handle SourceHandle, const UInt32 Switch);
         InputEventSwitch (const InputEventSwitch &Value);
         ~InputEventSwitch ();

         InputEventSwitch &operator= (const InputEventSwitch &Value);
         Boolean operator== (const InputEventSwitch &Value) const;
         Boolean operator!= (const InputEventSwitch &Value) const;

         void set_source_handle (const Handle SourceHandle);
         Handle get_source_handle () const;

         void set_switch_id (const UInt32 ID);
         UInt32 get_switch_id () const;

         Boolean update_switch_value (const UInt32 Value);
         void set_switch_value (const UInt32 Value);
         UInt32 get_switch_value () const;

         void set_switch_previous_value (const UInt32 Value);
         UInt32 get_switch_previous_value () const;

         Boolean get_switch_changed () const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };
};


DMZ_INPUT_EVENTS_LINK_SYMBOL dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::InputEventAxis &Event);

DMZ_INPUT_EVENTS_LINK_SYMBOL dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::InputEventButton &Event);

DMZ_INPUT_EVENTS_LINK_SYMBOL dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::InputEventSwitch &Event);

#endif// DMZ_INPUT_EVENT_CONTROLLER_DOT_H

