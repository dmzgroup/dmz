#include "dmzPluginSaveMessageMonostate.h"
#include <dmzRuntimeConfigWrite.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>

/*!

\class dmz::PluginSaveMessageMonostate
\ingroup Foundation
\brief Saves and restores dmz::Message monostates.
\details Uses the runtime session to archive and restore dmz::Message monostate values.
\code
<dmz>
<dmzPluginSaveMessageMonostate>
   <message name="Message Name"/>
   <message name="Next Message Name"/>
   ...
   <message name="Final Message Name"/>
</dmzPluginSaveMessageMonostate>
</dmz>
\endcode
\sa dmz;::get_session_config

*/


//! \cond
dmz::PluginSaveMessageMonostate::PluginSaveMessageMonostate (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      _log (Info),
      _defs (Info),
      _messages () {

   _init (local);
}


dmz::PluginSaveMessageMonostate::~PluginSaveMessageMonostate () {

}


// Plugin Interface
void
dmz::PluginSaveMessageMonostate::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

      RuntimeContext *context (get_plugin_runtime_context ());

      Config session (get_plugin_name ());
      String name;
      
      while (_messages.get_next (name)) {
      
         Message message;
         
         if (_defs.lookup_message (name, message)) {
            
            const Data *data (message.get_monostate ());

            if (data) {

               Config config ("message");
               config.store_attribute ("name", message.get_name ());
               config.add_config (data_to_config (*data, context, &_log));

               session.add_config (config);
            }
         }
      }
      
      set_session_config (context, session);
   }
}


void
dmz::PluginSaveMessageMonostate::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


void
dmz::PluginSaveMessageMonostate::_restore_messages (Config &list) {

   RuntimeContext *context (get_plugin_runtime_context ());
   
   ConfigIterator it;
   Config cd;
   
   while (list.get_next_config (it, cd)) {
      
      const String Name (config_to_string ("name", cd));
      Message message;
      
      if (_defs.lookup_message (Name, message)) {

         if (_messages.contains (message.get_name ())) {
         
            Data data;
            
            if (config_to_data ("data", cd, context, data, &_log)) {
               
               message.send (&data);
            }
         }
      }
   }
}


void
dmz::PluginSaveMessageMonostate::_init_messages (Config &list) {
   
   RuntimeContext *context (get_plugin_runtime_context ());

   ConfigIterator it;
   Config cd;
   
   while (list.get_next_config (it, cd)) {
      
      const String Name (config_to_string ("name", cd));
      Message message;
      
      if (_defs.lookup_message (Name, message)) {
         
         _messages.add_string (message.get_name ());
      }
   }
}


void
dmz::PluginSaveMessageMonostate::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   Config initList;
   
   if (local.lookup_all_config ("message", initList)) {
      
      _init_messages (initList);
   }

   Config restoreList;
   Config session (get_session_config (get_plugin_name (), context));
   
   if (session.lookup_all_config ("message", restoreList)) {

      _restore_messages (restoreList);
   }
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzPluginSaveMessageMonostate (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::PluginSaveMessageMonostate (Info, local);
}

};
