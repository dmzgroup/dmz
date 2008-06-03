#ifndef DMZ_NET_MODULE_LOCAL_DR_DOT_H
#define DMZ_NET_MODULE_LOCAL_DR_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>

namespace dmz {

   const char NetModuleLocalDRInterfaceName[] = "NetModuleLocalDRInterface";

   class NetModuleLocalDR {

      public:
         static NetModuleLocalDR *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         // NetModuleLocalDR Interface
         virtual Boolean update_object (const Handle ObjectHandle) = 0;

      protected:
         NetModuleLocalDR (const PluginInfo &Info);
         ~NetModuleLocalDR ();

      private:
         NetModuleLocalDR ();
         NetModuleLocalDR (const NetModuleLocalDR &);
         NetModuleLocalDR &operator= (const NetModuleLocalDR &);

         const PluginInfo &__Info;
   };
};


inline dmz::NetModuleLocalDR *
dmz::NetModuleLocalDR::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (NetModuleLocalDR *)lookup_rtti_interface (
      NetModuleLocalDRInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::NetModuleLocalDR::NetModuleLocalDR (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (NetModuleLocalDRInterfaceName, __Info, (void *)this);
}


inline
dmz::NetModuleLocalDR::~NetModuleLocalDR () {

   remove_rtti_interface (NetModuleLocalDRInterfaceName, __Info);
}

#endif // DMZ_NET_MODULE_LOCAL_DR_DOT_H
