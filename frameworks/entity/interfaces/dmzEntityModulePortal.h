#ifndef DMZ_ENTITY_MODULE_PORTAL_DOT_H
#define DMZ_ENTITY_MODULE_PORTAL_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class Matrix;
   class Vector;

   class EntityModulePortal {

      public:
         static EntityModulePortal *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         String get_entity_module_portal_name () const;
         Handle get_entity_module_portal_handle () const;

         // EntityModulePortal Interface
         virtual void set_view (
            const Vector &Position,
            const Matrix &Orientation,
            const Vector &Velocity) = 0;

         virtual void get_view (
            Vector &position,
            Matrix &orientation,
            Vector &velocity) = 0;

      protected:
         EntityModulePortal (const PluginInfo &Info);
         ~EntityModulePortal ();

      private:
         EntityModulePortal ();
         EntityModulePortal (const EntityModulePortal &);
         EntityModulePortal &operator= (const EntityModulePortal &);

         const PluginInfo &__Info;
   };

   //! \cond
   const char EntityModulePortalInterfaceName[] = "EntityModulePortalInterface";
   //! \endcond
};


inline dmz::EntityModulePortal *
dmz::EntityModulePortal::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (EntityModulePortal *)lookup_rtti_interface (
      EntityModulePortalInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::EntityModulePortal::EntityModulePortal (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (EntityModulePortalInterfaceName, __Info, (void *)this);
}


inline
dmz::EntityModulePortal::~EntityModulePortal () {

   remove_rtti_interface (EntityModulePortalInterfaceName, __Info);
}


inline dmz::String
dmz::EntityModulePortal::get_entity_module_portal_name () const { return __Info.get_name (); }


inline dmz::Handle
dmz::EntityModulePortal::get_entity_module_portal_handle () const { return __Info.get_handle (); }

#endif // DMZ_ENTITY_MODULE_PORTAL_DOT_H
