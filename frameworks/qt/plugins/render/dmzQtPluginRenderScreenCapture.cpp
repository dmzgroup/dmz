#include "dmzQtPluginRenderScreenCapture.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzSystem.h>
#include <dmzSystemFile.h>
#include <dmzTypesUUID.h>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtGui/QFileDialog>

dmz::QtPluginRenderScreenCapture::QtPluginRenderScreenCapture (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      MessageObserver (Info),
      _log (Info),
      _convert (Info),
      _watchForFile (False),
      _saveFile (False),
      _fileExt (".png") {

   _ui.setupUi (this);
   close ();

   _init (local);
}


dmz::QtPluginRenderScreenCapture::~QtPluginRenderScreenCapture () {

}


// Plugin Interface
void
dmz::QtPluginRenderScreenCapture::update_plugin_state (
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
dmz::QtPluginRenderScreenCapture::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// TimeSlice Interface
void
dmz::QtPluginRenderScreenCapture::update_time_slice (const Float64 TimeDelta) {

   if (_watchForFile) {

      if (is_valid_path (_fileName)) {

         _watchForFile = False;

         QPixmap img (_fileName.get_buffer ());
         _ui.imgLabel->setPixmap (img);
         adjustSize ();
         show ();
         raise ();
      }
      else if (!_fileName) { _watchForFile = False; }
   }
}


// Message Observer Interface
void
dmz::QtPluginRenderScreenCapture::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   if (Type == _startCaptureMsg) {

      _fileName = qPrintable (QDir::tempPath ());
      UUID fileUUID;
      create_uuid (fileUUID);
      _fileName << "/" << fileUUID.to_string (UUID::NotFormatted) << _fileExt;
      Data out = _convert.to_data (_fileName);
      _screenCaptureMsg.send (&out);
      _watchForFile = True;
   }
}

void
dmz::QtPluginRenderScreenCapture::closeEvent (QCloseEvent *) {

   if (_saveFile) {

      QString file = QFileDialog::getSaveFileName (
         this,
         "Save Screen Capture");

      String p, f, ext;

      split_path_file_ext (qPrintable (file), p, f, ext);

      if (ext != _fileExt) { file += _fileExt.get_buffer (); }

      if (!file.isEmpty ()) {

         QFile::rename (_fileName.get_buffer (), file);
      }
         
      _saveFile = False;
   }

   if (is_valid_path (_fileName)) {

      QFile::remove (_fileName.get_buffer ());
   }

   _fileName.flush ();
}


void
dmz::QtPluginRenderScreenCapture::on_buttonBox_accepted () {

   _saveFile = True;
   close ();
}


void
dmz::QtPluginRenderScreenCapture::on_buttonBox_rejected () {

   _saveFile = False;
   close ();
}


void
dmz::QtPluginRenderScreenCapture::_init (Config &local) {

   RuntimeContext *context = get_plugin_runtime_context ();

   _startCaptureMsg = config_create_message (
      "start-screen-capture-message.name",
      local,
      "Start_Render_Screen_Capture_Message",
      context);

   subscribe_to_message (_startCaptureMsg);

   _screenCaptureMsg = config_create_message (
      "screen-capture-message.name",
      local,
      "Capture_Render_Screen_Message",
      context);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginRenderScreenCapture (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginRenderScreenCapture (Info, local);
}

};
