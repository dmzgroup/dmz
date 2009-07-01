#include <dmzArchiveModule.h>
#include <dmzQtModuleMap.h>
#include <dmzQtModuleMainWindow.h>
#include "dmzQtPluginMapProperties.h"
#include <dmzQtUtil.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzSystemFile.h>
#include <dmzSystemStreamString.h>
#include <dmzXMLBase64.h>
#include <QtGui/QtGui>


dmz::QtPluginMapProperties::QtPluginMapProperties (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ArchiveObserverUtil (Info, local),
      MessageObserver (Info),
      _log (Info),
      _undo (Info),
      _dataConverter (Info.get_context ()),
      _appState (Info),
      _mapModule (0),
      _mapModuleName (),
      _mainWindowModule (0),
      _mainWindowModuleName (),
      _cleanupMessage (),
      _backgroundEditMessage (),
      _undoMessage (),
      _imageFile (),
      _bgItem (0),
      _bgConfig ("image"),
      _data ("") {

   _bgConfig.add_config (_data);
   _data.set_formatted (True);
         
   _init (local);
}


dmz::QtPluginMapProperties::~QtPluginMapProperties () {

}


// Plugin Interface
void
dmz::QtPluginMapProperties::update_plugin_state (
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
dmz::QtPluginMapProperties::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_mapModule) {

         _mapModule = QtModuleMap::cast (PluginPtr, _mapModuleName);
      }
      
      if (!_mainWindowModule) {

         _mainWindowModule = QtModuleMainWindow::cast (PluginPtr, _mainWindowModuleName);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_mapModule && (_mapModule == QtModuleMap::cast (PluginPtr))) {

         _mapModule = 0;
      }
      
      if (_mainWindowModule &&
          (_mainWindowModule == QtModuleMainWindow::cast (PluginPtr))) {

         _mainWindowModule = 0;
      }
   }
}


// ArchiveObserver Interface.
void
dmz::QtPluginMapProperties::create_archive (
      const Handle ArchiveHandle,
      Config &local,
      Config &global) {

   if (_bgItem) {
      
      local.add_config (_bgConfig);
   }
}


void
dmz::QtPluginMapProperties::process_archive (
      const Handle ArchiveHandle,
      Config &local,
      Config &global) {

   Config imageData;

   String fileName (config_to_string ("image.file", local));

   if (local.lookup_config ("image", imageData)) {

      String encodedValue;
      
      if (imageData.get_value (encodedValue)) {
         
         String decodedValue;
         decode_base64 (encodedValue, decodedValue);
         
         QPixmap pixmap;
         
         Int32 size (0);
         const uchar *buffer = (uchar *)decodedValue.get_buffer (size);
         
         if (pixmap.loadFromData (buffer, size)) {
            
            // save the data so it can be put in the next archive -ss
            _data.set_value (encodedValue);
            
            _load_pixmap (pixmap);
            
            _imageFile = fileName;
            _bgConfig.store_attribute ("file", _imageFile);
         }
      }
   }
}


// Message Observer Interface
void
dmz::QtPluginMapProperties::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   if (Type == _backgroundEditMessage) {

      QMessageBox msgBox;
      QPushButton *loadButton = msgBox.addButton ("Load...", QMessageBox::ActionRole);
      
      QPushButton *clearButton = msgBox.addButton ("Clear", QMessageBox::DestructiveRole);
            
      if (!_bgItem) { clearButton->setEnabled (False); }
      
      QPushButton *cancelButton = msgBox.addButton ("Cancel", QMessageBox::RejectRole);

      msgBox.setIcon (QMessageBox::Question);
      msgBox.setText ("Background Image");
      
      QString infoText ("Load a new background image");
      infoText.append ("or\nclear the current background image.");
      
      msgBox.setInformativeText (infoText);
      msgBox.exec();

      if (msgBox.clickedButton() == loadButton) {
   
         QList<QByteArray> imagesFormatList (QImageReader::supportedImageFormats ());
         
         QString filter ("Images (");
         
         foreach (QString format, imagesFormatList) {
         
            filter.append (QString ("*.%1 ").arg (format));
         }

         filter = filter.simplified () + ")";
         
         QString fileName =
            QFileDialog::getOpenFileName (
               _mainWindowModule ? _mainWindowModule->get_widget () : 0,
               "Select Image File",
               _get_last_path (),
               filter);

         if (!fileName.isEmpty ()) {
         
            qApp->setOverrideCursor (QCursor (Qt::BusyCursor));
            
            const Handle UndoHandle (_undo.start_record ("Set Background"));

            Data data (_dataConverter.to_data (_imageFile));
            _undo.store_action (_undoMessage, get_plugin_handle (), &data);
               
            _load_background (qPrintable (fileName));
            
            _undo.stop_record (UndoHandle);
            
            qApp->restoreOverrideCursor ();
         }
      }
      else if (msgBox.clickedButton() == clearButton) {
    
         const Handle UndoHandle (_undo.start_record ("Clear Background"));
      
         Data data (_dataConverter.to_data (_imageFile));
         _undo.store_action (_undoMessage, get_plugin_handle (), &data);
            
         _imageFile.flush ();
         _bgConfig.store_attribute ("file", _imageFile);
         
         _clear_background ();

         _undo.stop_record (UndoHandle);
      }
   }
   else if (Type == _cleanupMessage) {
      
      _clear_background ();
      _imageFile.flush ();
   }
   else if (Type == _undoMessage) {

      String value (_dataConverter.to_string (InData));
      
      if (!value) {
      
         Data data (_dataConverter.to_data (_imageFile));
         _undo.store_action (_undoMessage, get_plugin_handle (), &data);

         _imageFile.flush ();
         _bgConfig.store_attribute ("file", _imageFile);
            
         _clear_background ();
      }
      else if (QFile::exists (value.get_buffer ())) {

         Data data (_dataConverter.to_data (_imageFile));
         _undo.store_action (_undoMessage, get_plugin_handle (), &data);

         _load_background (value);
      }
   }
}


QString
dmz::QtPluginMapProperties::_get_last_path () {
   
   String lastPath (_appState.get_default_directory ());

   QFileInfo fi (lastPath.get_buffer ());

   return fi.absoluteFilePath ();
}


dmz::Boolean
dmz::QtPluginMapProperties::_load_background (const String &FileName) {

   Boolean result (False);

   _imageFile = FileName;
   _bgConfig.store_attribute ("file", _imageFile);
         
   if (_imageFile) {
      
      QPixmap bg (_imageFile.get_buffer ());
      
      _load_pixmap (bg);
      
      _mapModule->zoom_extents ();
      
      _encode_image (_imageFile);
      
      result = True;
   }
   
   return result;
}


void
dmz::QtPluginMapProperties::_load_pixmap (const QPixmap &Pixmap) {

   if (_mapModule) {

      QGraphicsScene *scene = _mapModule->get_scene ();

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
dmz::QtPluginMapProperties::_clear_background () {

   if (_mapModule && _bgItem) {
      
      QGraphicsScene *scene = _mapModule->get_scene ();
      
      if (scene) {
         
         scene->removeItem (_bgItem);
         
         delete _bgItem;
         _bgItem = 0;
      }
   }
}


void
dmz::QtPluginMapProperties::_encode_image (const String &FileName) {

   if (FileName) {

      _data.set_value ("\n");

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
dmz::QtPluginMapProperties::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   _mainWindowModuleName = config_to_string (
      "module.mainWindow.name",
      local,
      "dmzQtModuleMainWindowBasic");
      
   _mapModuleName = config_to_string (
      "module.canvas.name",
      local,
      "dmzQtModuleMapBasic");
   
   init_archive (local);
   
   _cleanupMessage = config_create_message (
      "message.name",
      local,
      "CleanupObjectsMessage",
      context,
      &_log);
   
   _backgroundEditMessage = config_create_message (
      "message.background.edit",
      local,
      "CanvasBackgroundEditMessage",
      context,
      &_log);

   _undoMessage = config_create_message (
      "message.background.undo",
      local,
      "CanvasBackgroundUndoMessage",
      context,
      &_log);

   subscribe_to_message (_cleanupMessage);
   subscribe_to_message (_backgroundEditMessage);
   subscribe_to_message (_undoMessage);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginMapProperties (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginMapProperties (Info, local);
}

};
