#include <dmzApplication.h>
#include <dmzAppShellExt.h>
#include <dmzFoundationCommandLine.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeLogObserverBasic.h>
#include <dmzQtLogObserver.h>
#include "dmzQtSplashScreen.h"
#include <dmzSystem.h>
#include <dmzSystemDynamicLibrary.h>
#include <dmzSystemFile.h>
#include <QtGui/QtGui>


using namespace dmz;

namespace {
   
const char LocalIconFile[] = "./assets/DMZ-Icon.png";
static const String LocalResourcesFactory ("dmz_resources_validate");

static void
local_starup_error (const QString &Msg) {

   QString errorMsg (Msg + "\n\nStart up errors encountered.\nShutting down.");
   QMessageBox::critical (0, "Start Up Error", errorMsg);
}

static void
local_validate_resources (Application &app) {

   Config global;

   app.get_global_config (global);

// <resources>
//    <extension name="dmzQtExtAppShellResources" factory="dmz_validate_resources"/>
// </resources>

   const String LibName = config_to_string ("dmz.resources.extension.name", global);

   if (LibName) {

      DynamicLibrary dl (LibName, DynamicLibraryModeKeep);

      if (dl.is_loaded ()) {

         const String FuncName = config_to_string (
            "dmz.resources.extension.factory", global, LocalResourcesFactory);

         validate_resources_extension validate =
            (validate_resources_extension)dl.get_function_ptr (FuncName);

         if (validate) {

            app.log.info << "Validating Resoruces using: " << FuncName << " from "
               << LibName << endl;

            AppShellResourcesStruct resources (LibName, app);

            validate (resources);
         }
         else {
            
            String msg ("Resource extension function: ");
            msg << FuncName << " not found.";
            
            app.log.error << msg << endl;
            app.quit (msg);
         }
      }
      else {

         String msg ("Resource extension library: ");
         msg << LibName << " not found.";
         app.log.error << msg << endl;
         app.quit (msg);
      }
   }
}

};


int
main (int argc, char *argv[]) {

   Q_INIT_RESOURCE (dmzAppQt);

//   QCoreApplication::setAttribute (Qt::AA_MacPluginApplication);

   QApplication qtApp (argc, argv);

   Application app ("dmzAppQt", "dmz");

   //qsrand (QTime (0,0,0).secsTo (QTime::currentTime ()));
   srand (QDateTime::currentDateTime ().toTime_t ());
   //sranddev ();

   QPixmap iconPixmap (
      is_valid_path (LocalIconFile) ? LocalIconFile : ":/AppQt/DMZ-Splash.png");

   iconPixmap.scaledToWidth (30, Qt::SmoothTransformation);
   qtApp.setWindowIcon (QIcon (iconPixmap));

   LogObserverBasic logObs (app.get_context ());
   logObs.set_level (string_to_log_level (get_env ("DMZ_APP_LOG_LEVEL")));

   QtLogObserver qtLogObs (app.get_context ());
   qtLogObs.set_process_updates (True);

   app.load_session ();
   qtLogObs.load_session ();

   CommandLine cl (argc, argv);

   app.process_command_line (cl);
   app.init_runtime ();
   
   local_validate_resources (app);
   
   if (app.is_running ()) {
      
      QtSplashScreen *splash = new QtSplashScreen (app.get_context ());
      splash->show ();
      splash->raise ();

      app.load_plugins ();
      app.start ();

      qtLogObs.set_process_updates (False);

      splash->close ();
      delete splash;
      splash = 0;

      // This little hack is needed to get the main menubar to
      // show up correctly under OSX when using Qt 4.6 -ss
      QEventLoop dummyLoop;
      QTimer::singleShot(0, &dummyLoop, SLOT (quit ()));
      dummyLoop.exec ();

      do {

         QCoreApplication::sendPostedEvents (0, 0);
         QCoreApplication::processEvents (QEventLoop::AllEvents);

      } while (app.update_time_slice ());

      app.stop ();
      app.unload_plugins ();

      if (app.is_error ()) {
         
         app.log.error << app.get_error () << endl;
         local_starup_error (app.get_error ().get_buffer ());
      }
      else {

         qtLogObs.save_session ();
         app.save_session ();
      }

      DynamicLibrary::dump_loaded (app.log.info);

      while (qtLogObs.isVisible ()) {

         // wait for log window to close
         QCoreApplication::sendPostedEvents (0, 0);
         QCoreApplication::processEvents (QEventLoop::WaitForMoreEvents);
      }
   }
   else if (app.is_error ()) {
      
      app.log.error << app.get_error () << endl;
      local_starup_error (app.get_error ().get_buffer ());
   }
   
   qtApp.quit ();

   return app.is_error () ? -1 : 0;
}
