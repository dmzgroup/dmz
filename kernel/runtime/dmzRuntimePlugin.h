#ifndef DMZ_RUNTIME_PLUGIN_DOT_H
#define DMZ_RUNTIME_PLUGIN_DOT_H

#include <dmzRuntimePluginInfo.h>
#include <dmzTypesBase.h>

namespace dmz {

   class RuntimeContext;
   class Config;

   class Plugin {

      public:
         Plugin (const PluginInfo &Info);
         virtual ~Plugin () {;}

         String get_plugin_name () const;
         Handle get_plugin_handle () const;
         RuntimeContext *get_plugin_runtime_context () const;

         virtual void discover_plugin (const Plugin *PluginPtr) = 0;
         virtual void start_plugin () = 0;
         virtual void stop_plugin () = 0;
         virtual void shutdown_plugin () = 0;
         virtual void remove_plugin (const Plugin *PluginPtr) = 0;

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
