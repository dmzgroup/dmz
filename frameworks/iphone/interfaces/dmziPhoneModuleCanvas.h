#ifndef DMZ_IPHONE_MODULE_CANVAS_DOT_H
#define DMZ_IPHONE_MODULE_CANVAS_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>

@class UIView;


namespace dmz {

   const char iPhoneModuleCanvasInterfaceName[] = "iPhoneModuleCanvasInterface";


   class iPhoneModuleCanvas {

      public:
         static iPhoneModuleCanvas *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");
      
         String get_iphone_module_canvas_name () const;
         Handle get_iphone_module_canvas_handle () const;
      
         virtual UIView *get_view () const = 0;

         virtual Boolean add_item (const Handle ObjectHandle, UIView *item) = 0;
         virtual UIView *lookup_item (const Handle ObjectHandle) = 0;
         virtual UIView *remove_item (const Handle ObjectHandle) = 0;

      protected:
         iPhoneModuleCanvas (const PluginInfo &Info);
         ~iPhoneModuleCanvas ();

      private:
         iPhoneModuleCanvas ();
         iPhoneModuleCanvas (const iPhoneModuleCanvas &);
         iPhoneModuleCanvas &operator= (const iPhoneModuleCanvas &);

         const PluginInfo &__Info;
   };
}


inline dmz::iPhoneModuleCanvas *
dmz::iPhoneModuleCanvas::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (iPhoneModuleCanvas *)lookup_rtti_interface (
      iPhoneModuleCanvasInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::iPhoneModuleCanvas::iPhoneModuleCanvas (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (iPhoneModuleCanvasInterfaceName, __Info, (void *)this);
}


inline
dmz::iPhoneModuleCanvas::~iPhoneModuleCanvas () {

   remove_rtti_interface (iPhoneModuleCanvasInterfaceName, __Info);
}


inline dmz::String
dmz::iPhoneModuleCanvas::get_iphone_module_canvas_name () const { return __Info.get_name (); }


inline dmz::Handle
dmz::iPhoneModuleCanvas::get_iphone_module_canvas_handle () const { return __Info.get_handle (); }


#endif // DMZ_IPHONE_MODULE_CANVAS_DOT_H
