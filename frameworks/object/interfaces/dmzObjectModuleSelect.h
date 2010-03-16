#ifndef DMZ_OBJECT_MODULE_SELECT_DOT_H
#define DMZ_OBJECT_MODULE_SELECT_DOT_H

#include <dmzObjectConsts.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesBase.h>

namespace dmz {

   class ObjectModuleSelect {

      public:
         static ObjectModuleSelect *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         // ObjectModuleSelect Interface
         virtual void get_selected_objects (HandleContainer &container) = 0;

         virtual Boolean is_selected (const Handle ObjectHandle) = 0;

         virtual Boolean select_object (
            const Handle ObjectHandle,
            const ObjectSelectModeEnum Mode) = 0;

         virtual Boolean unselect_object (const Handle ObjectHandle) = 0;

         virtual void unselect_all_objects () = 0;

      protected:
         ObjectModuleSelect (const PluginInfo &Info);
         ~ObjectModuleSelect ();

      private:
         ObjectModuleSelect ();
         ObjectModuleSelect (const ObjectModuleSelect &);
         ObjectModuleSelect &operator= (const ObjectModuleSelect &);

         const PluginInfo &__Info;
   };

   //! \cond
   const char ObjectModuleSelectInterfaceName[] = "ObjectModuleSelectInterface";
   //! \endcond
};


inline dmz::ObjectModuleSelect *
dmz::ObjectModuleSelect::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (ObjectModuleSelect *)lookup_rtti_interface (
      ObjectModuleSelectInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::ObjectModuleSelect::ObjectModuleSelect (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (ObjectModuleSelectInterfaceName, __Info, (void *)this);
}


inline
dmz::ObjectModuleSelect::~ObjectModuleSelect () {

   remove_rtti_interface (ObjectModuleSelectInterfaceName, __Info);
}

#endif // DMZ_OBJECT_MODULE_SELECT_DOT_H
