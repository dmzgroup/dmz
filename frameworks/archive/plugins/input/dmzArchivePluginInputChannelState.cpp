#include <dmzArchiveModule.h>
#include <dmzInputModule.h>
#include "dmzArchivePluginInputChannelState.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!
\class dmz::ArchivePluginInputChannelState
\ingroup Archive
\brief Archives and restores the state of the specified input channels.
\details
Calls dmz::ArchiveObserverUtil::init_archive.
\code
<dmz>
<ArchivePluginInputChannelState>
   <channel name="Name of channel to archive"/>
   <channel name="Name of channel to archive"/>
</ArchivePluginInputChannelState>
</dmz>
\endcode
\sa dmz::ArchiveObserverUtil::init_archive()

*/

namespace {

static const char DisableAllName[] = "disable-all-channels";

};

//! \cond
dmz::ArchivePluginInputChannelState::ArchivePluginInputChannelState (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ArchiveObserverUtil (Info, local),
      _log (Info),
      _input (0) {

   _init (local);
}


dmz::ArchivePluginInputChannelState::~ArchivePluginInputChannelState () {

}


// Plugin Interface
void
dmz::ArchivePluginInputChannelState::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {;}


void
dmz::ArchivePluginInputChannelState::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_input) { _input = InputModule::cast (PluginPtr, _inputModuleName); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_input && (_input == InputModule::cast (PluginPtr))) { _input = 0; }
   }
}


// ArchiveObserver Interface.
void
dmz::ArchivePluginInputChannelState::create_archive (
      const Handle ArchiveHandle,
      const Int32 Version,
      Config &local,
      Config &global) {

   if (is_active_archive_handle (ArchiveHandle) && _input) {

      Definitions defs (get_plugin_runtime_context (), &_log);

      HandleContainerIterator it;
      Handle value (0);
      Boolean set (False);

      while (_channels.get_next (it, value)) {

         const Boolean State = _input->get_channel_state (value);

         if (State) {

            set = True;
            Config channel ("channel");

            channel.store_attribute (
               "name",
               defs.lookup_named_handle_name (value));

            local.add_config (channel);
         }
      }

      if (!set) {

         Config cd (DisableAllName);
         cd.store_attribute ("value", "true");
         local.add_config (cd);
      }
   }
}


void
dmz::ArchivePluginInputChannelState::process_archive (
      const Handle ArchiveHandle,
      const Int32 Version,
      Config &local,
      Config &global) {

   if (is_active_archive_handle (ArchiveHandle)) {

      Definitions defs (get_plugin_runtime_context (), &_log);
      Config list;
      HandleContainer activeChannels;

      Boolean found = False;

      if (local.lookup_all_config ("channel", list)) {

         found = True;

         ConfigIterator it;
         Config channel;

         while (list.get_next_config (it, channel)) {

            activeChannels.add (defs.lookup_named_handle (
               config_to_string ("name", channel)));
         }
      }
      else {

         /*
            Note: This else clause is here for compatibility with older archives that
                  would only have one channel active.
         */
         Handle channel (defs.lookup_named_handle (config_to_string ("channel", local)));

         if (channel) { activeChannels.add (channel); found = True; }
      }

      if (activeChannels.get_count () == 0) {

         found = config_to_boolean (String (DisableAllName) + ".value", local);
      }

      if (found && _input) {

         HandleContainerIterator it;
         Handle value (0);

         while (_channels.get_next (it, value)) {

            if (!activeChannels.contains (value)) {

               _input->set_channel_state (value, False);
            }
         }

         it.reset ();

         while (activeChannels.get_next (it, value)) {

            _input->set_channel_state (value, True);
         }
      }
   }
}


void
dmz::ArchivePluginInputChannelState::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context ());

   init_archive (local);

   Config list;

   if (local.lookup_all_config ("channel", list)) {

      ConfigIterator it;
      Config channel;

      while (list.get_next_config (it, channel)) {

         _channels.add (
            defs.create_named_handle (config_to_string ("name", channel)));
      }
   }
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzArchivePluginInputChannelState (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ArchivePluginInputChannelState (Info, local);
}

};
