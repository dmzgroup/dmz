#include "dmzQtPluginRenderScreenCapture.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <dmzSystem.h>
#include <dmzSystemFile.h>
#include <dmzTypesUUID.h>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtGui/QFileDialog>

dmz::QtScreenCaptureSave::QtScreenCaptureSave (
      const QString &FileName,
      const QString &TmpName,
      Log &log) :
      _FileName (FileName),
      _TmpName (TmpName),
      _log (log),
      _qsave (0),
      _done (False),
      _watchForFile (True) {

   _ui.setupUi (this);
}


dmz::QtScreenCaptureSave::~QtScreenCaptureSave () {

   if (QFile::exists (_TmpName)) { QFile::remove (_TmpName); }
}


dmz::Boolean
dmz::QtScreenCaptureSave::update () {

   Boolean result (True);

   if (_watchForFile) {

      if (QFile::exists (_TmpName)) {

         _watchForFile = False;

         QString title (windowTitle ());
         title += ": ";
         QPixmap img (_TmpName);
         title += QString::number (img.width ());
         title += "x";
         title += QString::number (img.height ());

         if (img.width () > 800) {

            img = img.scaledToWidth (800, Qt::SmoothTransformation);
         }

         if (img.height () > 600) {

            img = img.scaledToHeight (600, Qt::SmoothTransformation);
         }

         _ui.imgLabel->setPixmap (img);
         setWindowTitle (title);
         adjustSize ();
         show ();
      }
      else if (_TmpName.isEmpty ()) { _watchForFile = False; _done = True; }
   }
   else if (_done) { result = False; }

   return result;
}


QString
dmz::QtScreenCaptureSave::get_file_name () { return _savedFileName; }


void
dmz::QtScreenCaptureSave::closeEvent (QCloseEvent *) {

   _done = True;
}


void
dmz::QtScreenCaptureSave::do_accepted () {

   QStringList list = _qsave->selectedFiles ();

   if (!list.isEmpty ()) {

      QString file = list.first ();

      if (!file.isEmpty ()) {

         QFile::rename (_TmpName, file);
         _savedFileName = file;
      }
   }

   close ();
}


void
dmz::QtScreenCaptureSave::do_rejected () {

   close ();
}

void
dmz::QtScreenCaptureSave::on_buttonBox_accepted () {

   _qsave = new QFileDialog (this, "Save Screen Capture");
   _qsave->setAcceptMode (QFileDialog::AcceptSave);
   QStringList filters;
   _qsave->selectFile (_FileName);
   connect (_qsave, SIGNAL (accepted ()), this, SLOT (do_accepted ()));
   connect (_qsave, SIGNAL (rejected ()), this, SLOT (do_rejected ()));
   _qsave->open ();
}


void
dmz::QtScreenCaptureSave::on_buttonBox_rejected () {

   close ();
}


dmz::QtPluginRenderScreenCapture::QtPluginRenderScreenCapture (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      MessageObserver (Info),
      _log (Info),
      _convert (Info),
      _fileExt (".png") {

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

      Config session (get_plugin_name ());

      session.store_attribute ("save-path.value", _savePath );

      set_session_config (get_plugin_runtime_context (), session);

      _saveTable.empty ();
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

   HashTableStringIterator it;
   QtScreenCaptureSave *save (0);

   while (_saveTable.get_next (it, save)) {

      if (!save->update ()) {

         String file = qPrintable (save->get_file_name ());

         if (file) {

            String path, f, e;

            split_path_file_ext (file, path, f, e);
            _savePath = path;
         }
         if (_saveTable.remove (it.get_hash_key ()) == save) { delete save; save = 0; }
      }
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

      String fileName = qPrintable (QDir::tempPath ());
      UUID fileUUID;
      create_uuid (fileUUID);
      fileName << "/" << fileUUID.to_string (UUID::NotFormatted) << _fileExt;
      Data out = _convert.to_data (fileName);
      _screenCaptureMsg.send (&out);

      String target = _find_target_name ();

      QtScreenCaptureSave *save = new QtScreenCaptureSave (
         target.get_buffer (),
         fileName.get_buffer (),
         _log);

      if (!_saveTable.store (fileName, save)) { delete save; save = 0; }
   }
}


dmz::String
dmz::QtPluginRenderScreenCapture::_find_target_name () {

   String result;

   Boolean done = False;
   Int32 count = 1;

   while (!done) {

      String test = _savePath + _fileRoot + "-" + String::number (count) + _fileExt;

      if (!is_valid_path (test)) { done = True; result = test; }
      else { count++; }
   }

   if (!result) { result = _savePath + _fileRoot + _fileExt; }

   return result;
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

   _fileRoot = config_to_string ("file-root.name", local, "DMZ-Screen-Capture");

   Config session (get_session_config (get_plugin_name (), context));

   _savePath = config_to_string ("save-path.value", session);
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
