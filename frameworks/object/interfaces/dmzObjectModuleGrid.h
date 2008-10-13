#ifndef DMZ_OBJECT_MODULE_GRID_DOT_H
#define DMZ_OBJECT_MODULE_GRID_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class HandleContainer;
   class ObjectObserverGrid;
   class Volume;

   class ObjectModuleGrid {

      public:
         static ObjectModuleGrid *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         String get_object_module_grid_name () const;
         Handle get_object_module_grid_handle () const;

         // ObjectModuleGrid Interface
         virtual Boolean register_object_observer_grid (ObjectObserverGrid &observer) = 0;
         virtual Boolean update_object_observer_grid (ObjectObserverGrid &observer) = 0;
         virtual Boolean release_object_observer_grid (ObjectObserverGrid &observer) = 0;

         virtual void find_objects (
            const Volume &SearchSpace,
            HandleContainer &objects,
            const ObjectTypeSet *IncludeTypes = 0,
            const ObjectTypeSet *ExcludeTypes = 0) = 0;

      protected:
         ObjectModuleGrid (const PluginInfo &Info);
         ~ObjectModuleGrid ();

      private:
         ObjectModuleGrid ();
         ObjectModuleGrid (const ObjectModuleGrid &);
         ObjectModuleGrid &operator= (const ObjectModuleGrid &);

         const PluginInfo &__Info;
   };

   //! \cond
   const char ObjectModuleGridInterfaceName[] = "ObjectModuleGridInterface";
   //! \endcond
};


inline dmz::ObjectModuleGrid *
dmz::ObjectModuleGrid::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (ObjectModuleGrid *)lookup_rtti_interface (
      ObjectModuleGridInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::ObjectModuleGrid::ObjectModuleGrid (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (ObjectModuleGridInterfaceName, __Info, (void *)this);
}


inline
dmz::ObjectModuleGrid::~ObjectModuleGrid () {

   remove_rtti_interface (ObjectModuleGridInterfaceName, __Info);
}


inline dmz::String
dmz::ObjectModuleGrid::get_object_module_grid_name () const { return __Info.get_name (); }


inline dmz::Handle
dmz::ObjectModuleGrid::get_object_module_grid_handle () const {

   return __Info.get_handle ();
}

#endif // DMZ_OBJECT_MODULE_GRID_DOT_H
