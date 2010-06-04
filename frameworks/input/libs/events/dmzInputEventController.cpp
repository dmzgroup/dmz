#include <dmzInputEventController.h>
#include <dmzSystemStream.h>
#include <dmzTypesMath.h>

/*!

\class dmz::InputEventAxis
\ingroup Input
\brief Controller axis event.

*/

struct dmz::InputEventAxis::State {

   Handle sourceHandle;
   UInt32 axisID;
   Float32 value;
   Float32 valuePrevious;

   State () :
         sourceHandle (0),
         axisID (0),
         value (0.0f),
         valuePrevious (0.0f) {;}

   State &operator= (const State &Value) {

      sourceHandle = Value.sourceHandle;
      axisID = Value.axisID;
      value = Value.value;
      valuePrevious = Value.valuePrevious;

      return *this;
   }

   Boolean operator== (const State &Value) const {

      return (sourceHandle == Value.sourceHandle) &&
         (axisID == Value.axisID) &&
         (is_zero32 (value - Value.value)) &&
         (is_zero32 (valuePrevious - Value.valuePrevious));
   }
};


//! Base constructor.
dmz::InputEventAxis::InputEventAxis () : _state (*(new State)) {

}


/*!

\brief Constructor.
\param[in] Source Handle of the source of the event.
\param[in] Axis Which axis.

*/
dmz::InputEventAxis::InputEventAxis (const Handle Source, const UInt32 Axis) :
      _state (*(new State)) {

   _state.sourceHandle = Source;
   _state.axisID = Axis;
}


//! Copy constructor.
dmz::InputEventAxis::InputEventAxis (const InputEventAxis &Value) :
      _state (*(new State)) {

   _state = Value._state;
}


//! Destructor.
dmz::InputEventAxis::~InputEventAxis () { delete &_state; }


//! Assignment operator.
dmz::InputEventAxis &
dmz::InputEventAxis::operator= (const InputEventAxis &Value) {

   _state = Value._state;
   return *this;
}


//! Relational "equal to" operator.
dmz::Boolean
dmz::InputEventAxis::operator== (const InputEventAxis &Value) const {

   return _state == Value._state;
}


//! Relational "not equal to" operator.
dmz::Boolean
dmz::InputEventAxis::operator!= (const InputEventAxis &Value) const {

   return !(_state == Value._state);
}


//! Sets Handle of event's source.
void
dmz::InputEventAxis::set_source_handle (const Handle SourceHandle) {

   _state.sourceHandle = SourceHandle;
}


//! Gets Handle of event's source.
dmz::Handle
dmz::InputEventAxis::get_source_handle () const {

   return _state.sourceHandle;
}


//! Sets axis ID.
void
dmz::InputEventAxis::set_axis_id (const UInt32 ID) {

   _state.axisID = ID;
}


//! Gets axis ID.
dmz::UInt32
dmz::InputEventAxis::get_axis_id () const {

   return _state.axisID;
}


/*!

\brief Updates the value of the axis.
\param[in] Value Axis value in a range of -1.0 to 1.0.
\return Returns dmz::True if \a Value is different from the current event value.
\note The previous value of the axis is updated only if \a Value is different from the
current axis value.

*/
dmz::Boolean
dmz::InputEventAxis::update_axis_value (const Float32 Value) {

   Boolean result (False);

   if (!is_zero32 (Value - _state.value)) {

      set_axis_value (Value);
      result = True;
   }

   return result;
}


/*!

\brief Sets axis value.
\details Should be in the range of -1.0 to 1.0. Automatically sets the events previous
value.
\param[in] Value Current axis value.

*/
void
dmz::InputEventAxis::set_axis_value (const Float32 Value) {

   _state.valuePrevious = _state.value;
   _state.value = Value;
}


//! Gets axis value.
dmz::Float32
dmz::InputEventAxis::get_axis_value () const { return _state.value; }


//! Sets previous axis value.
void
dmz::InputEventAxis::set_axis_previous_value (const Float32 Value) {

   _state.valuePrevious = Value;
}


//! Gets previous axis value.
dmz::Float32
dmz::InputEventAxis::get_axis_previous_value () const { return _state.valuePrevious; }


//! Gets the delta of the axis.
dmz::Float32
dmz::InputEventAxis::get_axis_delta () const {

   return _state.value - _state.valuePrevious;
}


/*!

\class dmz::InputEventButton
\ingroup Input
\brief Controller button event.

*/

struct dmz::InputEventButton::State {

   Handle sourceHandle;
   UInt32 buttonID;
   Boolean value;
   Boolean valuePrevious;

   State () :
         sourceHandle (0),
         buttonID (0),
         value (False),
         valuePrevious (False) {;}

   State &operator= (const State &Value) {

      sourceHandle = Value.sourceHandle;
      buttonID = Value.buttonID;
      value = Value.value;
      valuePrevious = Value.valuePrevious;

      return *this;
   }

   Boolean operator== (const State &Value) const {

      return (sourceHandle == Value.sourceHandle) &&
         (buttonID == Value.buttonID) &&
         (value == Value.value) &&
         (valuePrevious == Value.valuePrevious);
   }
};


//! Base constructor.
dmz::InputEventButton::InputEventButton () : _state (*(new State)) {;}


/*!

\brief Constructor.
\param[in] Source Handle of the source of the event.
\param[in] Button Which button.

*/
dmz::InputEventButton::InputEventButton (const Handle Source, const UInt32 Button) :
      _state (*(new State)) {

   _state.sourceHandle = Source;
   _state.buttonID = Button;
}


//! Copy constructor.
dmz::InputEventButton::InputEventButton (const InputEventButton &Value) :
      _state (*(new State)) { *this = Value; }


//! Destructor.
dmz::InputEventButton::~InputEventButton () { delete &_state; }


//! Assignment operator.
dmz::InputEventButton &
dmz::InputEventButton::operator= (const InputEventButton &Value) {

   _state = Value._state;
   return *this;
}


//! Relational "equal to" operator.
dmz::Boolean
dmz::InputEventButton::operator== (const InputEventButton &Value) const {

   return _state == Value._state;
}


//! Relational "not equal to" operator.
dmz::Boolean
dmz::InputEventButton::operator!= (const InputEventButton &Value) const {

   return !(_state == Value._state);
}


//! Sets Handle of the source of the event.
void
dmz::InputEventButton::set_source_handle (const Handle SourceHandle) {

   _state.sourceHandle = SourceHandle;
}


//! Gets Handle of the source of the event.
dmz::Handle
dmz::InputEventButton::get_source_handle () const {

   return _state.sourceHandle;
}


//! Sets the button's ID.
void
dmz::InputEventButton::set_button_id (const UInt32 ID) {

   _state.buttonID = ID;
}


//! Gets the button's ID.
dmz::UInt32
dmz::InputEventButton::get_button_id () const { return _state.buttonID; }


/*!

\brief Updates the value of the button.
\param[in] Value Current value of the button.
\return Returns dmz::True if \a Value is different than the current value of the button.
\note The previous button value will only be updated is \a Value is different from
the current button value.

*/
dmz::Boolean
dmz::InputEventButton::update_button_value (const Boolean Value) {

   Boolean result (False);

   if (Value != _state.value) {

      set_button_value (Value);
      result = True;
   }

   return result;
}


/*!

\brief Sets button's current value.
\details Automatically updates the previous button value.
\param[in] Value Current button value.

*/
void
dmz::InputEventButton::set_button_value (const Boolean Value) {

   _state.valuePrevious = _state.value;
   _state.value = Value;
}


//! Gets button's value.
dmz::Boolean
dmz::InputEventButton::get_button_value () const { return _state.value; }


//! Sets button's previous value.
void
dmz::InputEventButton::set_button_previous_value (const Boolean Value) {

   _state.valuePrevious = Value;
}


//! Gets button's previous value.
dmz::Boolean
dmz::InputEventButton::get_button_previous_value () const {

   return _state.valuePrevious;
}


/*!

\brief Tests if button value has changed.
\return Returns dmz::True if the buttons value has changed since the last event.

*/
dmz::Boolean
dmz::InputEventButton::get_button_changed () const {

   return _state.value != _state.valuePrevious;
}


/*!

\class dmz::InputEventSwitch
\ingroup Input
\brief Controller switch event.

*/
struct dmz::InputEventSwitch::State {

   Handle sourceHandle;
   UInt32 switchID;
   UInt32 value;
   UInt32 valuePrevious;

   State () :
         sourceHandle (0),
         switchID (0),
         value (0),
         valuePrevious (0) {;}

   State &operator= (const State &Value) {

      sourceHandle = Value.sourceHandle;
      switchID = Value.switchID;
      value = Value.value;
      valuePrevious = Value.valuePrevious;

      return *this;
   }

   Boolean operator== (const State &Value) const {

      return (sourceHandle == Value.sourceHandle) &&
         (switchID == Value.switchID) &&
         (value == Value.value) &&
         (valuePrevious == Value.valuePrevious);
   }
};


//! Base constructor.
dmz::InputEventSwitch::InputEventSwitch () : _state (*(new State)) {;}


/*!

\brief Constructor.
\param[in] Source Handle of the source of the event.
\param[in] Switch Which switch.

*/
dmz::InputEventSwitch::InputEventSwitch (const Handle Source, const UInt32 Switch) :
      _state (*(new State)) {

   _state.sourceHandle = Source;
   _state.switchID = Switch;
}


//! Copy constructor.
dmz::InputEventSwitch::InputEventSwitch (const InputEventSwitch &Value) :
      _state (*(new State)) {

   *this = Value;
}


//! Destructor.
dmz::InputEventSwitch::~InputEventSwitch () { delete &_state; }


//! Assignment operator.
dmz::InputEventSwitch &
dmz::InputEventSwitch::operator= (const InputEventSwitch &Value) {

   _state = Value._state;
   return *this;
}


//! Relational "equal to" operator.
dmz::Boolean
dmz::InputEventSwitch::operator== (const InputEventSwitch &Value) const {

   return _state == Value._state;
}


//! Relational "not equal to" operator.
dmz::Boolean
dmz::InputEventSwitch::operator!= (const InputEventSwitch &Value) const {

   return !(_state == Value._state);
}


//! Sets Handle of source of the event.
void
dmz::InputEventSwitch::set_source_handle (const Handle SourceHandle) {

   _state.sourceHandle = SourceHandle;
}


//! Gets handle of source of the event.
dmz::Handle
dmz::InputEventSwitch::get_source_handle () const {

   return _state.sourceHandle;
}


//! Sets switch ID.
void
dmz::InputEventSwitch::set_switch_id (const UInt32 ID) {

   _state.switchID = ID;
}


//! Gets switch ID.
dmz::UInt32
dmz::InputEventSwitch::get_switch_id () const {

   return _state.switchID;
}


/*!

\brief Updates the value of the switch.
\param[in] Value Switch value.
\return Returns dmz::True if the \a Value is different from the current switch value.
\note The previous value of the switch is updated only if \a Value is different from
the current switch value.

*/
dmz::Boolean
dmz::InputEventSwitch::update_switch_value (const UInt32 Value) {

   Boolean result (False);

   if (Value != _state.value) {

      set_switch_value (Value);
      result = True;
   }

   return result;
}


/*!

\brief Sets the switch's value.
\param[in] Value Current value of the switch.
\note Automatically sets the switch's previous value.

*/
void
dmz::InputEventSwitch::set_switch_value (const UInt32 Value) {

   _state.valuePrevious = _state.value;
   _state.value = Value;
}


//! Gets the switch's current value.
dmz::UInt32
dmz::InputEventSwitch::get_switch_value () const { return _state.value; }


//! Sets the switch's previous value.
void
dmz::InputEventSwitch::set_switch_previous_value (const UInt32 Value) {

   _state.valuePrevious = Value;
}


//! Gets the switch's previous value.
dmz::UInt32
dmz::InputEventSwitch::get_switch_previous_value () const {

   return _state.valuePrevious;
}


/*!

\brief Tests if the switch's value has changed.
\return Returns dmz::True if the switch's value has changed.

*/
dmz::Boolean
dmz::InputEventSwitch::get_switch_changed () const {

   return _state.value != _state.valuePrevious;
}



dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::InputEventAxis &Event) {

   stream << dmz::endl
      << "             Source: " << Event.get_source_handle () << dmz::endl
      << "            Axis ID: " << Event.get_axis_id () << dmz::endl
      <<          "Axis Value: " << Event.get_axis_value () << dmz::endl
      << "Axis Previous Value: " << Event.get_axis_previous_value () << dmz::endl
      << "   Axis Delta Value: " << Event.get_axis_delta () << dmz::endl;

   return stream;
}


dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::InputEventButton &Event) {

   stream << dmz::endl
      << "               Source: " << Event.get_source_handle () << dmz::endl
      << "            Button ID: " << Event.get_button_id () << dmz::endl
      << "         Button Value: " << (Event.get_button_value () ? "Pressed" : "Release")
         << dmz::endl
      << "Button Previous Value: "
         << (Event.get_button_previous_value () ? "Pressed" : "Release") << dmz::endl;

   return stream;
}


dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::InputEventSwitch &Event) {

   stream << dmz::endl
      << "               Source: " << Event.get_source_handle () << dmz::endl
      << "            Switch ID: " << Event.get_switch_id () << dmz::endl
      << "         Switch Value: " << Event.get_switch_value () << dmz::endl
      << "Switch Previous Value: " << Event.get_switch_previous_value () << dmz::endl;

   return stream;
}

