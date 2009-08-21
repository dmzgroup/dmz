#ifndef DMZ_QT_MODULE_MAIN_WINDOW_DOT_H
#define DMZ_QT_MODULE_MAIN_WINDOW_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <QtCore/Qt>

class QMainWindow;


namespace dmz {

   class QtModuleMainWindow {

      public:
         static QtModuleMainWindow *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         virtual QString get_window_name () = 0;
         virtual QMainWindow *get_qt_main_window () = 0;

      protected:
         QtModuleMainWindow (const PluginInfo &Info);
         ~QtModuleMainWindow ();

      private:
         QtModuleMainWindow ();
         QtModuleMainWindow (const QtModuleMainWindow &);
         QtModuleMainWindow &operator= (const QtModuleMainWindow &);

         const PluginInfo &__Info;
   };

   //! \cond
   const char QtModuleMainWindowInterfaceName[] = "QtModuleMainWindowInterface";
   //! \endcond
}


inline dmz::QtModuleMainWindow *
dmz::QtModuleMainWindow::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (QtModuleMainWindow *)lookup_rtti_interface (
      QtModuleMainWindowInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::QtModuleMainWindow::QtModuleMainWindow (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (QtModuleMainWindowInterfaceName, __Info, (void *)this);
}


inline
dmz::QtModuleMainWindow::~QtModuleMainWindow () {

   remove_rtti_interface (QtModuleMainWindowInterfaceName, __Info);
}


#endif // DMZ_QT_MODULE_MAIN_WINDOW_DOT_H
