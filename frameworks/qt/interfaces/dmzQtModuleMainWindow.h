#ifndef DMZ_QT_MODULE_MAIN_WINDOW_DOT_H
#define DMZ_QT_MODULE_MAIN_WINDOW_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <QtCore/Qt>

class QDockWidget;
class QMenuBar;
class QStatusBar;
class QToolBar;
class QWidget;


namespace dmz {

   //! \cond
   const char QtModuleMainWindowInterfaceName[] = "QtModuleMainWindowInterface";
   //! \endcond

   class QtModuleMainWindow {

      public:
         static QtModuleMainWindow *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         virtual QString get_window_name () = 0;
         virtual QWidget *get_widget () = 0;
         virtual QMenuBar *get_menu_bar () = 0;
         virtual QStatusBar *get_status_bar () = 0;

         virtual Boolean add_tool_bar (QToolBar *toolBar) = 0;
         virtual Boolean add_tool_bar (const Handle Channel, QToolBar *toolBar) = 0;

         virtual Boolean remove_tool_bar (QToolBar *toolBar) = 0;
         virtual Boolean remove_tool_bar (const Handle Channel, QToolBar *toolBar) = 0;

         virtual void add_tool_bar_break (
            const Qt::ToolBarArea Area = Qt::TopToolBarArea) = 0;

         virtual Boolean add_dock_widget (
            const Qt::DockWidgetArea Area,
            QDockWidget *dock) = 0;

         virtual Boolean add_dock_widget (
            const Handle Channel,
            const Qt::DockWidgetArea Area,
            QDockWidget *dock) = 0;

         virtual Boolean remove_dock_widget (QDockWidget *dock) = 0;
         virtual Boolean remove_dock_widget (const Handle Channel, QDockWidget *dock) = 0;

         virtual Boolean add_central_widget (const Handle Channel, QWidget *widget) = 0;
         virtual Boolean remove_central_widget (const Handle Channel) = 0;

      protected:
         QtModuleMainWindow (const PluginInfo &Info);
         ~QtModuleMainWindow ();

      private:
         QtModuleMainWindow ();
         QtModuleMainWindow (const QtModuleMainWindow &);
         QtModuleMainWindow &operator= (const QtModuleMainWindow &);

         const PluginInfo &__Info;
   };
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
