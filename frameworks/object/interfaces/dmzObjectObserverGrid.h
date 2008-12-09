#ifndef DMZ_OBJECT_OBSERVER_GRID_DOT_H
#define DMZ_OBJECT_OBSERVER_GRID_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class ObjectType;
   class Volume;

   //! \brief Indicates the state of the object with regards to the observer bounding
   //! Volume
   //! \ingroup Object
   enum ObjectGridStateEnum {
      ObjectGridStateEnter, //!< Object has entered the bounding Volume.
      ObjectGridStateExit,  //!< Object has exited the bounding Volume.
   };

   class ObjectObserverGrid {

      public:
         static ObjectObserverGrid *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         String get_object_observer_grid_name () const;
         Handle get_object_observer_grid_handle () const;

         // ObjectObserverGrid Interface
         virtual const Volume &get_observer_volume () = 0;

         virtual void update_object_grid_state (
            const ObjectGridStateEnum State,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const Vector &Position) = 0;

      protected:
         ObjectObserverGrid (const PluginInfo &Info);
         ~ObjectObserverGrid ();

      private:
         ObjectObserverGrid ();
         ObjectObserverGrid (const ObjectObserverGrid &);
         ObjectObserverGrid &operator= (const ObjectObserverGrid &);

         const PluginInfo &__Info;
   };

   //! \cond
   const char ObjectObserverGridInterfaceName[] = "ObjectObserverGridInterface";
   //! \endcond
};


inline dmz::ObjectObserverGrid *
dmz::ObjectObserverGrid::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (ObjectObserverGrid *)lookup_rtti_interface (
      ObjectObserverGridInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::ObjectObserverGrid::ObjectObserverGrid (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (ObjectObserverGridInterfaceName, __Info, (void *)this);
}


inline
dmz::ObjectObserverGrid::~ObjectObserverGrid () {

   remove_rtti_interface (ObjectObserverGridInterfaceName, __Info);
}


inline dmz::String
dmz::ObjectObserverGrid::get_object_observer_grid_name () const {

   return __Info.get_name ();
}


inline dmz::Handle
dmz::ObjectObserverGrid::get_object_observer_grid_handle () const {

   return __Info.get_handle ();
}

#endif // DMZ_OBJECT_OBSERVER_GRID_DOT_H
