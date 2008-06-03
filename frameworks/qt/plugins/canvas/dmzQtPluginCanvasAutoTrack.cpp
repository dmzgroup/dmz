#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include "dmzQtPluginCanvasAutoTrack.h"
#include <dmzQtModuleCanvas.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <QtGui/QtGui>


dmz::QtPluginCanvasAutoTrack::QtPluginCanvasAutoTrack (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      Sync (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defaultAttributeHandle (0),
      _hilAttributeHandle (0),
      _hilHandle (0),
      _canvasModule (0),
      _canvasModuleName (),
      _updateView (False) {
   
   _init (local);
}


dmz::QtPluginCanvasAutoTrack::~QtPluginCanvasAutoTrack () {

}


// Plugin Interface
void
dmz::QtPluginCanvasAutoTrack::discover_plugin (const Plugin *PluginPtr) {

   if (!_canvasModule) {
      
      _canvasModule = QtModuleCanvas::cast (PluginPtr, _canvasModuleName);
   }
}


void
dmz::QtPluginCanvasAutoTrack::start_plugin () {
   
}


void
dmz::QtPluginCanvasAutoTrack::update_sync (const Float64 TimeDelta) {

   if (_updateView && _canvasModule && _hilHandle) {
      
      QGraphicsView *view (_canvasModule->get_view ());
      QGraphicsItem *item (_canvasModule->lookup_item (_hilHandle));

      if (item && view) { view->ensureVisible (item); }
      
      _updateView = False;
   }
}


void
dmz::QtPluginCanvasAutoTrack::stop_plugin () {

}


void
dmz::QtPluginCanvasAutoTrack::shutdown_plugin () {

}


void
dmz::QtPluginCanvasAutoTrack::remove_plugin (const Plugin *PluginPtr) {

   if (_canvasModule && (_canvasModule == QtModuleCanvas::cast (PluginPtr))) {
         
      _canvasModule = 0;
   }
}


// Object Observer Interface
void
dmz::QtPluginCanvasAutoTrack::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (AttributeHandle == _defaultAttributeHandle) {

      if (ObjectHandle == _hilHandle) { _updateView = True; }
   }
}


void
dmz::QtPluginCanvasAutoTrack::update_object_orientation (
   const UUID &Identity,
   const Handle ObjectHandle,
   const Handle AttributeHandle,
   const Matrix &Value,
   const Matrix *PreviousValue) {
      
   if (AttributeHandle == _defaultAttributeHandle) {

      if (ObjectHandle == _hilHandle) { _updateView = True; }
   }
}


void
dmz::QtPluginCanvasAutoTrack::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (AttributeHandle == _hilAttributeHandle) {
      
      if (Value) {
      
         _hilHandle = ObjectHandle;
         _updateView = True;
      }
      else { _hilHandle = 0; }
   }
}


void
dmz::QtPluginCanvasAutoTrack::_init (Config &local) {
   
   _canvasModuleName = config_to_string ("module.canvas.name", local);
   
   _defaultAttributeHandle = activate_default_object_attribute (
      ObjectPositionMask | ObjectOrientationMask);

   _hilAttributeHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginCanvasAutoTrack (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginCanvasAutoTrack (Info, local);
}

};
