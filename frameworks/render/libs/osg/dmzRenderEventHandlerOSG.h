#ifndef DMZ_RENDER_EVENT_HANDLER_OSG_DOT_H
#define DMZ_RENDER_EVENT_HANDLER_OSG_DOT_H

#include <dmzRenderUtilOSGExport.h>
#include <dmzTypesBase.h>

#include <osgGA/GUIEventHandler>

namespace dmz {

   class Config;
   class RuntimeContext;
   class InputModule;

   class DMZ_RENDER_UTIL_OSG_LINK_SYMBOL RenderEventHandlerOSG :
         public osgGA::GUIEventHandler {

      public:
         RenderEventHandlerOSG (RuntimeContext *context, Config &init);

         virtual bool handle (
               const osgGA::GUIEventAdapter &Event,
               osgGA::GUIActionAdapter &action,
               osg::Object *object,
               osg::NodeVisitor *visitor);

         void set_input_module_channels (
               InputModule *module,
               const UInt32 SourceHandle);

         void set_portal_size (const Int32 TheX, const Int32 TheY);

      protected:
         struct State;
         State &_state;

         ~RenderEventHandlerOSG ();
   };
};

#endif //  DMZ_RENDER_EVENT_HANDLER_OSG_DOT_H
