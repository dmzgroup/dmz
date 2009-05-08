#ifndef DMZ_QT_MODULE_CANVAS_DOT_H
#define DMZ_QT_MODULE_CANVAS_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>

class QGraphicsItem;
class QGraphicsScene;
class QGraphicsView;
class QPointF;


namespace dmz {

   //! \cond
   const char QtModuleCanvasInterfaceName[] = "QtModuleCanvasInterface";
   //! \endcond

   const UInt32 QtCanvasObjectNameIndex (0);
   const UInt32 QtCanvasObjectHandleIndex (1);


   class QtModuleCanvas {

      public:
         static QtModuleCanvas *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         String get_qt_module_canvas_name () const;
         Handle get_qt_module_canvas_handle () const;

         virtual QGraphicsScene *get_scene () = 0;
         virtual QGraphicsView *get_view () = 0;
         
         virtual Boolean add_item (const Handle ObjectHandle, QGraphicsItem *item) = 0;
         virtual QGraphicsItem *lookup_item (const Handle ObjectHandle) = 0;
         virtual QGraphicsItem *remove_item (const Handle ObjectHandle) = 0;

         virtual void set_zoom_min_value (const Float32 Value) = 0;
         virtual Float32 get_zoom_min_value () const = 0;

         virtual void set_zoom_max_value (const Float32 Value) = 0;
         virtual Float32 get_zoom_max_value () const = 0;

         virtual void set_zoom_step_value (const Float32 Value) = 0;
         virtual Float32 get_zoom_step_value () const = 0;

         virtual void set_zoom (const Float32 Value) = 0;
         virtual Float32 get_zoom () const = 0;

         virtual void zoom_in () = 0;
         virtual void zoom_out () = 0;
         virtual void zoom_extents () = 0;

         virtual void pan_direction (const Int32 Dx, const Int32 Dy) = 0;

         virtual void center_on (const Handle ObjectHandle) = 0;
//         virtual void center_on (const QPointF &Value) = 0;
         
         virtual QPointF get_center () const = 0;

#if 0
         virtual void enable_dragging () = 0;
         virtual void disable_dragging () = 0;
         virtual Boolean dragging_enabled () const = 0;

         virtual void enable_double_click_zoom () = 0;
         virtual void disable_double_click_zoom () = 0;
         virtual Boolean double_click_zoom_enabled () const = 0;

         virtual void enable_scroll_wheel_zoom () = 0;
         virtual void disable_scroll_wheel_zoom () = 0;
         virtual Boolean scroll_wheel_zoom_enabled () const = 0;

         virtual void set_center (const QPoint &Center, const UInt32 Zoom) = 0;
         virtual void get_center (QPoint &center) const = 0;

         virtual void get_bounds (const SW, NE);
         virtual void get_bounds_zoom_level (Bounds);

         virtual void pan_to (const QPoint &Center) = 0;
         virtual void pan_by (const QPoint &Distance) = 0;
#endif

      protected:
         QtModuleCanvas (const PluginInfo &Info);
         ~QtModuleCanvas ();

      private:
         QtModuleCanvas ();
         QtModuleCanvas (const QtModuleCanvas &);
         QtModuleCanvas &operator= (const QtModuleCanvas &);

         const PluginInfo &__Info;
   };
}


inline dmz::QtModuleCanvas *
dmz::QtModuleCanvas::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (QtModuleCanvas *)lookup_rtti_interface (
      QtModuleCanvasInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::QtModuleCanvas::QtModuleCanvas (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (QtModuleCanvasInterfaceName, __Info, (void *)this);
}


inline
dmz::QtModuleCanvas::~QtModuleCanvas () {

   remove_rtti_interface (QtModuleCanvasInterfaceName, __Info);
}


inline dmz::String
dmz::QtModuleCanvas::get_qt_module_canvas_name () const { return __Info.get_name (); }


inline dmz::Handle
dmz::QtModuleCanvas::get_qt_module_canvas_handle () const { return __Info.get_handle (); }


#endif //  DMZ_QT_MODULE_CANVAS_DOT_H
