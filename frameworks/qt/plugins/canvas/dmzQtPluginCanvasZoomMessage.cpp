#include "dmzQtPluginCanvasZoomMessage.h"
#include <dmzQtModuleCanvas.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesVector.h>


dmz::QtPluginCanvasZoomMessage::QtPluginCanvasZoomMessage (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      MessageObserver (Info),
      _log (Info),
      _canvasModule (0),
      _canvasModuleName (),
      _location (0, 0),
      _convertNum (Info) {

   _init (local);
}


dmz::QtPluginCanvasZoomMessage::~QtPluginCanvasZoomMessage () {

}


// Plugin Interface
void
dmz::QtPluginCanvasZoomMessage::update_plugin_state (
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
dmz::QtPluginCanvasZoomMessage::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_canvasModule) {

         _canvasModule = QtModuleCanvas::cast (PluginPtr, _canvasModuleName);
         if (_canvasModule) { _set_zoom (); }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_canvasModule && (_canvasModule == QtModuleCanvas::cast (PluginPtr))) {

         _canvasModule = 0;
      }
   }
}


// Message Observer Interface
void
dmz::QtPluginCanvasZoomMessage::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   if (Type == _zoomInMessage) {

      if (_zoomIndex < (_zoomList.count () - 1)) {

         Vector v;
         if (InData->lookup_vector (_positionAttr, 0, v)) {

            _location = QPointF (v.get_x (), v.get_z ());
            ++_zoomIndex;
            _set_zoom ();
         }
      }
   }
   else if (Type == _zoomOutMessage) {

      if (_zoomIndex > 0) {

         Vector v;
         if (InData->lookup_vector (_positionAttr, 0, v)) {

            _location = QPointF (v.get_x (), v.get_z ());
            --_zoomIndex;
            _set_zoom ();
         }
      }
   }

}


// QtPluginCanvasZoomMessage Interface
void
dmz::QtPluginCanvasZoomMessage::_set_zoom () {

   if (_canvasModule && (_zoomIndex <= (_zoomList.count () - 1)) && (_zoomIndex >= 0)) {

      Float32 zoom = _zoomList.at (_zoomIndex);
      _canvasModule->center_on (_location);
      _canvasModule->set_zoom (zoom);
      Data data = _convertNum.to_data (zoom);
      _zoomMessage.send (&data);
   }
}


void
dmz::QtPluginCanvasZoomMessage::_init (Config &local) {

   RuntimeContext *context = get_plugin_runtime_context ();
   _canvasModuleName = config_to_string ("module.canvas.name", local);
   _zoomMessage = config_create_message ("message", local, "Canvas_Zoom_Message", context);
   _zoomInMessage = config_create_message ("in-message", local, "Zoom_In_Message", context);
   _zoomOutMessage = config_create_message ("out-message", local, "Zoom_Out_Message", context);
   _positionAttr = config_to_named_handle ("attribute.position", local, "position", context);
   subscribe_to_message (_zoomInMessage);
   subscribe_to_message (_zoomOutMessage);

   _zoomIndex = 0;
   Config zoomList;
   if (local.lookup_all_config ("zoom", zoomList)) {

      ConfigIterator it;
      Config zoomConfig;
      while (zoomList.get_next_config (it, zoomConfig)) {

         Float32 zoomLevel (config_to_float32 ("amt", zoomConfig, 0.0));
         Boolean isStart (config_to_boolean ("start", zoomConfig, False));
         if (zoomLevel) {

            _zoomList.append (zoomLevel);
            if (isStart) { _zoomIndex = _zoomList.count () - 1; }
         }
      }
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginCanvasZoomMessage (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginCanvasZoomMessage (Info, local);
}

};
