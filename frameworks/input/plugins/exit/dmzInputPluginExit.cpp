#include "dmzInputPluginExit.h"
#include <dmzInputEventKey.h>
#include <dmzInputEventMasks.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>

/*!

\class dmz::InputPluginExit
\ingroup Input
\brief Input exit plugin.
\details Requests an exit when the mapped key is pressed. The default key value is the
Escape Key. If not input channels are specified then the default channel is used.
The Plugin's XML format:
\code
<dmz>
<dmzInputPluginExit>
   <key value="Key Value"/>
   <input>
      <channel name="Channel Name"/>
   </input>
</dmzInputPluginExit>
</dmz>
\endcode

*/

//! \cond
dmz::InputPluginExit::InputPluginExit (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      InputObserverUtil (Info, local),
      _exit (Info.get_context ()),
      _log (Info.get_name (), Info.get_context ()) {

   _init (local);
}


dmz::InputPluginExit::~InputPluginExit () {

   _keyTable.empty ();
}


void
dmz::InputPluginExit::receive_key_event (
      const UInt32 Channel,
      const InputEventKey &Value) {

   if (_keyTable.lookup (Value.get_key())) {

      _exit.request_exit (ExitStatusNormal, "Exit key pressed");
   }
}


void
dmz::InputPluginExit::_init (Config &local) {

   Config keys;

   if (local.lookup_all_config ("key", keys)) {

      ConfigIterator it;
      Config cd;

      Boolean found (keys.get_first_config (it, cd));

      while (found)  {

         const UInt32 Key (config_to_key_value ("value", cd, 0, &_log));

         if (Key) {

            UInt32 *keyPtr (new UInt32 (Key));

            if (keyPtr) {

               if (_keyTable.store (*keyPtr, keyPtr)) {

                  _log.info << "Setting exit key: "
                  << config_to_string ("value", cd) << endl;
               }
               else { delete keyPtr; keyPtr = 0; }
             }
          }

         found = keys.get_next_config (it, cd);
      }
   }
   else {

      UInt32 *keyPtr (new UInt32 (dmz::KeyEsc));

      if (keyPtr) {

         if (_keyTable.store (*keyPtr, keyPtr)) {

            _log.info << "Setting 'Esc' as default exit key" << endl;
         }
         else { delete keyPtr; keyPtr = 0; }
      }
   }

   init_input_channels (local, InputEventKeyMask, &_log);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzInputPluginExit (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::InputPluginExit (Info, local);
}

};
