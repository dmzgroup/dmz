#ifndef DMZ_OBJECT_MODULE_SELECT_BASIC_DOT_H
#define DMZ_OBJECT_MODULE_SELECT_BASIC_DOT_H

#include <dmzObjectModuleSelect.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class ObjectModuleSelectBasic :
         public Plugin,
         public ObjectModuleSelect,
         public ObjectObserverUtil {

      public:
         //! \cond
         ObjectModuleSelectBasic (const PluginInfo &Info, Config &local);
         ~ObjectModuleSelectBasic ();

         // ObjectModuleSelect Interface
         virtual void get_selected_objects (HandleContainer &container);
      
         virtual Boolean is_selected (const Handle ObjectHandle);
   
         virtual Boolean select_object (
            const Handle ObjectHandle,
            const ObjectSelectModeEnum Mode);
      
         virtual Boolean unselect_object (const Handle ObjectHandle);
   
         virtual void unselect_all_objects ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr) {;}
         virtual void start_plugin () {;}
         virtual void stop_plugin () {;}
         virtual void shutdown_plugin () {;}
         virtual void remove_plugin (const Plugin *PluginPtr) {;}

         // Object Observer Interface
         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void remove_object_attribute (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &AttrMask);

         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

      protected:
         void _init (Config &local);

         // Object Observer Util Interface
         virtual void _store_object_module (ObjectModule &objMod);

         HashTableHandleTemplate<const ObjectModuleSelectBasic> _selectTable;
         Handle _selectHandle;
         Log _log;
         //! \endcond

      private:
         ObjectModuleSelectBasic ();
         ObjectModuleSelectBasic (const ObjectModuleSelectBasic &);
         ObjectModuleSelectBasic &operator= (const ObjectModuleSelectBasic &);

   };
};

#endif // DMZ_OBJECT_MODULE_SELECT_BASIC_DOT_H
