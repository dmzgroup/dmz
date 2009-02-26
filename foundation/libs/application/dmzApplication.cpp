#include <dmzApplication.h>
#include "dmzApplicationStateBasic.h"
#include <dmzCommandLine.h>
#include <dmzCommandLineConfig.h>
#include <dmzFileCacheLocal.h>
#include <dmzRuntime.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeExit.h>
#include <dmzRuntimeInit.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePluginContainer.h>
#include <dmzRuntimeLoadPlugins.h>
#include <dmzRuntimeSession.h>
#include <dmzRuntimeTime.h>
#include <dmzSystem.h>
#include <dmzSystemFile.h>
#include <dmzSystemStreamFile.h>
#include <dmzTypesBase.h>
#include <dmzXMLUtil.h>

namespace {

static const dmz::String ApplicationName ("dmzApplication");
static const dmz::String DirName ("default_directory");
static const dmz::String DefaultDirName (ApplicationName + "." + DirName);

};

/*!

\defgroup Foundation DMZ Foundation Layer

\class dmz::Application
\ingroup Foundation
\brief Simple DMZ application class.
\details Class provides basic DMZ application functionality. The class
will create the runtime context, parse the command line, load the XML configuration
files, manage session data, create the file cache, and manage plugins.

*/

struct dmz::Application::State {

   const String Name;
   const String NamePrefix;
   const String Domain;
   Runtime rt;
   Time time;
   Log log;
   Exit exit;
   ExitObserverBasic exitObs;
   FileCacheLocal cache;
   ApplicationStateBasic appState;
   Config global;
   PluginContainer container;
   Boolean error;
   String errorMsg;
   String sessionDir;
   String sessionFile;
   String sessionPath;
   Float64 startTime;
   Float64 frameCount;
   Boolean quiet;

   State (const String &TheName, const String &TheDomain) :
         Name (TheName),
         NamePrefix (TheName.get_upper ()),
         Domain (TheDomain),
         time (rt.get_context ()),
         log (TheName, rt.get_context ()),
         exit (rt.get_context ()),
         exitObs (rt.get_context ()),
         cache (rt.get_context ()),
         appState (rt.get_context ()),
         global ("global"),
         container (&log),
         error (False),
         startTime (0.0),
         frameCount (0.0),
         quiet (False) {;}
};


/*!

\brief Constructor.
\details The \a Name and \a Domain are used in the creation of the session data path.
\param[in] Name String containing the name of the application.
\param[in] Domain String containing the application's domain.

*/
dmz::Application::Application (
      const String &Name,
      const String &Domain) :
      _state (*(new State ((Name ? Name : "dmz"), (Domain ? Domain : "dmz")))),
      log (_state.log),
      state (_state.appState) {;}


//! Destructor.
dmz::Application::~Application () { delete &_state; }


//! Tests if application is still running.
dmz::Boolean
dmz::Application::is_running () const { return !_state.exitObs.is_exit_requested (); }


//! Requestion the application quit.
void
dmz::Application::quit (const String &Reason) {

   _state.exit.request_exit (ExitStatusNormal, Reason);
}

/*!

\brief Disables all messages that are not errors or warnings.
\param[in] Value True turns quiet mode on. False turns it off. It is off by default.

*/
void
dmz::Application::set_quiet (const Boolean Value) { _state.quiet = Value; }


/*!

\brief Sets the error state.
\param[in] ErrorMessage String containing th error message.

*/
void
dmz::Application::set_error (const String &ErrorMessage) {

   _state.error = True;
   _state.errorMsg = ErrorMessage;
}


/*!

\brief Gets error state.
\return Returns dmz::True if an error has occurred.

*/
dmz::Boolean
dmz::Application::is_error () const { return _state.error; }


/*!

\brief Gets a description of the last error.
\return Returns a String containing a description of the last error.

*/
dmz::String
dmz::Application::get_error () const { return _state.errorMsg; }


/*!

\brief Gets the name of the application.
\return Returns a string containing the name of the application.

*/
dmz::String
dmz::Application::get_name () const { return _state.Name; }


/*!

\brief Gets the prefix of the application.
\return Returns a string containing the prefix of the application.
\note At this time, the prefix is the application name in all caps.

*/
dmz::String
dmz::Application::get_prefix () const { return _state.NamePrefix; }


/*!

\brief Gets runtime context.
\return Returns a pointer to the runtime context.

*/
dmz::RuntimeContext *
dmz::Application::get_context () const { return _state.rt.get_context (); }


//! Gets the root of the config context tree.
void
dmz::Application::get_global_config (Config &data) const { data = _state.global; }


//! Adds config contexts to the root of the config context tree.
void
dmz::Application::add_global_config (Config &data) {

   _state.global.add_children (data);
}


/*!

\brief Adds a config to the global config context tree.
\param[in] Name String containing scoped to store config context.
\param[in] data Config object containing config context to store.

*/
void
dmz::Application::add_config (const String &Name, Config &data) {

   _state.global.add_config (Name, data);

   _state.cache.configure (_state.global);
}


/*!

\brief Loads session data.
\details Attempts to load session data using the \a Name and \a Domain from the
constructor. The session file for the various platforms is as follows:
- Win32 \$(HOME)/\$(Domain)/\$(Name).xml
- MacOS ~/Library/Preferences/\$(Domain)/\$(Name).xml
- Linux ~/.\$(Domain)/\$(Name).xml

\return Returns dmz::True if the session data was read successfully.

*/
dmz::Boolean
dmz::Application::load_session () {

   Boolean result (True);

   _state.sessionDir = get_home_directory ();

   if (is_valid_path (_state.sessionDir)) {

#if defined (_WIN32)
      if (_state.Domain) { _state.sessionDir << "/" << _state.Domain; }
      else { _state.sessionDir << "/dmz"; }
#elif defined (__APPLE__) || defined (MACOSX)
      _state.sessionDir << "/Library/Preferences";
      if (_state.Domain) { _state.sessionDir << "/" << _state.Domain; }
      else { _state.sessionDir << "/dmz"; }
#else
      if (_state.Domain) { _state.sessionDir << "/." << _state.Domain; }
      else { _state.sessionDir << "/.dmz"; }
#endif
   }
   else { result = False; }

   if (_state.sessionDir) {

      if (_state.Name) { _state.sessionFile << _state.Name << ".xml"; }
      else { _state.sessionFile << "session.xml"; }

      _state.sessionPath = format_path (_state.sessionDir + "/" + _state.sessionFile);

      if (is_valid_path (_state.sessionPath)) {

         Config data ("global");

         if (xml_to_config (_state.sessionPath, data, &(_state.log))) {

            if (!_state.quiet) {

               _state.log.info << "Loaded session file: " << _state.sessionPath << endl;
            }

            Config session;
            _state.rt.get_session_config (session);

            if (data.lookup_all_config_merged (session.get_name (), session)) {

               const String DefaultDir (config_to_string (DefaultDirName, session));

               if (DefaultDir && is_valid_path (DefaultDir)) {

                  _state.appState.set_default_directory (DefaultDir);
               }
               else { _state.appState.set_default_directory (get_home_directory ()); }

               _state.rt.set_session_config (session);
            }
         }
         else {

            _state.log.error << "Failed parsing session file: " << _state.sessionPath
               << endl;
         }
      }
      else if (!_state.quiet) {

         _state.log.info << "Session file: " << _state.sessionPath << " does not exist"
            << endl;
         result = False;
      }
   }

   return result;
}


/*!

\brief Process the command line.
\details Loads all XML configuration files specified by the command line.
\param[in] CL CommandLine object to process.
\return Returns dmz::True if command line was successfully processed. Returns
dmz::False if there was an error parsing the XML configuration files.

*/
dmz::Boolean
dmz::Application::process_command_line (const CommandLine &CL) {

   CommandLineConfig clconfig;

   if (!clconfig.process_command_line (CL, _state.global, &(_state.log))) {

      _state.errorMsg.flush () << "Unable to process command line: "
         << clconfig.get_error ();

      _state.log.error << _state.errorMsg << endl;

      _state.error = True;
   }

   if (!_state.error) { _state.cache.configure (_state.global); }

//   format_config_to_xml (global, _state.log.debug, True);

   return !_state.error;
}


/*!

\brief Loads Plugins.
\details Loads all Plugins specified by the configuration files. Also performs Plugin
discovery.
\return Returns dmz::True if all Plugins were loaded successfully.

*/
dmz::Boolean
dmz::Application::load_plugins () {

   Config runtimeData;

   if (!_state.error && !_state.quiet &&
         !_state.global.lookup_all_config_merged ("dmz.runtime", runtimeData)) {

      _state.log.warn << "dmz.runtime not found" << endl;
   }

   if (!_state.error) {

      runtime_init (runtimeData, _state.rt.get_context (), &(_state.log));
   }

   Config pluginList;

   if (!_state.error &&
         !_state.global.lookup_all_config ("dmz.plugin-list.plugin", pluginList)) {

      _state.errorMsg.flush ()
         << "dmz.plugin-list.plugin not found. No plugins listed for loading";

      _state.log.error << _state.errorMsg << endl;

      _state.error = True;
   }

   if (!_state.error) {

      Config pluginInit;

      _state.global.lookup_all_config_merged ("dmz", pluginInit);

      dmz::load_plugins (
         _state.rt.get_context (),
         pluginList,
         pluginInit,
         _state.global,
         _state.container,
         &(_state.log));

      _state.container.discover_plugins ();
      _state.container.init_plugins ();
   }

   return !_state.error;
}


//! Starts all Plugins.
dmz::Boolean
dmz::Application::start () {

   if (!_state.error) {

      _state.container.start_plugins ();
      _state.rt.update_time_slice ();

      FileCache *fc (FileCache::get_interface (_state.rt.get_context ()));

      if (fc) { fc->process_all_requests (); }

      _state.startTime = get_time ();
      _state.frameCount = 0.0;
   }

   return !_state.error;
}


/*!

\brief TimeSlices all Plugins.
\return Returns dmz::True if time slices should continue. Returns dmz::False if and
exit has been requested.

*/
dmz::Boolean
dmz::Application::update_time_slice () {

   Boolean result (False);

   if (!_state.error) {

      result = !_state.exitObs.is_exit_requested ();

      _state.frameCount += 1.0;

      if (result) { _state.rt.update_time_slice (); }
      else {

         _state.error = (_state.exitObs.get_status () == ExitStatusError ? True : False);
         _state.errorMsg.flush () << _state.exitObs.get_reason ();

         if (!_state.quiet) {

            _state.log.out << "Exit reason: " << _state.errorMsg << endl;
         }
      }
   }

   return result;
}


//! Stops Plugins.
dmz::Boolean
dmz::Application::stop () {

   const Float64 StopTime (get_time ());
   const Float64 TimeDelta (StopTime - _state.startTime);

   _state.rt.update_time_slice ();
   _state.container.stop_plugins ();

   if (!is_zero64 (_state.frameCount) && !is_zero64 (TimeDelta) && !_state.quiet) {

      _state.log.out << "Average Frame Rate: " <<  _state.frameCount / TimeDelta
         << "Hz" << endl;
   }

   return True;
}


//! Unloads and deletes Plugins.
dmz::Boolean
dmz::Application::unload_plugins () {

   _state.container.shutdown_plugins ();
   _state.container.remove_plugins ();
   _state.container.delete_plugins ();

   return True;
}


/*!

\brief Save session data.
\details Session data is saved to the session file.
\return Returns dmz::True if the session data is successfully saved.
\sa dmz::Application::load_session()

*/
dmz::Boolean
dmz::Application::save_session () {

   Boolean result (False);

   Config session;

   _state.rt.get_session_config (session);

   Config prevDirConfig;

   const String DefaultDir (_state.appState.get_default_directory ());

   if (DefaultDir) {

      if (session.lookup_config (ApplicationName, prevDirConfig)) {

         prevDirConfig.store_attribute (DirName, DefaultDir);
      }
      else if (DefaultDir != get_home_directory ()) {

         Config dirConfig (ApplicationName);
         dirConfig.store_attribute (DirName, DefaultDir);
         session.add_config (dirConfig);
      }
   }

   if (!session.is_empty () && _state.sessionDir && _state.sessionPath) {

      if (create_directory (_state.sessionDir)) {

         FILE *file = open_file (_state.sessionPath, "wb");

         if (file) {

            StreamFile fs (file);

            write_xml_header (fs);

            format_config_to_xml (session, fs);

            close_file (file); file = 0;

            result = True;

            if (!_state.quiet) {

               _state.log.info << "Saved session to file: " << _state.sessionPath << endl;
            }
         }
         else {

            _state.log.error << "Unable to create session file: "
               << _state.sessionPath << endl;
         }
      }
      else {

         _state.log.error << "Unable to create session directory: " << _state.sessionDir
            << endl;
      }
   }
   else if (session.is_empty ()) {

      if (!_state.quiet) { _state.log.info << "No session data to save." << endl; }
   }
   else if (!_state.sessionDir) {

      _state.log.error << "Session directory not found." << endl;
   }
   else if (!_state.sessionPath) {

      _state.log.error << "Session file not found." << endl;
   }

   return result;
}
