#include "dmzNetModuleIdentityMapBasic.h"
#include <dmzObjectConsts.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::NetModuleIdentityMapBasic::NetModuleIdentityMapBasic (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      NetModuleIdentityMap (Info),
      MessageObserver (Info),
      _log (Info),
      _site (0),
      _host (0),
      _netHandles (1001, 1, 0xFFFF),
      _handleConverter (Info.get_context ()) {

   _init (local);

   Definitions defs (Info, &_log);

   if (defs.create_message_type (ObjectDestroyMessageName, _removeObjMsg)) {

      subscribe_to_message (_removeObjMsg);
   }
}


dmz::NetModuleIdentityMapBasic::~NetModuleIdentityMapBasic () {

   _nameTable.clear ();
   _siteTable.empty ();
   _objTable.empty ();
}


dmz::UInt32
dmz::NetModuleIdentityMapBasic::get_site_handle () { return _site; }


dmz::UInt32
dmz::NetModuleIdentityMapBasic::get_host_handle () { return _host; }


dmz::Boolean
dmz::NetModuleIdentityMapBasic::create_site_host_entity (
      const Handle ObjectHandle,
      UInt32 &site,
      UInt32 &host,
      UInt32 &entity) {

   Boolean result (False);

   result = lookup_site_host_entity (ObjectHandle, site, host, entity);

   if (!result) {

      const UInt32 StartEntity (_netHandles.get_next_handle (""));
      entity = StartEntity;

      result = store_site_host_entity (ObjectHandle, _site, _host, entity);

      Boolean done (result);

      while (!done && !result) {

         _netHandles.release_handle (entity);
         entity = _netHandles.get_next_handle ("");
         result = store_site_host_entity (ObjectHandle, _site, _host, entity);

         if (StartEntity == entity) { done = True; }
      }

      if (result) {

         EntityStruct *es (_create_entity_struct (ObjectHandle));

         if (es) { es->entityHandleCreated = True; }
         site = _site;
         host = _host;
      }
      else { entity = 0; }
   }

   return result;
}


dmz::Boolean
dmz::NetModuleIdentityMapBasic::store_site_host_entity (
      const Handle ObjectHandle,
      const UInt32 Site,
      const UInt32 Host,
      const UInt32 Entity) {

   Boolean result (False);

   EntityStruct *es (_create_entity_struct (ObjectHandle));

   if (es) {

      if ((Site == es->site) && (Host == es->host) && (Entity == es->entity)) {

         result = True;
      }
      else {

         es->site = Site;
         es->host = Host;
         es->entity = Entity;

         SiteStruct *ss = _siteTable.lookup (Site);

         if (!ss) {

            ss = new SiteStruct (Site);
            if (!_siteTable.store (Site, ss)) { delete ss; ss = 0; }
         }

         if (ss) {

            HostStruct *hs = ss->table.lookup (Host);

            if (!hs) {

               hs = new HostStruct (Host);
               if (!ss->table.store (Host, hs)) { delete hs; hs = 0; }
            }

            if (hs) { result = hs->table.store (Entity, es); }
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::NetModuleIdentityMapBasic::lookup_site_host_entity (
      const Handle ObjectHandle,
      UInt32 &site,
      UInt32 &host,
      UInt32 &entity) {

   Boolean result (False);

   EntityStruct *es (_create_entity_struct (ObjectHandle));

   if (es) {

      site = es->site;
      host = es->host;
      entity = es->entity;

      result = site || host || entity;
   }

   return result;
}


dmz::Boolean
dmz::NetModuleIdentityMapBasic::store_name (
      const Handle ObjectHandle,
      const String &Name) {

   Boolean result (False);

   EntityStruct *es (_create_entity_struct (ObjectHandle));

   if (es) {

      if (!Name.is_null () && (Name == es->name)) { result = True; }
      else {

         if (!es->name.is_null ()) { _nameTable.remove (es->name); }

         es->name = Name;

         if (!es->name.is_null ()) { result = _nameTable.store (Name, es); }
      }
   }

   return result;
}


dmz::Boolean
dmz::NetModuleIdentityMapBasic::lookup_name (
      const Handle ObjectHandle,
      String &name) {

   Boolean result (False);

   EntityStruct *es (_create_entity_struct (ObjectHandle));

   if (es) { name = es->name; result = !name.is_null (); }

   return result;
}


dmz::Boolean
dmz::NetModuleIdentityMapBasic::lookup_handle_from_name (
      const String &Name,
      Handle &handle) {

   Boolean result (False);

   EntityStruct *es (_nameTable.lookup (Name));

   if (es) { handle = es->ObjectHandle; result = (handle != 0); }

   return result;
}


dmz::Boolean
dmz::NetModuleIdentityMapBasic::lookup_handle_from_site_host_entity (
      const UInt32 Site,
      const UInt32 Host,
      const UInt32 Entity,
      Handle &handle) {

   Boolean result (False);

   SiteStruct *ss (_siteTable.lookup (Site));

   if (ss) {

      HostStruct *hs (ss->table.lookup (Host));

      if (hs) {

         EntityStruct *es (hs->table.lookup (Entity));

         if (es) {

            handle = es->ObjectHandle; result = (handle != 0);
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::NetModuleIdentityMapBasic::remove_object (const Handle ObjectHandle) {

   Boolean result (False);

   EntityStruct *es (_objTable.remove (ObjectHandle));

   if (es) {

      if (es->entityHandleCreated) { _netHandles.release_handle (es->entity); }

      _nameTable.remove (es->name);

      SiteStruct *ss (_siteTable.lookup (es->site));

      if (ss) {

         HostStruct *hs (ss->table.lookup (es->host));

         if (hs) { hs->table.remove (es->entity); }
      }

      delete es; es = 0;

      result = True;
   }

   return result;
}


void
dmz::NetModuleIdentityMapBasic::receive_message (
      const Message &Msg,
      const UInt32 MessageSendHandle,
      const UInt32 TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   if (Msg.is_of_type (_removeObjMsg)) {

      const Handle ObjectHandle (_handleConverter.to_handle (InData));

      if (ObjectHandle) { remove_object (ObjectHandle); }
   }
}


void
dmz::NetModuleIdentityMapBasic::_init (Config &local) {

   const UInt32 Site (0);
   const UInt32 Host (0);

   _site = config_to_uint32 ("site.value", local, Site);
   _host = config_to_uint32 ("host.value", local, Host);

   if ((Site == _site) && (Host == _host)) {

      _log.warn << "Site and host not defined, using site: " << _site << " and host : "
         << _host << endl;
   }
   else { _log.info << "Site: " << _site << " Host: " << _host << endl; }
}


dmz::NetModuleIdentityMapBasic::EntityStruct *
dmz::NetModuleIdentityMapBasic::_create_entity_struct (const Handle ObjectHandle) {

   EntityStruct *es (_objTable.lookup (ObjectHandle));

   if (!es) {

      es = new EntityStruct (ObjectHandle);

      if (es && !_objTable.store (ObjectHandle, es)) { delete es; es = 0; }
   }

   return es;
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzNetModuleIdentityMapBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::NetModuleIdentityMapBasic (Info, local);
}

};
