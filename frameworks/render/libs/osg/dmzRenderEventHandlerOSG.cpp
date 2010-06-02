#include <dmzInputEventKey.h>
#include <dmzInputEventMouse.h>
#include <dmzInputModule.h>
#include <dmzRenderEventHandlerOSG.h>
#include <dmzRenderPortalSize.h>
#include <dmzRenderUtilOSG.h>
#include <dmzRuntimeExit.h>


struct dmz::RenderEventHandlerOSG::State {

   Exit exit;
   InputModule *channels;
   InputEventKey keyEvent;
   InputEventMouse mouseEvent;
   PortalSize size;

   State (RuntimeContext *context, Config &init) :
         exit (context),
         channels (0),
         size (context, init) {;}
};


dmz::RenderEventHandlerOSG::RenderEventHandlerOSG (
      RuntimeContext *context,
      Config &init) :
      _state (*(new State (context, init))) {;}


dmz::RenderEventHandlerOSG::~RenderEventHandlerOSG () { delete &_state; }


bool
dmz::RenderEventHandlerOSG::handle (
      const osgGA::GUIEventAdapter &Event,
      osgGA::GUIActionAdapter &acion,
      osg::Object *object,
      osg::NodeVisitor *visitor) {

   Boolean result (false);

   if (_state.channels) {

      switch (Event.getEventType ()) {

         case (osgGA::GUIEventAdapter::KEYDOWN): {

            _state.keyEvent.set_key_state (true);
            _state.keyEvent.set_key (convert_osg_key_to_dmz_key (Event.getKey ()));
            _state.channels->send_key_event (_state.keyEvent);
            result = true;
            break;
         }

         case (osgGA::GUIEventAdapter::KEYUP): {

            _state.keyEvent.set_key_state (false);
            _state.keyEvent.set_key (convert_osg_key_to_dmz_key (Event.getKey ()));
            _state.channels->send_key_event (_state.keyEvent);
            result = true;
            break;
         }

         case (osgGA::GUIEventAdapter::PUSH):
         case (osgGA::GUIEventAdapter::RELEASE):
         case (osgGA::GUIEventAdapter::DOUBLECLICK):
         case (osgGA::GUIEventAdapter::RESIZE):
         case (osgGA::GUIEventAdapter::MOVE):
         case (osgGA::GUIEventAdapter::DRAG):
         case (osgGA::GUIEventAdapter::SCROLL): {

            _state.mouseEvent.set_button_mask (Event.getButtonMask ());

            const Int32 Width = Event.getWindowWidth ();
            const Int32 Height = Event.getWindowHeight ();

            _state.size.set_size (Width, Height);

            _state.mouseEvent.set_window_size (Width, Height);

            _state.mouseEvent.set_mouse_position (
               Event.getX (),
               Height - Event.getY ());

            osgGA::GUIEventAdapter::ScrollingMotion scrollMotion =
               Event.getScrollingMotion ();
            
            UInt32 deltaX (0);
            UInt32 deltaY (0);
            
            switch (scrollMotion) {
            
               case osgGA::GUIEventAdapter::SCROLL_LEFT: deltaX = -120; break;

               case osgGA::GUIEventAdapter::SCROLL_RIGHT: deltaX = 120; break;

               case osgGA::GUIEventAdapter::SCROLL_UP: deltaY = 120; break;

               case osgGA::GUIEventAdapter::SCROLL_DOWN: deltaY = -120; break;

               case osgGA::GUIEventAdapter::SCROLL_2D:
                  deltaX = Event.getScrollingDeltaX ();
                  deltaY = Event.getScrollingDeltaY ();
                  break;

               default: break;
            }
            
            _state.mouseEvent.set_scroll_delta (deltaX, deltaY);

            _state.channels->send_mouse_event (_state.mouseEvent);

            result = true;
            break;
         }

         case (osgGA::GUIEventAdapter::CLOSE_WINDOW): {

            _state.exit.request_exit (ExitStatusNormal, "Application Quit.");
            break;
         }

         case (osgGA::GUIEventAdapter::QUIT_APPLICATION):  {

            _state.exit.request_exit (ExitStatusNormal, "Application Quit.");
            break;
         }

         default:
            break;
      }
   }

   return result;
}


void
dmz::RenderEventHandlerOSG::set_input_module_channels (
      InputModule *module,
      const UInt32 SourceHandle) {

   _state.channels = module;
   _state.keyEvent.set_source_handle (SourceHandle);
   _state.mouseEvent.set_source_handle (SourceHandle);
}


void
dmz::RenderEventHandlerOSG::set_portal_size (const Int32 TheX, const Int32 TheY) {

   _state.size.set_size (TheX, TheY);
}
