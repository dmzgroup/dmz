#ifndef DMZ_PLUGIN_DEFINITIONS_DUMP_DOT_H
#define DMZ_PLUGIN_DEFINITIONS_DUMP_DOT_H

#include <dmzRuntimeDefinitionsObserver.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class PluginDefinitionsDump :
         public Plugin,
         public DefinitionsObserver {

      public:
         //! \cond
         PluginDefinitionsDump (const PluginInfo &Info, Config &local);
         ~PluginDefinitionsDump () {;}

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // Definitions Observer Interface
         virtual void define_named_handle (const Handle TheHandle, const String &Name);
         virtual void define_state (const Mask &TheState, const String &Name);
         virtual void define_object_type (const ObjectType &Type);
         virtual void define_event_type (const EventType &Type);

      protected:
         void _init (Config &local);

         Log _log;
         //! \endcond

      private:
         PluginDefinitionsDump ();
         PluginDefinitionsDump (const PluginDefinitionsDump &);
         PluginDefinitionsDump &operator= (const PluginDefinitionsDump &);

   };
};

#endif // DMZ_PLUGIN_DEFINITIONS_DUMP_DOT_H
