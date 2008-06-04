#ifndef DMZ_NET_MODULE_IDENTITY_MAP_BASIC_DOT_H
#define DMZ_NET_MODULE_IDENTITY_MAP_BASIC_DOT_H

#include <dmzNetModuleIdentityMap.h>
#include <dmzRuntimeDataConverters.h>
#include <dmzRuntimeHandleAllocator.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableUInt32Template.h>

namespace dmz {

   class NetModuleIdentityMapBasic :
         public Plugin,
         public NetModuleIdentityMap,
         public MessageObserver {

      public:
         NetModuleIdentityMapBasic (const PluginInfo &Info, Config &local);
         ~NetModuleIdentityMapBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // NetModuleIdentityMap Interface
         virtual UInt32 get_site_handle ();
         virtual UInt32 get_host_handle ();

         virtual Boolean create_site_host_entity (
            const Handle ObjectHandle,
            UInt32 &site,
            UInt32 &host,
            UInt32 &entity);

         virtual Boolean store_site_host_entity (
            const Handle ObjectHandle,
            const UInt32 Site,
            const UInt32 Host,
            const UInt32 Entity);

         virtual Boolean lookup_site_host_entity (
            const Handle ObjectHandle,
            UInt32 &site,
            UInt32 &host,
            UInt32 &entity);

         virtual Boolean store_name (
            const Handle ObjectHandle,
            const String &Name);

         virtual Boolean lookup_name (
            const Handle ObjectHandle,
            String &name);

         virtual Boolean lookup_handle_from_name (
            const String &Name,
            UInt32 &handle);

         virtual Boolean lookup_handle_from_site_host_entity (
            const UInt32 Site,
            const UInt32 Host,
            const UInt32 Entity,
            UInt32 &handle);

         virtual Boolean remove_object (const Handle ObjectHandle);

         // MessageObserver Interface
         virtual void receive_message (
            const Message &Msg,
            const UInt32 MessageSendHandle,
            const UInt32 TargetObserverHandle,
            const Data *InData,
            Data *outData);

      protected:
         struct EntityStruct {

            const Handle ObjectHandle;
            UInt32 site;
            UInt32 host;
            UInt32 entity;
            String name;

            Boolean entityHandleCreated;

            EntityStruct (const Handle TheHandle) :
                  ObjectHandle (TheHandle),
                  site (0),
                  host (0),
                  entity (0),
                  entityHandleCreated (False) {;}
         };

         struct HostStruct {

            const UInt32 Host;
            HashTableUInt32Template<EntityStruct> table;

            HostStruct (const UInt32 TheHost) : Host (TheHost) {;}
            ~HostStruct () { table.clear (); }
         };

         struct SiteStruct {

            const UInt32 Site;
            HashTableUInt32Template<HostStruct> table;

            SiteStruct (const UInt32 TheSite) : Site (TheSite) {;}
            ~SiteStruct () { table.empty (); }
         };

         void _init (Config &local);
         EntityStruct *_create_entity_struct (const Handle ObjectHandle);

         Log _log;
         UInt32 _site;
         UInt32 _host;

         HandleAllocator _netHandles;

         HashTableUInt32Template<SiteStruct> _siteTable;
         HashTableUInt32Template<EntityStruct> _objTable;
         HashTableStringTemplate<EntityStruct> _nameTable;

         Message _removeObjMsg;
         DataConverterHandle _handleConverter;

      private:
         NetModuleIdentityMapBasic ();
         NetModuleIdentityMapBasic (const NetModuleIdentityMapBasic &);
         NetModuleIdentityMapBasic &operator= (const NetModuleIdentityMapBasic &);
   };
};

#endif // DMZ_NET_MODULE_IDENTITY_MAP_BASIC_DOT_H
