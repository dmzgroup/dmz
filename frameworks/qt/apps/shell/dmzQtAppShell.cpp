#include <dmzApplication.h>
#include <dmzApplicationState.h>
#include <dmzCommandLine.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeLogObserverBasic.h>
#include <dmzRuntimeMessaging.h>
#include <dmzQtLogObserver.h>
#include <dmzQtSingletonApplication.h>
#include "dmzQtSplashScreen.h"
#include <dmzSystem.h>
#include <dmzSystemFile.h>
#include <dmzXMLUtil.h>
#include <QtGui/QtGui>

#if defined(__APPLE__) || defined(MACOSX)
#include <CoreServices/CoreServices.h>
#endif

using namespace dmz;

namespace {

   static dmz::Log *appLog (0);

   const char OrganizationName[] = "dmz";
   const char OrganizationDomain[] = "dmz.nps.edu";

   void local_starup_error (const QString &Msg) {

      QString errorMsg (Msg + "\n\nStart up errors encountered.\nShutting down.");
      QMessageBox::critical (0, "Start Up Error", errorMsg);
   }

   void qt_message_handler (QtMsgType type, const char *msg) {

      if (appLog) {

         const dmz::String PreFix ("qt:");

         switch (type) {

            case QtDebugMsg:
               appLog->debug << PreFix << msg << endl;
               break;

            case QtWarningMsg:
               appLog->warn << PreFix << msg << endl;
               break;

            case QtCriticalMsg:
            case QtFatalMsg:
               appLog->error << PreFix << msg << endl;
               break;
         }
      }
   }
};


int
main (int argc, char *argv[]) {

   String path, file, ext;
   split_path_file_ext (argv[0], path, file, ext);
   const String AppPrefix = file.get_upper ();
   const String AppName = file.get_lower ();

   // Q_INIT_RESOURCE (mbra);

   Application app (AppName, OrganizationName);

   if (!appLog) { appLog = &(app.log); }

#if defined(__APPLE__) || defined(MACOSX) || defined (_WIN32)
   QtSingletonApplication qtApp (
      AppName.get_buffer (),
      argc,
      argv,
      app.get_context ());

   if (!qtApp.start_application ()) {

      if (argc > 1) { qtApp.send_to_running_application (argv[1]); }
      return 0; // Application is already running so just bail out now -rb
   }
#else
   QApplication qtApp (argc, argv);
#endif

   // Set up the custom qWarning/qDebug custom handler
   qInstallMsgHandler (qt_message_handler);

   QCoreApplication::setOrganizationName (OrganizationName);
   QCoreApplication::setOrganizationDomain (OrganizationDomain);
   QCoreApplication::setApplicationName (AppName.get_buffer ());

   app.state.set_autosave_file (
      get_home_directory () + "/." + AppPrefix + "_AUTO_SAVE_FILE");

   qsrand (QTime (0,0,0).secsTo (QTime::currentTime ()));

   LogObserverBasic logObs (app.get_context ());
   logObs.set_level (string_to_log_level (get_env (AppPrefix + "_LOG_LEVEL")));

   QtLogObserver qtLogObs (app.get_context ());
   qtLogObs.set_process_updates (True);

   app.load_session ();
   qtLogObs.load_session ();

   QSettings settings;
   String workingDir (get_env (AppPrefix + "_WORKING_DIR"));

#if defined(__APPLE__) || defined(MACOSX)
   if (!workingDir) {

      CFURLRef pluginRef = CFBundleCopyBundleURL (CFBundleGetMainBundle ());

      CFStringRef macPath =
      CFURLCopyFileSystemPath (pluginRef, kCFURLPOSIXPathStyle);

      workingDir =
         CFStringGetCStringPtr (macPath, CFStringGetSystemEncoding ());

      if (workingDir) {

         // Running as an app. Need to tell Qt where to find the plugins.
         QString plugins (workingDir.get_buffer ());
         plugins += "/Contents/Frameworks/Qt/plugins";
         QApplication::addLibraryPath (plugins);

         workingDir += "/Contents/Resources";
      }

      if (!is_valid_path (workingDir)) { workingDir.flush (); }
   }
#endif

   if (!workingDir) {

      workingDir = qPrintable (settings.value ("/workingDir", ".").toString ());
      QString plugins (workingDir.get_buffer ());
      plugins += "/bin/plugins";
      QApplication::addLibraryPath (plugins);
   }

   if (!workingDir) { app.log.info << "No working found." << endl; }
   else if (change_directory (workingDir)) {

      app.log.info << "Working directory: " << get_current_directory () << endl;
   }
   else {

      app.log.error << "Unable to to change to working directory: " << workingDir << endl;
   }

   QtSplashScreen *splash = create_splash_screen (AppName, app.get_context ());

   if (splash) { splash->show (); splash->raise (); }

   QString manifestFile (get_env (AppPrefix + "_MANIFEST").get_buffer ());

   if (manifestFile.isEmpty ()) {

      manifestFile = settings.value ("/manifest", "config/manifest.xml").toString ();
   }

   QFileInfo fi (manifestFile);
   manifestFile = fi.absoluteFilePath ();

   CommandLineArgs fileArgs ("f");

   Config global ("global");
   app.log.info << "Manifest file: " << qPrintable (manifestFile) << endl;

   if (fi.exists ()) {

      if (xml_to_config (qPrintable (manifestFile), global, &(app.log))) {

         Config manifest;

         if (global.lookup_all_config_merged ("manifest", manifest)) {

            String rcc (get_env (AppPrefix + "_RESOURCE"));

            if (!rcc) {

               rcc = config_to_string (
                  "resource.file", manifest, AppName + ".rcc");
            }

            if (rcc) {

               if (QResource::registerResource (rcc.get_buffer ())) {

                  app.log.info << "Registered resource: " << rcc << endl;
               }
            }

            ConfigIterator it;
            Config cd;
            String value;

            while (manifest.get_next_config (it, cd)) {

               const String DataName (cd.get_name ().to_lower ());

               if (DataName == "config") {

                  if (cd.lookup_attribute ("file", value)) {

                     fileArgs.append_arg (value);
                  }
               }
               else if (DataName == "searchpath") {

                  const String Prefix (config_to_string ("prefix", cd));
                  const String Path (config_to_string ("path", cd));

                  if (Prefix && Path) {

                     QDir::addSearchPath (Prefix.get_buffer (), Path.get_buffer ());
                     app.log.info << "Added Search Path: [" << Prefix << "]" << Path
                        << endl;
                  }
               }
            }

            String loadFile;

            if ((argc > 1) && argv[1]) { loadFile = argv[1]; }

#if defined(__APPLE__) || defined(MACOSX)
           else if (!qtApp.get_requested_file ().isEmpty ()) {

              loadFile = qPrintable (qtApp.get_requested_file ());
           }
#endif

            const String AutoSaveFile (app.state.get_autosave_file ());

            if (is_valid_path (AutoSaveFile)) {

               const QMessageBox::StandardButton Value (QMessageBox::question (
                  0,
                  "Restore",
                  "Unsaved data from a previous session found."
                  " Would you like to recover?",
                  QMessageBox::Open | QMessageBox::Cancel,
                  QMessageBox::Open));

               if (Value & QMessageBox::Open) { loadFile.flush (); }
               else if (!remove_file (AutoSaveFile)) {

                  app.log.error << "Failed removing auto save file: "
                     << AutoSaveFile << endl;
               }
            }

            if (loadFile) { fileArgs.append_arg (loadFile); }

            if (loadFile) {

               Config startFileConfig ("mbraStartFile");
               startFileConfig.store_attribute ("name", loadFile);
               app.add_config ("", startFileConfig);
            }

            if (is_valid_path ("./config/version.xml")) {

               fileArgs.append_arg ("./config/version.xml");
            }

            String qss (":/");
            qss << AppName.get_buffer () << ".qss";
            qss = config_to_string ("stylesheet.file", manifest, qss);

            if (qss) {

               QFile file (qss.get_buffer ());
               if (file.open (QFile::ReadOnly)) {

                  QString styleSheet (QLatin1String (file.readAll ()));
                  qtApp.setStyleSheet (styleSheet);

                  app.log.info << "Application Style Sheet: " << qss << endl;
               }
            }
         }
      }
   }

   if (fileArgs.get_count ()) {

      CommandLine cl;
      cl.add_args (fileArgs);

      app.process_command_line (cl);
      app.load_plugins ();
      app.start ();

      qtLogObs.set_process_updates (False);

      if (splash) { delete splash; splash = 0; }

      do {

         QApplication::sendPostedEvents (0, -1);
         QApplication::processEvents ();

      } while (app.update_time_slice ());

      app.stop ();
      app.unload_plugins ();

      if (app.is_error ()) {

         local_starup_error (app.get_error ().get_buffer ());
      }
      else {

         qtLogObs.save_session ();
         app.save_session ();
      }

      while (qtLogObs.isVisible ()) {

         // wait for log window to close
         QApplication::sendPostedEvents (0, -1);
         QApplication::processEvents ();
      }
   }
   else {

      if (splash) { delete splash; splash = 0; }
      QString errorMsg ("Unable to process manifest:\n");
      local_starup_error (errorMsg + manifestFile);
   }

   qtApp.quit ();

   appLog = 0;

   return app.is_error () ? -1 : 0;
}
