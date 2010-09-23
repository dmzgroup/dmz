#include "dmzQtModuleDropEventToMessage.h"
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <QtGui/QDropEvent>

dmz::QtModuleDropEventToMessage::QtModuleDropEventToMessage (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      QtModuleDropEvent (Info),
      _log (Info),
      _pick (0),
      _sConvert (Info),
      _vConvert (Info) {

   _init (local);
}


dmz::QtModuleDropEventToMessage::~QtModuleDropEventToMessage () {

}


// Plugin Interface
void
dmz::QtModuleDropEventToMessage::update_plugin_state (
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
dmz::QtModuleDropEventToMessage::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_pick) { _pick = RenderModulePick::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_pick && (_pick == RenderModulePick::cast (PluginPtr))) { _pick = 0; }
   }
}


// QtModuleDropEvent Interface
void
dmz::QtModuleDropEventToMessage::receive_drop_event (
      const Handle Source,
      const QDropEvent &Event) {

   if (_msg) {

      String str;

      Data out (get_plugin_runtime_context ());

      const QMimeData *MData = Event.mimeData ();

      if (MData && MData->hasText ()) {

         Vector pos, normal;
         Handle obj (0);

         str = qPrintable (MData->text ());

         _sConvert.write_data (str, out);

         QPoint point = Event.pos ();

         if (_pick) {

            _pick->source_to_world (Source, point.x (), point.y (), pos, normal, obj);
         }

         _vConvert.write_data (pos, out);

         _msg.send (_targets, &out);
      }
   }
}


// QtModuleDropEventToMessage Interface
void
dmz::QtModuleDropEventToMessage::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   _msg = config_create_message ("message.name", local, "", context, &_log);

   _targets = config_to_handle_container ("target", local, context);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtModuleDropEventToMessage (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtModuleDropEventToMessage (Info, local);
}

};
