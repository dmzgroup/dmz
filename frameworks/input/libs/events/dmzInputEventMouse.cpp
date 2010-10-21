#include <dmzInputEventMouse.h>
#include <dmzSystemStream.h>

/*!

\class dmz::InputEventMouse
\ingroup Input
\brief Mouse input event.

*/

struct dmz::InputEventMouse::State {

   Handle sourceHandle;
   Int32 windowSizeX;
   Int32 windowSizeY;
   Int32 mouseX;
   Int32 mouseY;
   Int32 mouseXPrevious;
   Int32 mouseYPrevious;
   Int32 screenX;
   Int32 screenY;
   Int32 screenXPrevious;
   Int32 screenYPrevious;
   UInt32 buttonMask;
   UInt32 buttonMaskPrevious;
   Int32 scrollDeltaX;
   Int32 scrollDeltaY;

   State () :
         sourceHandle (0),
         windowSizeX (0),
         windowSizeY (0),
         mouseX (0),
         mouseY (0),
         mouseXPrevious (0),
         mouseYPrevious (0),
         screenX (0),
         screenY (0),
         screenXPrevious (0),
         screenYPrevious (0),
         buttonMask (0),
         buttonMaskPrevious (0),
         scrollDeltaX (0),
         scrollDeltaY (0) {;}

   State &operator= (const State &Value) {

      sourceHandle = Value.sourceHandle;
      windowSizeX = Value.windowSizeX;
      windowSizeY = Value.windowSizeY;
      mouseX = Value.mouseX;
      mouseY = Value.mouseY;
      mouseXPrevious = Value.mouseXPrevious;
      mouseYPrevious = Value.mouseYPrevious;
      screenX = Value.screenX;
      screenY = Value.screenY;
      screenXPrevious = Value.screenXPrevious;
      screenYPrevious = Value.screenYPrevious;
      buttonMask = Value.buttonMask;
      buttonMaskPrevious = Value.buttonMaskPrevious;
      scrollDeltaX = Value.scrollDeltaX;
      scrollDeltaY = Value.scrollDeltaY;

      return *this;
   }

   Boolean operator== (const State &Value) const {

      return (sourceHandle == Value.sourceHandle) &&
         (windowSizeX == Value.windowSizeX) &&
         (windowSizeY == Value.windowSizeY) &&
         (mouseX == Value.mouseX) &&
         (mouseY == Value.mouseY) &&
         (mouseXPrevious == Value.mouseXPrevious) &&
         (mouseYPrevious == Value.mouseYPrevious) &&
         (screenX == Value.screenX) &&
         (screenY == Value.screenY) &&
         (screenXPrevious == Value.screenXPrevious) &&
         (screenYPrevious == Value.screenYPrevious) &&
         (buttonMask == Value.buttonMask) &&
         (buttonMaskPrevious == Value.buttonMaskPrevious) &&
         (scrollDeltaX == Value.scrollDeltaX) &&
         (scrollDeltaY == Value.scrollDeltaY);
   }
};


//! Base constructor.
dmz::InputEventMouse::InputEventMouse () : _state (*(new State)) {;}


//! Copy constructor.
dmz::InputEventMouse::InputEventMouse (const InputEventMouse &Value) :
      _state (*(new State)) {

   *this = Value;
}


//! Destructor.
dmz::InputEventMouse::~InputEventMouse () { delete &_state; }


//! Assignment operator.
dmz::InputEventMouse &
dmz::InputEventMouse::operator= (const InputEventMouse &Value) {

   _state = Value._state;
   return *this;
}


//! Relational "equal to" operator.
dmz::Boolean
dmz::InputEventMouse::operator== (const InputEventMouse &Value) const {

   return _state == Value._state;
}


//! Relational "not equal to" operator.
dmz::Boolean
dmz::InputEventMouse::operator!= (const InputEventMouse &Value) const {

   return !(_state == Value._state);
}


//! Sets Handle of the source of the event.
void
dmz::InputEventMouse::set_source_handle (const Handle SourceHandle) {

   _state.sourceHandle = SourceHandle;
}


//! Gets Handle of the source of the event.
dmz::Handle
dmz::InputEventMouse::get_source_handle () const {

   return _state.sourceHandle;
}


//! Sets window size.
void
dmz::InputEventMouse::set_window_size (const Int32 SizeX, const Int32 SizeY) {

   _state.windowSizeX = SizeX;
   _state.windowSizeY = SizeY;
}


//! Gets window size.
void
dmz::InputEventMouse::get_window_size (Int32 &sizeX, Int32 &sizeY) const {

   sizeX = _state.windowSizeX;
   sizeY = _state.windowSizeY;
}


//! Gets the X dimension of the window.
dmz::Int32
dmz::InputEventMouse::get_window_size_x () const { return _state.windowSizeX; }


//! Gets the Y dimension of the window.
dmz::Int32
dmz::InputEventMouse::get_window_size_y () const { return _state.windowSizeY; }


/*!

\brief Sets mouse position.
\param[in] TheX Mouse's X position.
\param[in] TheY Mouse's Y position.
\note The mouse's previous position is automatically updated. The mouse position
is relative to the source object that sent the event.

*/
void
dmz::InputEventMouse::set_mouse_position (const Int32 TheX, const Int32 TheY) {

   _state.mouseXPrevious = _state.mouseX;
   _state.mouseYPrevious = _state.mouseY;
   _state.mouseX = TheX;
   _state.mouseY = TheY;
}


//! Gets mouse position.
void
dmz::InputEventMouse::get_mouse_position (Int32 &theX, Int32 &theY) const {

   theX = _state.mouseX;
   theY = _state.mouseY;
}


//! Gets mouse X position.
dmz::Int32
dmz::InputEventMouse::get_mouse_x () const { return _state.mouseX; }


//! Gets mouse Y position.
dmz::Int32
dmz::InputEventMouse::get_mouse_y () const { return _state.mouseY; }


//! Sets mouse's previous position.
void
dmz::InputEventMouse::set_previous_mouse_position (const Int32 TheX, const Int32 TheY) {

   _state.mouseXPrevious = TheX;
   _state.mouseYPrevious = TheY;
}


//! Gets mouse's previous position.
void
dmz::InputEventMouse::get_previous_mouse_position (Int32 &theX, Int32 &theY) const {

   theX = _state.mouseXPrevious;
   theY = _state.mouseYPrevious;
}


//! Gets mouse's previous X position.
dmz::Int32
dmz::InputEventMouse::get_previous_mouse_x () const { return _state.mouseXPrevious; }


//! Gets mouse's previous Y position.
dmz::Int32
dmz::InputEventMouse::get_previous_mouse_y () const { return _state.mouseYPrevious; }


//! Gets the mouse's delta X position.
dmz::Int32
dmz::InputEventMouse::get_mouse_delta_x () const {

   return _state.mouseX - _state.mouseXPrevious;
}


//! Gets the mouse's delta Y position.
dmz::Int32
dmz::InputEventMouse::get_mouse_delta_y () const {

   return _state.mouseY - _state.mouseYPrevious;
}


/*!

\brief Sets mouse screen position.
\param[in] TheX Mouse's X screen position.
\param[in] TheY Mouse's Y screnn position.
\note The mouse's previous screen position is automatically updated. The mouse position
is in screen coordinates.

*/
void
dmz::InputEventMouse::set_mouse_screen_position (const Int32 TheX, const Int32 TheY) {

   _state.screenXPrevious = _state.screenX;
   _state.screenYPrevious = _state.screenY;
   _state.screenX = TheX;
   _state.screenY = TheY;
}


//! Gets mouse screen position.
void
dmz::InputEventMouse::get_mouse_screen_position (Int32 &theX, Int32 &theY) const {

   theX = _state.screenX;
   theY = _state.screenY;
}


//! Gets mouse screen X position.
dmz::Int32
dmz::InputEventMouse::get_mouse_screen_x () const { return _state.screenX; }


//! Gets mouse screen Y position.
dmz::Int32
dmz::InputEventMouse::get_mouse_screen_y () const { return _state.screenY; }


//! Sets mouse's previous screen position.
void
dmz::InputEventMouse::set_previous_mouse_screen_position (
      const Int32 TheX,
      const Int32 TheY) {

   _state.screenXPrevious = TheX;
   _state.screenYPrevious = TheY;
}


//! Gets mouse's previous screen position.
void
dmz::InputEventMouse::get_previous_mouse_screen_position (
      Int32 &theX,
      Int32 &theY) const {

   theX = _state.screenXPrevious;
   theY = _state.screenYPrevious;
}


//! Gets mouse's previous screen X position.
dmz::Int32
dmz::InputEventMouse::get_previous_mouse_screen_x () const {

   return _state.screenXPrevious;
}


//! Gets mouse's previous screen Y position.
dmz::Int32
dmz::InputEventMouse::get_previous_mouse_screen_y () const {

   return _state.screenYPrevious;
}


//! Gets the mouse's screen delta X position.
dmz::Int32
dmz::InputEventMouse::get_mouse_screen_delta_x () const {

   return _state.screenX - _state.screenXPrevious;
}


//! Gets the mouse's screen delta Y position.
dmz::Int32
dmz::InputEventMouse::get_mouse_screen_delta_y () const {

   return _state.screenY - _state.screenYPrevious;
}


/*!

\brief Sets the current button mask.
\param[in] Value Bit mask containing the state of the mouse's buttons.
\note The previous button mask value is automatically updated.

*/
void
dmz::InputEventMouse::set_button_mask (const UInt32 Value) {

   _state.buttonMaskPrevious = _state.buttonMask;
   _state.buttonMask = Value;
}


//! Gets button mask.
dmz::UInt32
dmz::InputEventMouse::get_button_mask () const { return _state.buttonMask; }


//! Sets previous button mask.
void
dmz::InputEventMouse::set_previous_button_mask (const UInt32 Value) {

   _state.buttonMaskPrevious = Value;
}


//! Gets previous button mask.
dmz::UInt32
dmz::InputEventMouse::get_previous_button_mask () const {

   return _state.buttonMaskPrevious;
}


/*!

\brief Tests if any mouse buttons state.
\returns dmz::True if any of the mouse buttons have changed state.

*/
dmz::Boolean
dmz::InputEventMouse::have_buttons_changed () const {

   return (_state.buttonMask ^ _state.buttonMaskPrevious) != 0;
}


/*!

\brief Tests if specific mouse button is pressed.
\param[in] ButtonNumber Number of button to check.
\return Returns dmz::True if the specified button is currently presses.

*/
dmz::Boolean
dmz::InputEventMouse::is_button_pressed (const UInt32 ButtonNumber) const {

   Boolean result (False);

   if ((ButtonNumber > 0) && (ButtonNumber <= 32)) {

      const UInt32 Mask (0x01 << (ButtonNumber - 1));

      result = (_state.buttonMask & Mask) != 0;
   }

   return result;
}


/*!

\brief Test if specific mouse button has changed state.
\param[in] ButtonNumber Number of button to check.
\return Returns dmz::True if the state of the button has changed.

*/
dmz::Boolean
dmz::InputEventMouse::is_button_changed (const UInt32 ButtonNumber) const {

   Boolean result (False);

   if ((ButtonNumber > 0) && (ButtonNumber <= 32)) {

      const UInt32 ButtonMask (0x01 << (ButtonNumber - 1));

      const UInt32 CurMask (ButtonMask & _state.buttonMask);
      const UInt32 PrevMask (ButtonMask & _state.buttonMaskPrevious);

      if (PrevMask != CurMask) { result = True; }
   }

   return result;
}


//! Sets the scroll delta.
void
dmz::InputEventMouse::set_scroll_delta (const Int32 TheX, const Int32 TheY) {

   _state.scrollDeltaX = TheX;
   _state.scrollDeltaY = TheY;
}


//! Gets the scroll delta.
void
dmz::InputEventMouse::get_scroll_delta (Int32 &theX, Int32 &theY) const {

   theX = _state.scrollDeltaX;
   theY = _state.scrollDeltaY;
}


//! Gets the scroll X delta.
dmz::Int32
dmz::InputEventMouse::get_scroll_delta_x () const { return _state.scrollDeltaX; }


//! Gets the scroll Y delta.
dmz::Int32
dmz::InputEventMouse::get_scroll_delta_y () const { return _state.scrollDeltaY; }


/*!

\brief Updates mouse event.
\param[in] Value InputEventMouse containing the state of the mouse event.
\return Returns dmz::True if \a Value is different than the internal mosue event value.
\note The previous mouse event values are only updated if \a Value is different from
the current mouse event value.

*/
dmz::Boolean
dmz::InputEventMouse::update (const InputEventMouse &Value) {

   Boolean result (False);

   if ((*this != Value) || (Value.get_scroll_delta_x () != 0) ||
         (Value.get_scroll_delta_y () != 0)) {

      set_window_size (Value.get_window_size_x (), Value.get_window_size_y ());
      set_mouse_position (Value.get_mouse_x (), Value.get_mouse_y ());

      set_mouse_screen_position (
         Value.get_mouse_screen_x (),
         Value.get_mouse_screen_y ());

      set_button_mask (Value.get_button_mask ());
      set_scroll_delta (Value.get_scroll_delta_x (), Value.get_scroll_delta_y ());

      result = True;
   }

   return result;
}


dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::InputEventMouse &Event) {

   stream << dmz::endl
      << "                  Source: " << Event.get_source_handle () << dmz::endl
      << "             Window Size: "
         << Event.get_window_size_x () << ", "
         << Event.get_window_size_y () << dmz::endl
      << "                Position: "
         << Event.get_mouse_x () << ", "
         << Event.get_mouse_y () << dmz::endl
      << "       Previous Position: "
         << Event.get_previous_mouse_x () << ", "
         << Event.get_previous_mouse_y () << dmz::endl
      << "                   Delta: "
         << Event.get_mouse_delta_x () << ", "
         << Event.get_mouse_delta_y () << dmz::endl
      << "         Screen Position: "
         << Event.get_mouse_screen_x () << ", "
         << Event.get_mouse_screen_y () << dmz::endl
      << "Previous Screen Position: "
         << Event.get_previous_mouse_screen_x () << ", "
         << Event.get_previous_mouse_screen_y () << dmz::endl
      << "            Screen Delta: "
         << Event.get_mouse_screen_delta_x () << ", "
         << Event.get_mouse_screen_delta_y () << dmz::endl
      << "                  Scroll: "
         << Event.get_scroll_delta_x () << ", "
         << Event.get_scroll_delta_y () << dmz::endl
      << "                 Buttons:";

   for (dmz::UInt32 ix = 0; ix < 32; ix++) {

      if (Event.is_button_pressed (ix)) { stream << " " << ix; }
   }

   stream << dmz::endl
      << "         Changed Buttons:";

   for (dmz::UInt32 ix = 0; ix < 32; ix++) {

      if (Event.is_button_changed (ix)) { stream << " " << ix; }
   }

   stream << dmz::endl;

   return stream;
}
