#ifndef DMZ_RUNTIME_PLUGIN_DOT_H
#define DMZ_RUNTIME_PLUGIN_DOT_H

#include <dmzRuntimePluginInfo.h>
#include <dmzTypesBase.h>

namespace dmz {

   class RuntimeContext;
   class Config;

   enum PluginStateEnum {
      PluginStateInit, //!< Plugin state start mode.
      PluginStateStart, //!< Plugin state start mode.
      PluginStateStop, //!< Plugin state stop mode.
      PluginStateShutdown, //!< Plugin state shutdown mode.
   };

   enum PluginDiscoverEnum {
      PluginDiscoverAdd, //!< Discovered plugin is being added.
      PluginDiscoverRemove //!< Discovered plugin is being removed.
   };

   class Plugin {

      public:
         Plugin (const PluginInfo &Info);
         virtual ~Plugin () {;}

         String get_plugin_name () const;
         Handle get_plugin_handle () const;
         RuntimeContext *get_plugin_runtime_context () const;

         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) = 0;

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) = 0;

      protected:
         //! Reference to the Plugin initialization information.
         const PluginInfo &_PluginInfoData;

      private:
         Plugin ();
         Plugin (const Plugin &);
         const Plugin &operator= (const Plugin &);
   };
};


inline
dmz::Plugin::Plugin (const PluginInfo &Info) : _PluginInfoData (Info) {;}


inline dmz::String
dmz::Plugin::get_plugin_name () const { return _PluginInfoData.get_name (); }


inline dmz::Handle
dmz::Plugin::get_plugin_handle () const { return _PluginInfoData.get_handle (); }


inline dmz::RuntimeContext *
dmz::Plugin::get_plugin_runtime_context () const {

   return _PluginInfoData.get_context ();
}


typedef dmz::Plugin *(*create_plugin_factory_function) (
   const dmz::PluginInfo &Info,
   dmz::Config &local,
   dmz::Config &global);

#endif // DMZ_RUNTIME_PLUGIN_DOT_H
