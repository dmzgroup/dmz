#ifndef DMZ_INPUT_PLUGIN_EXIT_DOT_H
#define DMZ_INPUT_PLUGIN_EXIT_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzRuntimeExit.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableUInt32Template.h>

namespace dmz {

   class Config;

   class InputPluginExit : public Plugin, public InputObserverUtil {

      public:
         //! \cond
         InputPluginExit (const PluginInfo &Info, Config &local);
         virtual ~InputPluginExit ();

         virtual void discover_plugin (const Plugin *PluginPtr) {;}
         virtual void start_plugin () {;}
         virtual void stop_plugin () {;}
         virtual void shutdown_plugin () {;}
         virtual void remove_plugin (const Plugin *PluginPtr) {;}

         virtual void receive_key_event (
            const UInt32 Channel,
            const InputEventKey &Value);

      protected:
         void _init (Config &local);

         Exit _exit;
         HashTableUInt32Template<UInt32> _keyTable;
         Log _log;
         //! \endcond

      private:
         InputPluginExit ();
         InputPluginExit (const InputPluginExit &);
         const InputPluginExit &operator= (const InputPluginExit &);
   };
};

#endif // DMZ_INPUT_PLUGIN_EXIT_DOT_H
