#include <dmzInputEventKey.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeLog.h>
#include <dmzSystemStream.h>
#include <dmzTypesString.h>

//! \cond
const dmz::UInt32 dmz::KeyF1 (257);
const dmz::UInt32 dmz::KeyF2 (258);
const dmz::UInt32 dmz::KeyF3 (259);
const dmz::UInt32 dmz::KeyF4 (260);
const dmz::UInt32 dmz::KeyF5 (261);
const dmz::UInt32 dmz::KeyF6 (262);
const dmz::UInt32 dmz::KeyF7 (263);
const dmz::UInt32 dmz::KeyF8 (264);
const dmz::UInt32 dmz::KeyF9 (265);
const dmz::UInt32 dmz::KeyF10 (266);
const dmz::UInt32 dmz::KeyF11 (267);
const dmz::UInt32 dmz::KeyF12 (268);
const dmz::UInt32 dmz::KeyUpArrow (269);
const dmz::UInt32 dmz::KeyDownArrow (270);
const dmz::UInt32 dmz::KeyRightArrow (271);
const dmz::UInt32 dmz::KeyLeftArrow (272);
const dmz::UInt32 dmz::KeyPageUp (273);
const dmz::UInt32 dmz::KeyPageDown (274);
const dmz::UInt32 dmz::KeyHome (275);
const dmz::UInt32 dmz::KeyEnd (276);
const dmz::UInt32 dmz::KeyInsert (277);
const dmz::UInt32 dmz::KeySpace (dmz::UInt32 (' '));
const dmz::UInt32 dmz::KeyEsc (27);
const dmz::UInt32 dmz::KeyTab (9);
const dmz::UInt32 dmz::KeyBackspace (8);
const dmz::UInt32 dmz::KeyEnter (13);
const dmz::UInt32 dmz::KeyDelete (127);
const dmz::UInt32 dmz::KeyShift (278);
const dmz::UInt32 dmz::KeyControl (279);
const dmz::UInt32 dmz::KeyMeta (280);
const dmz::UInt32 dmz::KeyAlt (281);
//! \endcond

/*!

\brief Converts string to key constant.
\details Function is \b not case sensitive. Single character are converted to their
corresponding key. Special keys are named as follows:
\code
        "f1" = dmz::KeyF1
        "f2" = dmz::KeyF2
        "f3" = dmz::KeyF3
        "f4" = dmz::KeyF4
        "f5" = dmz::KeyF5
        "f6" = dmz::KeyF6
        "f7" = dmz::KeyF7
        "f8" = dmz::KeyF8
        "f9" = dmz::KeyF9
       "f10" = dmz::KeyF10
       "f11" = dmz::KeyF11
       "f12" = dmz::KeyF12
   "uparrow" = dmz::KeyUpArrow
 "downarrow" = dmz::KeyDownArrow
"rightarrow" = dmz::KeyRightArrow
 "leftarrow" = dmz::KeyLeftArrow
    "pageup" = dmz::KeyPageUp
  "pagedown" = dmz::KeyPageDown
      "home" = dmz::KeyHome
       "end" = dmz::KeyEnd
    "insert" = dmz::KeyInsert
     "space" = dmz::KeySpace
       "esc" = dmz::KeyEsc
       "tab" = dmz::KeyTab
 "backspace" = dmz::KeyBackspace
     "enter" = dmz::KeyEnter
    "delete" = dmz::KeyDelete
     "shift" = dmz::KeyShift
   "control" = dmz::KeyControl
      "meta" = dmz::KeyMeta
       "alt" = dmz::KeyAlt
\endcode
\param[in] Value String containing key name.
\param[in] log Pointer to Log.
\return Returns key value. Returns zero if key name could not be mapped.

*/
dmz::UInt32
dmz::string_to_key_value (const String &Value, Log *log) {

   UInt32 result (0);

   const String Lower (Value.get_lower ());

   if (Lower == "f1") { result = KeyF1; }
   else if (Lower == "f2") { result = KeyF2; }
   else if (Lower == "f3") { result = KeyF3; }
   else if (Lower == "f4") { result = KeyF4; }
   else if (Lower == "f5") { result = KeyF5; }
   else if (Lower == "f6") { result = KeyF6; }
   else if (Lower == "f7") { result = KeyF7; }
   else if (Lower == "f8") { result = KeyF8; }
   else if (Lower == "f9") { result = KeyF9; }
   else if (Lower == "f10") { result = KeyF10; }
   else if (Lower == "f11") { result = KeyF11; }
   else if (Lower == "f12") { result = KeyF12; }
   else if (Lower == "uparrow") { result = KeyUpArrow; }
   else if (Lower == "downarrow") { result = KeyDownArrow; }
   else if (Lower == "rightarrow") { result = KeyRightArrow; }
   else if (Lower == "leftarrow") { result = KeyLeftArrow; }
   else if (Lower == "pageup") { result = KeyPageUp; }
   else if (Lower == "pagedown") { result = KeyPageDown; }
   else if (Lower == "home") { result = KeyHome; }
   else if (Lower == "end") { result = KeyEnd; }
   else if (Lower == "insert") { result = KeyInsert; }
   else if (Lower == "space") { result = KeySpace; }
   else if (Lower == "esc") { result = KeyEsc; }
   else if (Lower == "tab") { result = KeyTab; }
   else if (Lower == "backspace") { result = KeyBackspace; }
   else if (Lower == "enter") { result = KeyEnter; }
   else if (Lower == "delete") { result = KeyDelete; }
   else if (Lower == "shift") { result = KeyShift; }
   else if (Lower == "control") { result = KeyControl; }
   else if (Lower == "meta") { result = KeyMeta; }
   else if (Lower == "alt") { result = KeyAlt; }
   else if (Value.get_length () == 1) { result = UInt32 (Value.get_char (0)); }
   else if (log) { log->warn << "Unknown key type: '" << Value << "'" << endl; }

   return result;
}


/*!

\brief Converts a key value to a String.
\details The key name is returned as a lowercase string unless the key is an upper
case letter. The strings returned are the same as those supported in
dmz::string_to_key_value().
\param[in] Key Value of the key.
\return Returns String containing the key converted to a string.

*/
dmz::String
dmz::key_value_to_string (const UInt32 Key) {

   String result;

   if ((Key >= 33) && (Key <= 126)) {

      char value[2] = { char (Key), '\0' };
      result = value;
   }
   else if (Key == KeyF1) { result = "f1"; }
   else if (Key == KeyF2) { result = "f2"; }
   else if (Key == KeyF3) { result = "f3"; }
   else if (Key == KeyF4) { result = "f4"; }
   else if (Key == KeyF5) { result = "f5"; }
   else if (Key == KeyF6) { result = "f6"; }
   else if (Key == KeyF7) { result = "f7"; }
   else if (Key == KeyF8) { result = "f8"; }
   else if (Key == KeyF9) { result = "f9"; }
   else if (Key == KeyF10) { result = "f10"; }
   else if (Key == KeyF11) { result = "f11"; }
   else if (Key == KeyF12) { result = "f12"; }
   else if (Key == KeyUpArrow) { result = "uparrow"; }
   else if (Key == KeyDownArrow) { result = "downarrow"; }
   else if (Key == KeyRightArrow) { result = "rightarrow"; }
   else if (Key == KeyLeftArrow) { result = "leftarrow"; }
   else if (Key == KeyPageUp) { result = "pageup"; }
   else if (Key == KeyPageDown) { result = "pagedown"; }
   else if (Key == KeyHome) { result = "home"; }
   else if (Key == KeyEnd) { result = "end"; }
   else if (Key == KeyInsert) { result = "insert"; }
   else if (Key == KeySpace) { result = "space"; }
   else if (Key == KeyEsc) { result = "esc"; }
   else if (Key == KeyTab) { result = "tab"; }
   else if (Key == KeyBackspace) { result = "backspace"; }
   else if (Key == KeyEnter) { result = "enter"; }
   else if (Key == KeyDelete) { result = "delete"; }
   else if (Key == KeyShift) { result = "shift"; }
   else if (Key == KeyControl) { result = "control"; }
   else if (Key == KeyMeta) { result = "meta"; }
   else if (Key == KeyAlt) { result = "alt"; }
   else { result = "UNPRINTABLE"; }

   return result;
}


/*!

\fn dmz::UInt32 dmz::config_to_key_value (
const String &Name,
const Config &Source,
Log *log = 0)
\brief Converts a Config to a key constant.
\details Uses dmz::string_to_key_value() to convert the Config.
\param[in] Name String containing name of the Config.
\param[in] Source Config to be converted.
\param[in] log Pointer to the Log.
\return Returns the key value.

\fn dmz::UInt32 dmz::config_to_key_value (const Config &Source, Log *log = 0)
\brief Converts a Config to a key constant.
\details Uses dmz::string_to_key_value() to convert the Config.
\param[in] Source Config to be converted.
\param[in] log Pointer to the Log.
\return Returns the key value.

*/

/*!

\brief Converts a Config to a key constant.
\details Uses dmz::string_to_key_value() to convert the Config.
\param[in] Name String containing name of the Config.
\param[in] Source Config to be converted.
\param[in] DefaultValue Default key constant value.
\param[in] log Pointer to the Log.
\return Returns the key value.

*/
dmz::UInt32
dmz::config_to_key_value (
      const String &Name,
      const Config &Source,
      const UInt32 DefaultValue,
      Log *log) {

   const String Key = config_to_string (Name, Source);
   const UInt32 Result (Key ? string_to_key_value (Key, log) : 0);
   return Result ? Result : DefaultValue;
}


/*!

\class dmz::InputEventKey
\ingroup Input
\brief Key input event.

*/

//! \cond
struct dmz::InputEventKey::State {

   Handle sourceHandle;
   Boolean state;
   UInt32 value;

   State (const Handle TheSource,
          const Boolean TheState,
          const UInt32 TheValue) :
      sourceHandle (TheSource),
      state (TheState),
      value (TheValue) {;}

   State (const State &Value) :
      sourceHandle (0),
      state (False),
      value (0) { *this = Value; }

   State &operator= (const State &Value) {

      sourceHandle = Value.sourceHandle;
      state = Value.state;
      value = Value.value;

      return *this;
   }

   Boolean operator== (const State &Value) const {

      return (state == Value.state) &&
         (value == Value.value) &&
         (sourceHandle == Value.sourceHandle);
   }

   private:
      State ();
};
//! \endcond


//! Default constructor.
dmz::InputEventKey::InputEventKey () :
      _state (*(new State (0, False, 0))) {;}


//! Copy constructor.
dmz::InputEventKey::InputEventKey (const InputEventKey &Value) :
      _state (*(new State (Value._state))) {;}


//! Destructor.
dmz::InputEventKey::~InputEventKey () { delete &_state; }


//! Assignment operator.
dmz::InputEventKey &
dmz::InputEventKey::operator= (const InputEventKey &Value) {

   _state = Value._state;
   return *this;
}


//! Relational "equal to" operator.
dmz::Boolean
dmz::InputEventKey::operator== (const InputEventKey &Value) const {

   return _state == Value._state;
}


//! Relational "not equal to" operator.
dmz::Boolean
dmz::InputEventKey::operator!= (const InputEventKey &Value) const {

   return !(_state == Value._state);
}


//! Sets source's Handle.
void
dmz::InputEventKey::set_source_handle (const Handle SourceHandle) {

   _state.sourceHandle = SourceHandle;
}


//! Gets source's Handle.
dmz::Handle
dmz::InputEventKey::get_source_handle () const {

   return _state.sourceHandle;
}


//! Sets key.
void
dmz::InputEventKey::set_key (const UInt32 Value) { _state.value = Value; }


//! Gets key.
dmz::UInt32
dmz::InputEventKey::get_key () const { return _state.value; }


/*!

\brief Updates key state.
\param[in] Value Key state.
\return Returns dmz::True if the new state is different from the current state.

*/
dmz::Boolean
dmz::InputEventKey::update_key_state (const Boolean Value) {

   Boolean result (False);

   if (Value != _state.state) {

      _state.value = Value;
      result = True;
   }

   return result;
}


//! Sets key state.
void
dmz::InputEventKey::set_key_state (const Boolean Value) {

   _state.state = Value;
}


//! Gets key state.
dmz::Boolean
dmz::InputEventKey::get_key_state () const { return _state.state; }


dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::InputEventKey &Event) {

   const dmz::UInt32 Key (Event.get_key ());

   stream << dmz::endl
      << "Source: " << Event.get_source_handle () << dmz::endl
      << "   Key: " << dmz::key_value_to_string (Key) << "[" << Key << "]" << dmz::endl
      << " State: " << (Event.get_key_state () ? "Pressed" : "Released") << dmz::endl;

   return stream;
}

