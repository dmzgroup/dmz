#include <dmzObjectAttributeMasks.h>
#include <dmzRenderModuleIsect.h>
#include "dmzRenderPluginDisableObjectIsect.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::RenderPluginDisableObjectIsect::RenderPluginDisableObjectIsect (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _isect (0) {

   _init (local);
}


dmz::RenderPluginDisableObjectIsect::~RenderPluginDisableObjectIsect () {

   _isectTable.empty ();
}


// Plugin Interface
void
dmz::RenderPluginDisableObjectIsect::update_plugin_state (
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
dmz::RenderPluginDisableObjectIsect::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_isect) { _isect = RenderModuleIsect::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_isect && (_isect == RenderModuleIsect::cast (PluginPtr))) { _isect = 0; }
   }
}


// Object Observer Interface
void
dmz::RenderPluginDisableObjectIsect::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (_isect) {

      Boolean *value (0);

      ObjectType current (Type);

      while (current && !value) {

         value = _isectTable.lookup (current.get_handle ());

         if (!value) {

            Config data;

            if (current.get_config ().lookup_config ("render.isect", data)) {

               Boolean disable = config_to_boolean ("disable", data, False);

               value = new Boolean (disable);

               if (!_isectTable.store (current.get_handle () , value)) {

                  delete value; value = 0;
               }
            }
         }

         current.become_parent ();
      }

      if (value && *value) {

         if (_isect->disable_isect (ObjectHandle) == 0) {

            *value = False;

            _log.error << "Unable to disable intersection for type: " << Type.get_name ()
               << endl;
         }
      }
   }
}


void
dmz::RenderPluginDisableObjectIsect::_init (Config &local) {

   activate_default_object_attribute (ObjectCreateMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginDisableObjectIsect (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginDisableObjectIsect (Info, local);
}

};
