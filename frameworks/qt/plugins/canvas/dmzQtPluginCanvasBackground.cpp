#include <dmzQtModuleCanvas.h>
#include <dmzQtModuleMainWindow.h>
#include "dmzQtPluginCanvasBackground.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <QtGui/QtGui>


dmz::QtPluginCanvasBackground::QtPluginCanvasBackground (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      MessageObserver (Info),
      _log (Info),
      _canvasModule (0),
      _canvasModuleName (),
      _mainWindowModule (0),
      _mainWindowModuleName (),
      _backgroundEditMessage (),
      _bgItem (0) {

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
      msgBox.setText ("Network Analysis Background");
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


void
dmz::QtPluginCanvasBackground::_load_background () {

   QString fileName =
      QFileDialog::getOpenFileName (
         _mainWindowModule ? _mainWindowModule->get_widget () : 0,
         "Background Image File",
         ".", //         _get_last_path (),
         "Images (*.png *.jpg)");// + _suffix.get_buffer ());
         
   if (_canvasModule && !fileName.isEmpty ()) {

      QGraphicsScene *scene = _canvasModule->get_scene ();

      if (scene) {

         QPixmap bg (fileName);

         if (!_bgItem) {
            
            _bgItem = scene->addPixmap (bg);
            _bgItem->setZValue (-1000);
         }
         else {
            
            _bgItem->setPixmap (bg);
         }
         
         _canvasModule->zoom_extents ();
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
dmz::QtPluginCanvasBackground::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   _mainWindowModuleName = config_to_string ("module.mainWindow.name", local);
   _canvasModuleName = config_to_string ("module.canvas.name", local);
   
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
