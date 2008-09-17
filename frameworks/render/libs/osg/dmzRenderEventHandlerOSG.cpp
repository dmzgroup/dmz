#include <dmzInputModule.h>
#include <dmzRenderEventHandlerOSG.h>

dmz::RenderEventHandlerOSG::RenderEventHandlerOSG () :
      osgGA::GUIEventHandler (),
      _channels (0),
      _keyEvent (),
      _mouseEvent () {;}


dmz::RenderEventHandlerOSG::~RenderEventHandlerOSG () {;}


bool
dmz::RenderEventHandlerOSG::handle (
      const osgGA::GUIEventAdapter &Event,
      osgGA::GUIActionAdapter &acion,
      osg::Object *object,
      osg::NodeVisitor *visitor) {

   Boolean result (false);

   if (_channels) {

      switch (Event.getEventType ()) {

         case (osgGA::GUIEventAdapter::KEYDOWN): {

            __set_key_down (Event);
            _channels->send_key_event (_keyEvent);
            result = true;
            break;
         }

         case (osgGA::GUIEventAdapter::KEYUP): {

            __set_key_up (Event);
            _channels->send_key_event (_keyEvent);
            result = true;
            break;
         }

         case (osgGA::GUIEventAdapter::PUSH):
         case (osgGA::GUIEventAdapter::RELEASE):
         case (osgGA::GUIEventAdapter::DOUBLECLICK): {

            _mouseEvent.set_button_mask (Event.getButtonMask ());
            _channels->send_mouse_event (_mouseEvent);
            result = true;
            break;
         }

         case (osgGA::GUIEventAdapter::RESIZE): {

            _mouseEvent.set_window_size (
               Event.getWindowWidth (),
               Event.getWindowHeight ());

            _channels->send_mouse_event (_mouseEvent);
            result = true;
            break;
         }

         case (osgGA::GUIEventAdapter::MOVE): {

            _mouseEvent.set_mouse_position (Event.getX (), Event.getY ());
            _channels->send_mouse_event (_mouseEvent);
            result = true;
            break;
         }

         case (osgGA::GUIEventAdapter::SCROLL): {

            _mouseEvent.set_mouse_position (
               Event.getScrollingDeltaX (),
               Event.getScrollingDeltaY ());

            _channels->send_mouse_event (_mouseEvent);
            result = true;
            break;
         }

         case (osgGA::GUIEventAdapter::CLOSE_WINDOW): {

            // this should be passed in in some way
            break;
         }

         default:
            break;
      }
   }

   return result;
}


void
dmz::RenderEventHandlerOSG::__set_key_down (const osgGA::GUIEventAdapter &Event) {

   _keyEvent.set_key_state (true);
   _keyEvent.set_key (__to_dmz_key (Event.getKey ()));
}


void
dmz::RenderEventHandlerOSG::__set_key_up (const osgGA::GUIEventAdapter &Event) {

   _keyEvent.set_key_state (false);
   _keyEvent.set_key (__to_dmz_key (Event.getKey ()));
}


dmz::UInt32
dmz::RenderEventHandlerOSG::__to_dmz_key (const dmz::UInt32 Key) {

   UInt32 result (0);

   switch (Key) {
      case osgGA::GUIEventAdapter::KEY_Escape: { result = KeyEsc; } break;
      case osgGA::GUIEventAdapter::KEY_Up: { result = KeyUpArrow; } break;
      case osgGA::GUIEventAdapter::KEY_Down: { result = KeyDownArrow; } break;
      case osgGA::GUIEventAdapter::KEY_Left: { result = KeyLeftArrow; } break;
      case osgGA::GUIEventAdapter::KEY_Right: { result = KeyRightArrow; } break;
      case osgGA::GUIEventAdapter::KEY_Page_Up: { result = KeyPageUp; } break;
      case osgGA::GUIEventAdapter::KEY_Page_Down: {
         result = KeyPageDown; } break;
      case osgGA::GUIEventAdapter::KEY_Home: { result = KeyHome; } break;
      case osgGA::GUIEventAdapter::KEY_End: { result = KeyEnd; } break;
      case osgGA::GUIEventAdapter::KEY_Insert: { result = KeyInsert; } break;
      case osgGA::GUIEventAdapter::KEY_Delete: { result = KeyDelete; } break;
      case osgGA::GUIEventAdapter::KEY_Space: { result = KeySpace; } break;
      case osgGA::GUIEventAdapter::KEY_BackSpace: {
         result = KeyBackspace; } break;
      case osgGA::GUIEventAdapter::KEY_Tab: { result = KeyTab; } break;
      case osgGA::GUIEventAdapter::KEY_Return: { result = KeyEnter; } break;
      case osgGA::GUIEventAdapter::KEY_Shift_L:
      case osgGA::GUIEventAdapter::KEY_Shift_R: {
         result = KeyShift; } break;
      case osgGA::GUIEventAdapter::KEY_Control_L:
      case osgGA::GUIEventAdapter::KEY_Control_R: {
         result = KeyControl; } break;
      case osgGA::GUIEventAdapter::KEY_Meta_L:
      case osgGA::GUIEventAdapter::KEY_Meta_R: { result = KeyMeta; } break;
      case osgGA::GUIEventAdapter::KEY_Alt_L:
      case osgGA::GUIEventAdapter::KEY_Alt_R: { result = KeyAlt; } break;
      case osgGA::GUIEventAdapter::KEY_F1: { result = KeyF1; } break;
      case osgGA::GUIEventAdapter::KEY_F2: { result = KeyF2; } break;
      case osgGA::GUIEventAdapter::KEY_F3: { result = KeyF3; } break;
      case osgGA::GUIEventAdapter::KEY_F4: { result = KeyF4; } break;
      case osgGA::GUIEventAdapter::KEY_F5: { result = KeyF5; } break;
      case osgGA::GUIEventAdapter::KEY_F6: { result = KeyF6; } break;
      case osgGA::GUIEventAdapter::KEY_F7: { result = KeyF7; } break;
      case osgGA::GUIEventAdapter::KEY_F8: { result = KeyF8; } break;
      case osgGA::GUIEventAdapter::KEY_F9: { result = KeyF9; } break;
      case osgGA::GUIEventAdapter::KEY_F10: { result = KeyF10; } break;
      case osgGA::GUIEventAdapter::KEY_F11: { result = KeyF11; } break;
      case osgGA::GUIEventAdapter::KEY_F12: { result = KeyF12; } break;
      default: { result = Key; } break;
   }
#if 0
   // some other OSG keys
      case osgGA::GUIEventAdapter::KEY_Linefeed: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Clear: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Pause: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Scroll_Lock: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Sys_Req: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Prior: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Next: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Begin: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Select: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Print: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Execute: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Undo: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Redo: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Menu: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Find: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Cancel: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Help: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Break: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Mode_switch: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Script_switch: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Num_Lock: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Space: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Tab: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Enter: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_F1: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_F2: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_F3: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_F4: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Home: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Left: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Up: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Right: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Down: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Prior: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Page_Up: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Next: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Page_Down: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_End: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Begin: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Insert: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Delete: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Equal: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Multiply: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Add: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Separator: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Subtract: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Decimal: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_Divide: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_0: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_1: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_2: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_3: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_4: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_5: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_6: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_7: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_8: { result = } break;
      case osgGA::GUIEventAdapter::KEY_KP_9: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Caps_Lock: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Shift_Lock: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Super_L: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Super_R: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Hyper_L: { result = } break;
      case osgGA::GUIEventAdapter::KEY_Hyper_R: { result = } break;
#endif

   return result;
}

