#include <dmzApplication.h>
#include <dmzCommandLine.h>
#include <dmzRuntimeLogObserverBasic.h>
#include <dmzQtLogObserver.h>
#include "dmzQtSplashScreen.h"
#include <dmzSystem.h>
#include <QtGui/QtGui>

using namespace dmz;


int
main (int argc, char *argv[]) {
   
   Q_INIT_RESOURCE (dmzAppQt);
   
   QApplication qtApp (argc, argv); 
   
   Application app ("dmzAppQt", "dmz");

   qsrand (QTime (0,0,0).secsTo (QTime::currentTime ()));

   QPixmap iconPixmap (":/AppQt/DMZ-Splash.png");
   iconPixmap.scaledToWidth (30, Qt::SmoothTransformation);
   qtApp.setWindowIcon (QIcon (iconPixmap));

   LogObserverBasic logObs (app.get_context ());
   logObs.set_level (string_to_log_level (get_env ("DMZ_APP_LOG_LEVEL")));

   QtLogObserver qtLogObs (app.get_context ());
   qtLogObs.set_process_updates (True);

   app.load_session ();
   qtLogObs.load_session ();
   
   QtSplashScreen *splash = new QtSplashScreen (app.get_context ());
   splash->show ();

   CommandLine cl (argc, argv);

   app.process_command_line (cl);
   app.load_plugins ();
   app.start ();

   qtLogObs.set_process_updates (False);

   splash->close ();
   delete splash;
   splash = 0;

   do {

      qtApp.processEvents ();

   } while (app.sync ());

   app.stop ();
   app.unload_plugins ();

   if (app.is_error ()) {

      String errorMsg (app.get_error ());
      errorMsg << "\n\nStart up errors encountered.\nShutting down.";
      
      QMessageBox::critical (0, "Start Up Error", errorMsg.get_buffer ());
   }
   else {

      qtLogObs.save_session ();
      app.save_session ();
   }

   while (qtLogObs.isVisible ()) {

      // wait for log window to close
      qtApp.processEvents ();
   }
      
   qtApp.quit ();

   return app.is_error () ? -1 : 0;
}
