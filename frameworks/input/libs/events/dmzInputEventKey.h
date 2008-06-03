#ifndef DMZ_INPUT_EVENT_KEY_DOT_H
#define DMZ_INPUT_EVENT_KEY_DOT_H

#include <dmzInputEventsExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class Config;
   class Log;
   class Stream;
   class String;

   //! \addtogroup Input
   //! @{

   //! F1 key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyF1;
   //! F2 key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyF2;
   //! F3 key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyF3;
   //! F4 key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyF4;
   //! F5 key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyF5;
   //! F6 key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyF6;
   //! F7 key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyF7;
   //! F8 key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyF8;
   //! F9 key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyF9;
   //! F10 key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyF10;
   //! F11 key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyF11;
   //! F12 key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyF12;
   //! Up arrow key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyUpArrow;
   //! Down arrow key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyDownArrow;
   //! Right arrow key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyRightArrow;
   //! Left arrow key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyLeftArrow;
   //! Page up key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyPageUp;
   //! Page down key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyPageDown;
   //! Home key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyHome;
   //! End key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyEnd;
   //! Insert key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyInsert;
   //! Space key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeySpace;
   //! Escape key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyEsc;
   //! Tab key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyTab;
   //! Backspace key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyBackspace;
   //! Enter key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyEnter;
   //! Delete key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyDelete;
   //! Shift key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyShift;
   //! Control key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyControl;
   //! Meta key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyMeta;
   //! Alt key constant.
   DMZ_INPUT_EVENTS_LINK_SYMBOL extern const UInt32 KeyAlt;

   DMZ_INPUT_EVENTS_LINK_SYMBOL UInt32 string_to_key_value (
      const String &Value,
      Log *log = 0);

   DMZ_INPUT_EVENTS_LINK_SYMBOL String key_value_to_string (const UInt32 Key);

   UInt32 config_to_key_value (
      const Config &Source,
      Log *log = 0);

   UInt32 config_to_key_value (
      const String &Name,
      const Config &Source,
      Log *log = 0);

   DMZ_INPUT_EVENTS_LINK_SYMBOL UInt32 config_to_key_value (
      const String &Name,
      const Config &Source,
      const UInt32 DefaultValue,
      Log *log = 0);
   //! @}

   class DMZ_INPUT_EVENTS_LINK_SYMBOL InputEventKey {

      public:
         InputEventKey ();
         InputEventKey (const InputEventKey &Value);
         ~InputEventKey ();

         InputEventKey &operator= (const InputEventKey &Value);
         Boolean operator== (const InputEventKey &Value) const;
         Boolean operator!= (const InputEventKey &Value) const;

         void set_source_handle (const Handle SourceHandle);
         Handle get_source_handle () const;

         void set_key (const UInt32 Value);
         UInt32 get_key () const;

         Boolean update_key_state (const Boolean Value);

         void set_key_state (const Boolean Value);
         Boolean get_key_state () const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };
};

inline dmz::UInt32
dmz::config_to_key_value (const Config &Source, Log *log) {

   return config_to_key_value ("key", Source, 0, log);
}


inline dmz::UInt32
dmz::config_to_key_value (const String &Name, const Config &Source, Log *log) {

   return config_to_key_value (Name, Source, 0, log);
}

DMZ_INPUT_EVENTS_LINK_SYMBOL dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::InputEventKey &Event);

#endif //  DMZ_INPUT_EVENT_KEY_DOT_H
