#ifndef DMZ_QT_MODULE_MAIN_WINDOW_DOT_H
#define DMZ_QT_MODULE_MAIN_WINDOW_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <QtCore/Qt>

class QAction;
class QDockWidget;
class QMainWindow;
class QMenu;
class QWidget;


namespace dmz {

   class QtModuleMainWindow {

      public:
         static QtModuleMainWindow *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         virtual QString get_window_name () = 0;
         virtual QMainWindow *get_qt_main_window () = 0;

         virtual QMenu *lookup_menu (const String &Text) = 0;

         virtual void add_menu_separator (const String &MenuName) = 0;

         virtual void add_menu_action (
            const String &MenuName,
            QAction *action) = 0;

         virtual void insert_menu_separator (
            const String &MenuName,
            QAction *beforeAction) = 0;

         virtual void insert_menu_action (
            const String &MenuName,
            QAction *beforeAction,
            QAction *action) = 0;

         virtual void remove_menu_action (
            const String &MenuName,
            QAction *action) = 0;

         virtual QDockWidget *create_dock_widget (
            const String &DockName,
            const Qt::DockWidgetArea Area,
            QWidget *widget) = 0;

        virtual QDockWidget *lookup_dock_widget (const String &DockName) = 0;

        virtual Boolean remove_dock_widget (const String &DockName) = 0;

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
