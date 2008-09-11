#include "dmziPhoneCanvasView.h"
#include "dmziPhoneModuleCanvasBasic.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>


dmz::iPhoneModuleCanvasBasic::iPhoneModuleCanvasBasic (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      iPhoneModuleCanvas (Info),
      _log (Info),
      _channel (0),
      _canvas (0),
      _itemTable () {

_log.warn << "ctor" << endl;

//   _canvas = [[dmziPhoneCanvasView alloc] init];
         
   _init (local);
}


dmz::iPhoneModuleCanvasBasic::~iPhoneModuleCanvasBasic () {

   HashTableHandleIterator it;
   
   UIView *view (_itemTable.get_first (it));
   
   while (view) {
      
      [view release];
      view = _itemTable.get_next (it);
   }
   
   _itemTable.clear ();
   
   [_canvas release];
}


// Plugin Interface
void
dmz::iPhoneModuleCanvasBasic::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateStart) {

      _load_session ();
   }
   else if (State == PluginStateStop) {

      _save_session ();
   }
}

void
dmz::iPhoneModuleCanvasBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// iPhoneModuleCanvas Interface
dmz::Boolean
dmz::iPhoneModuleCanvasBasic::add_item (const Handle ObjectHandle, UIView *item) {

   Boolean retVal (False);

   if (item) {

      retVal = _itemTable.store (ObjectHandle, item);

      if (retVal) {

         [item retain];
         [_canvas addSubview:item];
      }
   }

   return retVal;
}


UIView *
dmz::iPhoneModuleCanvasBasic::lookup_item (const Handle ObjectHandle) {

   UIView *item (_itemTable.lookup (ObjectHandle));
   return item;
}


UIView *
dmz::iPhoneModuleCanvasBasic::remove_item (const Handle ObjectHandle) {

   UIView *item (_itemTable.remove (ObjectHandle));

   if (item) {

      [item removeFromSuperview];
      [item release];
   }

   return item;
}


void
dmz::iPhoneModuleCanvasBasic::_save_session () {

   // String data;
   //
   // Config session (get_plugin_name ());
   //
   // if (_canvas) {
   //
   //    session.add_config (qgraphicsview_to_config ("canvasView", *_canvas));
   // }
   //
   // set_session_config (get_plugin_runtime_context (), session);
}


void
dmz::iPhoneModuleCanvasBasic::_load_session () {

   // Config session (get_session_config (get_plugin_name (), get_plugin_runtime_context ()));
   //
   // if (_canvas) {
   //
   //    qgraphicsview_config_read ("canvasView", session, *_canvas);
   //    _canvas->set_scale (_canvas->get_scale ());
   // }
}


void
dmz::iPhoneModuleCanvasBasic::_init (Config &local) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmziPhoneModuleCanvasBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::iPhoneModuleCanvasBasic (Info, local);
}

};
