#include <dmzRenderConsts.h>
#include <dmzRenderModuleCoreOSG.h>
#include "dmzRenderPluginScreenCaptureOSG.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <osgViewer/Viewer>
#include <osgDB/WriteFile>

namespace {

class WriteImage : public osgViewer::ScreenCaptureHandler::CaptureOperation {

   public:
      WriteImage (const dmz::String &FileName, dmz::Log &log);
      virtual void operator()(const osg::Image& image, const unsigned int context_id);

   protected:
      const dmz::String _FileName;
      dmz::Log &_log;
};

};


WriteImage::WriteImage (const dmz::String &FileName, dmz::Log &log) :
      _FileName (FileName),
      _log (log) {;}


void
WriteImage::operator()(const osg::Image& image, const unsigned int context_id) {

   if (_FileName) {

      osgDB::writeImageFile(image, _FileName.get_buffer ());
   }
   else {

      _log.error << "Failed to create screen capture. No file name specified."
         << dmz::endl;
   }
}



dmz::RenderPluginScreenCaptureOSG::RenderPluginScreenCaptureOSG (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      MessageObserver (Info),
      _log (Info),
      _convert (Info),
      _core (0) {

   _init (local);
}


dmz::RenderPluginScreenCaptureOSG::~RenderPluginScreenCaptureOSG () {

}


// Plugin Interface
void
dmz::RenderPluginScreenCaptureOSG::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::RenderPluginScreenCaptureOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) { _core = RenderModuleCoreOSG::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOSG::cast (PluginPtr))) { _core = 0; }
   }
}


// TimeSlice Interface
void
dmz::RenderPluginScreenCaptureOSG::update_time_slice (const Float64 TimeDelta) {

   if (_fileName) {

      if (_core) {

         osgViewer::ViewerBase *viewer = _core->lookup_viewer (RenderMainPortalName);

         if (viewer) {

            // A new handler is created before each usage because if the same
            // handler is used more than once, each subsequent image capture is
            // corrupt.
            _handler = new osgViewer::ScreenCaptureHandler;

            if (_handler.valid ()) {

               _handler->setKeyEventTakeScreenShot (0);
               osg::ref_ptr<WriteImage> writer = new WriteImage (_fileName, _log);
               _handler->setCaptureOperation (writer.get ());
               _handler->captureNextFrame (*viewer);
            }
         }
      }

      _fileName.flush ();
   }
}


// Message Observer Interface
void
dmz::RenderPluginScreenCaptureOSG::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   if (Type == _screenCaptureMsg) { _fileName = _convert.to_string (InData);
}
}


void
dmz::RenderPluginScreenCaptureOSG::_init (Config &local) {

   RuntimeContext *context = get_plugin_runtime_context ();

   _screenCaptureMsg = config_create_message (
      "screen-capture-message.name",
      local,
      "Capture_Render_Screen_Message",
      context);

   subscribe_to_message (_screenCaptureMsg);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginScreenCaptureOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginScreenCaptureOSG (Info, local);
}

};
