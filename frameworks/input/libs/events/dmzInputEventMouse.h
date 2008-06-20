#ifndef DMZ_INPUT_EVENT_MOUSE_DOT_H
#define DMZ_INPUT_EVENT_MOUSE_DOT_H

#include <dmzInputEventsExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class Stream;

   class DMZ_INPUT_EVENTS_LINK_SYMBOL InputEventMouse {

      public:
         InputEventMouse ();
         InputEventMouse (const InputEventMouse &Value);
         ~InputEventMouse ();

         InputEventMouse &operator= (const InputEventMouse &Value);
         Boolean operator== (const InputEventMouse &Value) const;
         Boolean operator!= (const InputEventMouse &Value) const;

         void set_source_handle (const Handle SourceHandle);
         Handle get_source_handle () const;

         void set_window_size (const Int32 SizeX, const Int32 SizeY);
         void get_window_size (Int32 &sizeX, Int32 &sizeY) const;
         Int32 get_window_size_x () const;
         Int32 get_window_size_y () const;

         void set_mouse_position (const Int32 TheX, const Int32 TheY);
         void get_mouse_position (Int32 &theX, Int32 &theY) const;
         Int32 get_mouse_x () const;
         Int32 get_mouse_y () const;

         void set_previous_mouse_position (const Int32 TheX, const Int32 TheY);
         void get_previous_mouse_position (Int32 &theX, Int32 &theY) const;
         Int32 get_previous_mouse_x () const;
         Int32 get_previous_mouse_y () const;

         Int32 get_mouse_delta_x () const;
         Int32 get_mouse_delta_y () const;

         void set_mouse_screen_position (const Int32 TheX, const Int32 TheY);
         void get_mouse_screen_position (Int32 &theX, Int32 &theY) const;
         Int32 get_mouse_screen_x () const;
         Int32 get_mouse_screen_y () const;

         void set_previous_mouse_screen_position (const Int32 TheX, const Int32 TheY);
         void get_previous_mouse_screen_position (Int32 &theX, Int32 &theY) const;
         Int32 get_previous_mouse_screen_x () const;
         Int32 get_previous_mouse_screen_y () const;

         Int32 get_mouse_screen_delta_x () const;
         Int32 get_mouse_screen_delta_y () const;

         void set_button_mask (const UInt32 Value);
         UInt32 get_button_mask () const;

         void set_previous_button_mask (const UInt32 Value);
         UInt32 get_previous_button_mask () const;

         Boolean have_buttons_changed () const;
         Boolean is_button_pressed (const UInt32 ButtonNumber) const;
         Boolean is_button_changed (const UInt32 ButtonNumber) const;

         void set_scroll_delta (const Int32 TheX, const Int32 TheY);
         void get_scroll_delta (Int32 &theX, Int32 &theY) const;
         Int32 get_scroll_delta_x () const;
         Int32 get_scroll_delta_y () const;

         Boolean update (const InputEventMouse &Value);

      protected:
         struct State;
         State &_state; //!< Internal state.
   };
};

DMZ_INPUT_EVENTS_LINK_SYMBOL dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::InputEventMouse &Event);

#endif //  DMZ_INPUT_EVENT_MOUSE_DOT_H
