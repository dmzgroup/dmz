#ifndef DMZ_QT_MODULE_MAP_DOT_H
#define DMZ_QT_MODULE_MAP_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class QtModuleMap {

      public:
         static QtModuleMap *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         String get_qt_module_map_name () const;
         Handle get_qt_module_map_handle () const;

         virtual void set_zoom_min_value (const Int32 Value) = 0;
         virtual Int32 get_zoom_min_value () const = 0;

         virtual void set_zoom_max_value (const Int32 Value) = 0;
         virtual Int32 get_zoom_max_value () const = 0;
         
         virtual void set_zoom (const Int32 Value) = 0;
         virtual Int32 get_zoom () const = 0;

         virtual void zoom_in () = 0;
         virtual void zoom_out () = 0;
         //virtual void zoom_extents () = 0;
         
         virtual void pan_direction (const Int32 Dx, const Int32 Dy) = 0;

         //virtual void center_on (const Handle ObjectHandle) = 0;

      protected:
         QtModuleMap (const PluginInfo &Info);
         ~QtModuleMap ();

      private:
         QtModuleMap ();
         QtModuleMap (const QtModuleMap &);
         QtModuleMap &operator= (const QtModuleMap &);

         const PluginInfo &__Info;
   };

   //! \cond
   const char QtModuleMapInterfaceName[] = "QtModuleMapInterface";
   //! \endcond
};


inline dmz::QtModuleMap *
dmz::QtModuleMap::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (QtModuleMap *)lookup_rtti_interface (
      QtModuleMapInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::QtModuleMap::QtModuleMap (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (QtModuleMapInterfaceName, __Info, (void *)this);
}


inline
dmz::QtModuleMap::~QtModuleMap () {

   remove_rtti_interface (QtModuleMapInterfaceName, __Info);
}


inline dmz::String
dmz::QtModuleMap::get_qt_module_map_name () const { return __Info.get_name (); }


inline dmz::Handle
dmz::QtModuleMap::get_qt_module_map_handle () const { return __Info.get_handle (); }

#endif // DMZ_QT_MODULE_MAP_DOT_H
