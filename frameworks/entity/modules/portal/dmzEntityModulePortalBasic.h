#ifndef DMZ_ENTITY_MODULE_PORTAL_BASIC_DOT_H
#define DMZ_ENTITY_MODULE_PORTAL_BASIC_DOT_H

#include <dmzEntityModulePortal.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesVector.h>
#include <dmzTypesMatrix.h>

namespace dmz {

   class AudioModulePortal;
   class RenderModulePortal;

   class EntityModulePortalBasic :
         public Plugin,
         public EntityModulePortal {

      public:
         //! \cond
         EntityModulePortalBasic (const PluginInfo &Info, Config &local);
         ~EntityModulePortalBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // EntityModulePortal Interface
         virtual void set_view (
            const Vector &Position,
            const Matrix &Orientation,
            const Vector &Velocity);

         virtual void get_view (
            Vector &position,
            Matrix &orientation,
            Vector &velocity);

      protected:
         void _init (Config &local);

         Log _log;

         AudioModulePortal *_audio;
         RenderModulePortal *_render;

         Vector _position;
         Matrix _orientation;
         Vector _velocity;
         //! \endcond

      private:
         EntityModulePortalBasic ();
         EntityModulePortalBasic (const EntityModulePortalBasic &);
         EntityModulePortalBasic &operator= (const EntityModulePortalBasic &);

   };
};

#endif // DMZ_ENTITY_MODULE_PORTAL_BASIC_DOT_H
