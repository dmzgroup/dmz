#include "dmzAudioModulePortalBasic.h"
#include <dmzAudioModule.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

/*!

\class dmz::AudioModulePortalBasic
\ingroup Audio
\brief Basic implementation of the dmz::AudioModulePortal
\details
The audio module portal XML format:
\code
<dmz>
<dmzAudioModulePortalBasic>
   <name value="listener name"/>
</dmzAudioModulePortalBasic>
<dmz>
\endcode
If the listener name is not specified, the default listener is used.
*/

//! \cond
dmz::AudioModulePortalBasic::AudioModulePortalBasic (
   const PluginInfo &Info,
   const Config &Local) :
      Plugin (Info),
      AudioModulePortal (Info),
      _handle (0),
      _audioModule (0) {

   _init (Local);
}


dmz::AudioModulePortalBasic::~AudioModulePortalBasic () {

   if (_audioModule) {
      _audioModule->destroy_listener (_handle);
   }
}


// Plugin Interface
void
dmz::AudioModulePortalBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_audioModule) {

         _audioModule = AudioModule::cast (PluginPtr);

         if (_audioModule) {

            _handle = _audioModule->create_listener (_name);
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_audioModule && _handle && (_audioModule == AudioModule::cast (PluginPtr))) {

         _audioModule->destroy_listener (_handle);
         _handle = 0;
         _audioModule = 0;
      }
   }
}


// AudioModulePortal Interface
dmz::Boolean
dmz::AudioModulePortalBasic::is_master_portal () const {

   Boolean result (True);

   return result;
}


void
dmz::AudioModulePortalBasic::set_view (
      const Vector &Position,
      const Matrix &Orientation,
      const Vector &Velocity) {

   if (_audioModule && _handle) {

      _audioModule->set_listener (_handle, Position, Orientation, Velocity);
   }
}


void
dmz::AudioModulePortalBasic::get_view (
      Vector &position,
      Matrix &orientation,
      Vector &velocity) const {

   if (_audioModule && _handle) {

      _audioModule->get_listener (_handle, position, orientation, velocity);
   }
}


void
dmz::AudioModulePortalBasic::set_mute_state (const Boolean Value) {

   if (_audioModule && _handle) {

      _audioModule->set_mute_all_state (Value);
   }
}


dmz::Boolean
dmz::AudioModulePortalBasic::get_mute_state () const {

   Boolean result (False);

   if (_audioModule && _handle) {

      _audioModule->get_mute_all_state (result);
   }

   return result;
}

void
dmz::AudioModulePortalBasic::_init (const Config &Local) {

   Config data;
   if (Local.lookup_config ("name", data)) {

      _name = config_to_string ("value", data);
   }

}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzAudioModulePortalBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::AudioModulePortalBasic (Info, local);
}

};
