#ifndef DMZ_RENDER_EVENT_HANDLER_OSG_DOT_H
#define DMZ_RENDER_EVENT_HANDLER_OSG_DOT_H

#include <dmzRenderUtilOSGExport.h>
#include <dmzInputEventKey.h>
#include <dmzInputEventMouse.h>

#include <osgGA/GUIEventHandler>


namespace dmz {

   class InputModule;

   class DMZ_RENDER_UTIL_OSG_LINK_SYMBOL RenderEventHandlerOSG :
         public osgGA::GUIEventHandler {

      public:
         RenderEventHandlerOSG ();

         virtual bool handle (
               const osgGA::GUIEventAdapter &Event, 
               osgGA::GUIActionAdapter &action,
               osg::Object *object,
               osg::NodeVisitor *visitor);
               
         void set_input_module_channels (
               InputModule *module,
               const UInt32 SourceHandle) {

            _channels = module;
            _keyEvent.set_source_handle (SourceHandle);
            _mouseEvent.set_source_handle (SourceHandle);
         }

      protected:
         ~RenderEventHandlerOSG ();
         InputModule *_channels;
         InputEventKey _keyEvent;
         InputEventMouse _mouseEvent;
         
      private:
         void __set_key_down (const osgGA::GUIEventAdapter &Event);
         void __set_key_up (const osgGA::GUIEventAdapter &Event);
         UInt32 __to_dmz_key (const UInt32 Key);
   };
};

#endif //  DMZ_RENDER_EVENT_HANDLER_OSG_DOT_H
