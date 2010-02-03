#include <dmzApplication.h>
#include <dmzApplicationState.h>
#include <dmzAppShellExt.h>
#include <dmzFoundationCommandLine.h>
#include <dmzFoundationXMLUtil.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeLogObserverFile.h>
#include <dmzRuntimeMessaging.h>
#include <dmzQtLogObserver.h>
#include <dmzQtSingletonApplication.h>
#include "dmzQtSplashScreen.h"
#include <dmzSystem.h>
#include <dmzSystemDynamicLibrary.h>
#include <dmzSystemFile.h>
#include <QtGui/QtGui>
#include <time.h>

#if defined(__APPLE__) || defined(MACOSX)
#include <CoreServices/CoreServices.h>
#endif

using namespace dmz;

namespace {

static dmz::Log *appLog (0);

const char OrganizationName[] = "dmz";
const char OrganizationDomain[] = "dmzdev.org";

static const String LocalInitPrefix ("dmz_init_");

static void
local_starup_error (const QString &Msg) {

   QString errorMsg (Msg + "\n\nStart up errors encountered.\nShutting down.");
   QMessageBox::critical (0, "Start Up Error", errorMsg);
}


static void
local_qt_message_handler (QtMsgType type, const char *msg) {

   if (appLog) {

      const dmz::String Prefix ("qt:");

      switch (type) {

         case QtDebugMsg:
            appLog->debug << Prefix << msg << endl;
            break;

         case QtWarningMsg:
            appLog->warn << Prefix << msg << endl;
            break;

         case QtCriticalMsg:
         case QtFatalMsg:
            appLog->error << Prefix << msg << endl;
            break;

         default:
            appLog->out << Prefix << msg << endl;
            break;
      }
   }
}


static LogObserverFile *
local_create_log_file (const Application &App) {

   LogObserverFile *result (0);

   if (string_to_boolean (get_env (App.get_prefix () + "_LOG"))) {

      String logSuffix ("_log_");

      time_t clock = time (0);
      tm *tms = gmtime (&clock);

      if (tms) {

         logSuffix << (Int32)tms->tm_year + 1900
            << (tms->tm_mon < 10 ? "0" : "") << (Int32)tms->tm_mon + 1
            << (tms->tm_mday < 10 ? "0" : "") << (Int32)tms->tm_mday
            << (tms->tm_hour < 10 ? "0" : "") << (Int32)tms->tm_hour
            << (tms->tm_min < 10 ? "0" : "") << (Int32)tms->tm_min
            << (tms->tm_sec < 10 ? "0" : "") << (Int32)tms->tm_sec
            << ".txt";
      }
      else { logSuffix << "unknown.txt"; }

      result = new LogObserverFile (
         get_home_directory () + "/" + App.get_name () + logSuffix,
         App.get_context ());
   }

   return result;
}


static void
local_find_working_dir (const Application &App) {

   String result (get_env (App.get_prefix () + "_WORKING_DIR"));

#if defined(__APPLE__) || defined(MACOSX)
   if (!result) {

      CFURLRef pluginRef = CFBundleCopyBundleURL (CFBundleGetMainBundle ());

      CFStringRef macPath = CFURLCopyFileSystemPath (pluginRef, kCFURLPOSIXPathStyle);

      result = CFStringGetCStringPtr (macPath, CFStringGetSystemEncoding ());

      if (result) {

         // Running as an app. Need to tell Qt where to find the plugins.
         QString plugins (result.get_buffer ());
         plugins += "/Contents/Frameworks/Qt/plugins";
         QApplication::addLibraryPath (plugins);

         result += "/Contents/Resources";
      }

      if (!is_valid_path (result)) { result.flush (); }
   }
#endif

   if (!result) {

      QSettings settings;
      result = qPrintable (settings.value ("/workingDir", ".").toString ());
      QString plugins (result.get_buffer ());
      plugins += "/bin/plugins";
      QApplication::addLibraryPath (plugins);
   }

   if (!result) { App.log.info << "No working directory found." << endl; }
   else if (change_directory (result)) {

      App.log.info << "Working directory: " << get_current_directory () << endl;
   }
   else {

      App.log.error << "Unable to to change to working directory: " << result << endl;
   }
}


static dmz::Boolean
local_init (
      Config &manifest,
      Application &app,
      CommandLineArgs &fileList,
      String &launchFile,
      String versionFile) {

   dmz::Boolean result (False);

   const String LibName =
      config_to_string ("extension.name", manifest, app.get_name () + "Init");

   DynamicLibrary dl (LibName, DynamicLibraryModeUnload);

   if (dl.is_loaded ()) {

      const String FuncName = config_to_string (
         "extension.factory",
         manifest,
         LocalInitPrefix + app.get_name ());

      init_shell_extension func = (init_shell_extension)dl.get_function_ptr (FuncName);

      if (func) {

         app.log.info << "Launching init extension: " << FuncName << " from "
            << LibName << endl;

         AppShellInitStruct init (versionFile, launchFile, manifest, app, fileList);

         func (init);

         result = init.fileListPopulated;
      }
      else {

         app.log.error << "Init extension function: " << FuncName << " not found."
            << endl;
      }
   }
   else {

      app.log.info << "Init extension library: " << LibName << " not found." << endl;
   }

   return result;
}


static void
local_auto_save_file (Application &app, String &launchFile) {

   dmz::Boolean result (False);

   const String AutoSaveFile (app.state.get_autosave_file ());

   if (is_valid_path (AutoSaveFile)) {

      const QMessageBox::StandardButton Value (QMessageBox::question (
         0,
         "Restore",
         "Unsaved data from a previous session found."
         " Would you like to recover?",
         QMessageBox::Open | QMessageBox::Cancel,
         QMessageBox::Open));

      if (Value & QMessageBox::Open) { launchFile = AutoSaveFile; }
      else if (!remove_file (AutoSaveFile)) {

         app.log.error << "Failed removing auto save file: "
            << AutoSaveFile << endl;
      }
   }
}


static void
local_parse_manifest (String &launchFile, Application &app) {

   String manifestFile (get_env (app.get_prefix () + "_MANIFEST"));

   if (!manifestFile) {

      QSettings settings;
      manifestFile =
         qPrintable (settings.value ("/manifest", "config/manifest.xml").toString ());
   }

   Config global ("global");
   app.log.info << "Manifest file: " << manifestFile << endl;

   if (is_valid_path (manifestFile)) {

      if (xml_to_config (manifestFile, global, &(app.log))) {

         Config manifest;

         if (global.lookup_all_config_merged ("manifest", manifest)) {

            Config searchList;

            if (manifest.lookup_all_config ("search.path", searchList)) {

               ConfigIterator it;
               Config path;

               while (searchList.get_next_config (it, path)) {

                  const String Prefix (config_to_string ("prefix", path));
                  const String Value (config_to_string ("value", path));

                  if (Prefix && Value) {

                     QDir::addSearchPath (Prefix.get_buffer (), Value.get_buffer ());
                     app.log.info << "Added Search Path: [" << Prefix << "]" << Value
                        << endl;
                  }
               }
            }

            String rcc (get_env (app.get_prefix () + "_RESOURCE"));

            if (!rcc) {

               rcc = config_to_string (
                  "resource.file", manifest, app.get_name () + ".rcc");
            }

            if (rcc) {

               if (QResource::registerResource (rcc.get_buffer ())) {

                  app.log.info << "Registered resource: " << rcc << endl;
               }
            }

            local_auto_save_file (app, launchFile);

            const String VersionFile =
               config_to_string ("version.file", manifest, "./config/version.xml");

            CommandLineArgs fileList ("f");

            if (is_valid_path (VersionFile)) { fileList.append_arg (VersionFile); }

            if (!local_init (manifest, app, fileList, launchFile, VersionFile)) {

               if (launchFile) {

                  Config launchFileConfig ("launch-file");
                  launchFileConfig.store_attribute ("name", launchFile);
                  app.add_config ("", launchFileConfig);
                  fileList.append_arg (launchFile);
               }

               Config configList;

               if (manifest.lookup_all_config ("config", configList)) {

                  ConfigIterator it;
                  Config config;

                  while (configList.get_next_config (it, config)) {

                     const String Value = config_to_string ("file", config);

                     if (Value) { fileList.append_arg (Value); }
                  }

                  CommandLine cl;
                  cl.add_args (fileList);
                  app.process_command_line (cl);
               }
            }

#if 0
            String qss (":/");
            qss << app.get_name ().get_buffer () << ".qss";
            qss = config_to_string ("stylesheet.file", manifest, qss);

            if (qss) {

               QFile file (qss.get_buffer ());
               if (file.open (QFile::ReadOnly)) {

                  QString styleSheet (QLatin1String (file.readAll ()));
                  qtapp.setStyleSheet (styleSheet);

                  app.log.info << "Application Style Sheet: " << qss << endl;
               }
            }
#endif
         }
      }
      else {

         String msg ("Failed to parse manifest file: ");
         msg << manifestFile;
         app.set_error (msg);
      }
   }
   else {

      String msg ("Unable to find manifest file: ");
      msg << manifestFile;
      app.set_error (msg);
   }
}

};


int
main (int argc, char *argv[]) {

   String path, name, ext;
   split_path_file_ext (argv[0], path, name, ext);

   //QCoreApplication::setAttribute (Qt::AA_MacPluginApplication);

   Application app (name.get_lower (), OrganizationName);

   if (!appLog) { appLog = &(app.log); }

   String launchFile;

#if defined(__APPLE__) || defined(MACOSX)
   if ((argc > 1) && argv[1]) {

      launchFile = argv[1];

      if (launchFile.contains_sub ("-psn_")) {

         if ((argc > 2) && argv[2]) { launchFile = argv[2]; }
         else { launchFile.flush (); }
      }
   }
#else
   if ((argc > 1) && argv[1]) { launchFile = argv[1]; }
#endif

#if defined(__APPLE__) || defined(MACOSX) || defined (_WIN32)
   QtSingletonApplication qtApp (
      app.get_name ().get_buffer (),
      argc,
      argv,
      app.get_context ());

   if (!qtApp.start_application ()) {

      if (argc > 1) { qtApp.send_to_running_application (argv[1]); }
      return 0; // Application is already running so just bail out now -rb
   }

   QApplication::sendPostedEvents (0, 0);
   QApplication::processEvents ();

   if (!qtApp.get_requested_file ().isEmpty ()) {

      launchFile = qPrintable (qtApp.get_requested_file ());
   }
#else
   QApplication qtApp (argc, argv);
#endif

   // Set up the custom qWarning/qDebug custom handler
   qInstallMsgHandler (local_qt_message_handler);

   QCoreApplication::setOrganizationName (OrganizationName);
   QCoreApplication::setOrganizationDomain (OrganizationDomain);
   QCoreApplication::setApplicationName (app.get_name ().get_buffer ());
   
   app.state.set_autosave_file (
      get_home_directory () + "/." + app.get_prefix () + "_AUTO_SAVE_FILE");

   //qsrand (QTime (0,0,0).secsTo (QTime::currentTime ()));
   srand (QDateTime::currentDateTime ().toTime_t ());

   LogObserverFile *logFile = local_create_log_file (app);

   QtLogObserver qtLogObs (app.get_context ());
   qtLogObs.set_process_updates (True);

   app.load_session ();
   qtLogObs.load_session ();

   local_find_working_dir (app);

   local_parse_manifest (launchFile, app);

   if (app.is_running ()) {

      QtSplashScreen *splash = create_splash_screen (app);

      app.load_plugins ();
      app.start ();

      qtLogObs.set_process_updates (False);

      if (splash) { delete splash; splash = 0; }
      
      // This little hack is needed to get the main menubar to
      // show up correctly under OSX when using Qt 4.6 -ss
      QEventLoop dummyLoop;
      QTimer::singleShot(0, &dummyLoop, SLOT (quit ()));
      dummyLoop.exec ();

      do {

         QApplication::sendPostedEvents (0, 0);
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

      DynamicLibrary::dump_loaded (app.log.info);

      while (qtLogObs.isVisible () && !app.is_forced ()) {

         // wait for log window to close
         QApplication::sendPostedEvents (0, 0);
         QApplication::processEvents (QEventLoop::WaitForMoreEvents);
      }
   }
#if 0
   else {

      QString errorMsg ("Unable to process manifest file.\n");
      String appError = app.get_error ();
      if (appError) { errorMsg = appError.get_buffer (); }
      local_starup_error (errorMsg);
   }
#endif

   qtApp.quit ();

   appLog = 0;

   if (logFile) { delete logFile; logFile = 0; }

   return app.is_error () ? -1 : 0;
}
