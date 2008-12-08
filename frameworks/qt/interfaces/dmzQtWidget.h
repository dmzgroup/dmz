#ifndef DMZ_QT_WIDGET_DOT_H
#define DMZ_QT_WIDGET_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

class QWidget;

namespace dmz {

   class QtWidget {

      public:
         static QtWidget *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         String get_qt_widget_name () const;
         Handle get_qt_widget_handle () const;

         // QtWidget Interface
         virtual QWidget *get_qt_widget () = 0;

      protected:
         QtWidget (const PluginInfo &Info);
         ~QtWidget ();

      private:
         QtWidget ();
         QtWidget (const QtWidget &);
         QtWidget &operator= (const QtWidget &);

         const PluginInfo &__Info;
   };

   //! \cond
   const char QtWidgetInterfaceName[] = "QtWidgetInterface";
   //! \endcond
};


inline dmz::QtWidget *
dmz::QtWidget::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (QtWidget *)lookup_rtti_interface (
      QtWidgetInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::QtWidget::QtWidget (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (QtWidgetInterfaceName, __Info, (void *)this);
}


inline
dmz::QtWidget::~QtWidget () {

   remove_rtti_interface (QtWidgetInterfaceName, __Info);
}


inline dmz::String
dmz::QtWidget::get_qt_widget_name () const { return __Info.get_name (); }


inline dmz::Handle
dmz::QtWidget::get_qt_widget_handle () const { return __Info.get_handle (); }

#endif // DMZ_QT_WIDGET_DOT_H
