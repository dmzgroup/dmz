#include <dmzArchiveModule.h>
#include <dmzQtModuleCanvas.h>
#include <dmzQtModuleMainWindow.h>
#include "dmzQtPluginCanvasBackground.h"
#include <dmzQtUtil.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzSystemFile.h>
#include <dmzSystemStreamString.h>
#include <dmzXMLBase64.h>
#include <QtGui/QtGui>


dmz::QtPluginCanvasBackground::QtPluginCanvasBackground (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ArchiveObserverUtil (Info, local),
      MessageObserver (Info),
      _log (Info),
      _appState (Info),
      _canvasModule (0),
      _canvasModuleName (),
      _mainWindowModule (0),
      _mainWindowModuleName (),
      _backgroundEditMessage (),
      _bgItem (0),
      _bgConfig ("image"),
      _data ("") {

   _bgConfig.add_config (_data);
   _data.set_formatted (True);
         
   _init (local);
}


dmz::QtPluginCanvasBackground::~QtPluginCanvasBackground () {

}


// Plugin Interface
void
dmz::QtPluginCanvasBackground::update_plugin_state (
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
dmz::QtPluginCanvasBackground::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_canvasModule) {

         _canvasModule = QtModuleCanvas::cast (PluginPtr, _canvasModuleName);
      }
      
      if (!_mainWindowModule) {

         _mainWindowModule = QtModuleMainWindow::cast (PluginPtr, _mainWindowModuleName);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_canvasModule && (_canvasModule == QtModuleCanvas::cast (PluginPtr))) {

         _canvasModule = 0;
      }
      
      if (_mainWindowModule &&
          (_mainWindowModule == QtModuleMainWindow::cast (PluginPtr))) {

         _mainWindowModule = 0;
      }
   }
}


// ArchiveObserver Interface.
void
dmz::QtPluginCanvasBackground::create_archive (
      const Handle ArchiveHandle,
      Config &local,
      Config &global) {

   if (_bgItem) {
      
      local.add_config (_bgConfig);
   }
}

#include <dmzSystem.h>


void
dmz::QtPluginCanvasBackground::process_archive (
      const Handle ArchiveHandle,
      Config &local,
      Config &global) {

   Config imageData;
   
   if (local.lookup_config ("image", imageData)) {

      String encodedValue;
      
      if (imageData.get_value (encodedValue)) {
         
         String decodedValue;
         decode_base64 (encodedValue, decodedValue);
         
         QPixmap pixmap;
         
         Int32 size (0);
         const uchar *buffer = (uchar *)decodedValue.get_buffer (size);
         
         if (pixmap.loadFromData (buffer, size)) {
            
            _load_pixmap (pixmap);
         }
      }
   }
}


// Message Observer Interface
void
dmz::QtPluginCanvasBackground::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   if (Type == _backgroundEditMessage) {

      QMessageBox msgBox;
      QPushButton *loadButton = msgBox.addButton ("1) Load...", QMessageBox::ActionRole);
      QPushButton *clearButton = msgBox.addButton ("2) Clear", QMessageBox::DestructiveRole);
      QPushButton *cancelButton = msgBox.addButton ("Cancel", QMessageBox::RejectRole);

      msgBox.setIcon (QMessageBox::Question);
      msgBox.setText ("Background Image");
      msgBox.setInformativeText("1) Load new background image.\n2) Clear background image.");
      msgBox.exec();

      if (msgBox.clickedButton() == loadButton) {
   
         _load_background ();
      }
      else if (msgBox.clickedButton() == clearButton) {
    
         _clear_background ();
      }
   }
}


QString
dmz::QtPluginCanvasBackground::_get_last_path () {
   
   String lastPath (_appState.get_default_directory ());

   // if (is_valid_path (lastPath)) {
   // 
   //    if (is_directory (lastPath)) {
   // 
   //       lastPath << "/" << _defaultExportName << "." << _suffix;
   //    }
   // }
   // else {
   // 
   //    lastPath.flush () << get_home_directory () << "/" << _defaultExportName << "."
   //       << _suffix;
   // }

   QFileInfo fi (lastPath.get_buffer ());

   return fi.absoluteFilePath ();
}


void
dmz::QtPluginCanvasBackground::_load_background () {

   QString fileName =
      QFileDialog::getOpenFileName (
         _mainWindowModule ? _mainWindowModule->get_widget () : 0,
         "Select Image File",
         _get_last_path (),
         "Images (*.png *.jpg)");
         
   if (!fileName.isEmpty ()) {
      
      qApp->setOverrideCursor (QCursor (Qt::BusyCursor));
      
      QPixmap bg (fileName);
      
      _load_pixmap (bg);
      
      _canvasModule->zoom_extents ();
      
      _encode_image (qPrintable (fileName));
      
      qApp->restoreOverrideCursor ();
   }
}


void
dmz::QtPluginCanvasBackground::_load_pixmap (const QPixmap &Pixmap) {

   if (_canvasModule) {

      QGraphicsScene *scene = _canvasModule->get_scene ();

      if (scene) {

         if (!_bgItem) {
            
            _bgItem = scene->addPixmap (Pixmap);
            _bgItem->setZValue (-1000);
         }
         else {
            
            _bgItem->setPixmap (Pixmap);
         }
      }
   }
}


void
dmz::QtPluginCanvasBackground::_clear_background () {

   if (_canvasModule && _bgItem) {
      
      QGraphicsScene *scene = _canvasModule->get_scene ();
      
      if (scene) {
         
         scene->removeItem (_bgItem);
         
         delete _bgItem;
         _bgItem = 0;
      }
   }
}


void
dmz::QtPluginCanvasBackground::_encode_image (const String &FileName) {

   if (FileName) {

      _data.set_value ("");

      FILE *file = open_file (FileName, "rb");

      if (file) {

         const UInt32 RawBufferSize (1024);
         const UInt64 Base64BufferSize (get_file_size (FileName) * 2);

         String rawBuffer (0, 0, RawBufferSize + 1);
         String base64Buffer (0, 0, Base64BufferSize + 1);
         
         StreamString stream (base64Buffer);
         
         Base64Encoder encoder;
         encoder.set_stream (&stream);
         encoder.set_line_length (120);

         while (read_file (file, RawBufferSize, rawBuffer)) {

            Int32 size (0);
            const char *buffer = rawBuffer.get_buffer (size);

            encoder.encode (buffer, size);
         }

         encoder.encode (0, 0);
         
         _data.append_value (base64Buffer, False);
      }
   }
}


void
dmz::QtPluginCanvasBackground::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   _mainWindowModuleName = config_to_string ("module.mainWindow.name", local);
   _canvasModuleName = config_to_string ("module.canvas.name", local);
   
   init_archive (local);
   
   _backgroundEditMessage = config_create_message_type (
      "message.backgroundEdit.name",
      local,
      "CanvasBackgroundEditMessage",
      context,
      &_log);
      
   subscribe_to_message (_backgroundEditMessage);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginCanvasBackground (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginCanvasBackground (Info, local);
}

};
