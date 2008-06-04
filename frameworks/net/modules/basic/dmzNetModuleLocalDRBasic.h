#ifndef DMZ_NET_MODULE_LOCAL_DR_BASIC_DOT_H
#define DMZ_NET_MODULE_LOCAL_DR_BASIC_DOT_H

#include <dmzNetModuleLocalDR.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTime.h>
#include <dmzTypesHashTableUInt32Template.h>

namespace dmz {

   class ObjectModule;
   class ObjectType;

   class NetModuleLocalDRBasic :
         public Plugin,
         public NetModuleLocalDR {

      public:
         class ObjectUpdate {

            public:
               ObjectUpdate *next;

               virtual ~ObjectUpdate () { if (next) { delete next; next = 0; } }

               virtual Boolean update_object (
                  const Handle ObjectHandle,
                  ObjectModule &module,
                  Boolean &limitRate) = 0;

            protected:
               ObjectUpdate () : next (0) {;}

            private:
               ObjectUpdate (const ObjectUpdate &);
               ObjectUpdate &operator= (const ObjectUpdate &);
         };

         NetModuleLocalDRBasic (const PluginInfo &Info, Config &local);
         ~NetModuleLocalDRBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // NetModuleLocalDR Interface
         virtual Boolean update_object (const Handle ObjectHandle);

      protected:
         void _init (Config &local);
         ObjectUpdate *_create_update_list (Config &listData);
         ObjectUpdate *_create_test_from_type (const ObjectType &Type);

         Log _log;
         Time _time;

         ObjectUpdate *_defaultTest;
        
         HashTableUInt32Template<ObjectUpdate> _baseTable;
         HashTableUInt32Template<ObjectUpdate> _typeTable;

         ObjectModule *_objMod;

         UInt32 _defaultHandle;

      private:
         NetModuleLocalDRBasic ();
         NetModuleLocalDRBasic (const NetModuleLocalDRBasic &);
         NetModuleLocalDRBasic &operator= (const NetModuleLocalDRBasic &);
   };
};

#endif // DMZ_NET_MODULE_LOCAL_DR_BASIC_DOT_H
