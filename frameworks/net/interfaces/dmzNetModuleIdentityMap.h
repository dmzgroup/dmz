#ifndef DMZ_NET_MODULE_IDENTITY_MAP_DOT_H
#define DMZ_NET_MODULE_IDENTITY_MAP_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>


namespace dmz {

   //! \cond
   const char NetModuleIdentityMapInterfaceName[] = "NetModuleIdentityMapInterface";
   //! \endcond

   class NetModuleIdentityMap {

      public:
         static NetModuleIdentityMap *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         virtual UInt32 get_site_id () = 0;
         virtual UInt32 get_host_id () = 0;

         virtual Boolean create_site_host_entity (
            const Handle ObjectHandle,
            UInt32 &site,
            UInt32 &host,
            UInt32 &entity) = 0;

         virtual Boolean store_site_host_entity (
            const Handle ObjectHandle,
            const UInt32 Site,
            const UInt32 Host,
            const UInt32 Entity) = 0;

         virtual Boolean lookup_site_host_entity (
            const Handle ObjectHandle,
            UInt32 &site,
            UInt32 &host,
            UInt32 &entity) = 0;

         virtual Boolean store_name (
            const Handle ObjectHandle,
            const String &Name) = 0;

         virtual Boolean lookup_name (
            const Handle ObjectHandle,
            String &name) = 0;

         virtual Boolean lookup_handle_from_name (
            const String &Name,
            Handle &handle) = 0;

         virtual Boolean lookup_handle_from_site_host_entity (
            const UInt32 Site,
            const UInt32 Host,
            const UInt32 Entity,
            Handle &handle) = 0;

         virtual Boolean remove_object (const Handle ObjectHandle) = 0;

      protected:
         NetModuleIdentityMap (const PluginInfo &Info);
         ~NetModuleIdentityMap ();

      private:
         const PluginInfo &__Info;
   };
};


inline dmz::NetModuleIdentityMap *
dmz::NetModuleIdentityMap::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (NetModuleIdentityMap *)lookup_rtti_interface (
      NetModuleIdentityMapInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::NetModuleIdentityMap::NetModuleIdentityMap (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (
      NetModuleIdentityMapInterfaceName,
      __Info,
     (void *)this);
}


inline
dmz::NetModuleIdentityMap::~NetModuleIdentityMap () {

   remove_rtti_interface (NetModuleIdentityMapInterfaceName, __Info);
}

#endif //  DMZ_NET_MODULE_IDENTITY_MAP_DOT_H
