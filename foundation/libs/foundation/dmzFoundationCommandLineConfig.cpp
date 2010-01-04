#include <dmzFoundationCommandLine.h>
#include <dmzFoundationCommandLineConfig.h>
#include <dmzFoundationConfigFileIO.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeLog.h>
#include <dmzSystemFile.h>
#include <dmzTypesString.h>

/*!

\class dmz::CommandLineConfig
\brief Process the command line.
\details Takes the parsed command line and parse the specified config files and
converts them into a config context tree.
\sa dmz::Config \n dmz::ConfigContext

*/

struct dmz::CommandLineConfig::State {

   String error;
   PathContainer paths;
};


//! Default constructor.
dmz::CommandLineConfig::CommandLineConfig () : _state (*(new State)) {

}


/*!

\brief Search path constructor.
\param[in] Container PathContainer with paths to use in locating the config files.

*/
dmz::CommandLineConfig::CommandLineConfig (const PathContainer &Container) :
      _state (*(new State)) {

   _state.paths = Container;
}


//! Destructor.
dmz::CommandLineConfig::~CommandLineConfig () { delete &_state; }


/*!

\brief Set file search paths.
\details Sets the search path used to find config files specified on the command line.
Will overwrite any existing search path data.
\param[in] Container PathContainer with paths to use in locating the config files.

*/
void
dmz::CommandLineConfig::set_search_path (const PathContainer &Container) {

   _state.paths = Container;
}


/*!

\brief Process command line.
\details Process the command line and attempts to parse the specified
config files. The command line should be formatted as follows:
\verbatim
% application -f <file 1> ... <file N>
\endverbatim
The "-f" may occur more than once one the command line.
\param[in] Opts CommandLine of parsed command line arguments.
\param[out] globalData Config object containing the root of the config context tree.
\param[in] log Pointer to the Log to use for logging messages. May be NULL.

*/
dmz::Boolean
dmz::CommandLineConfig::process_command_line (
      const CommandLine &Opts,
      Config &globalData,
      Log *log) {

   CommandLineArgs args;
   Boolean error (False);
   Boolean done (!Opts.get_first_option (args));

   while (!done) {

      if (args.get_name ().get_lower () == "f") {

         String file, foundFile;

         Boolean nextArg (args.get_first_arg (file));

         while (nextArg) {

            if (find_file (_state.paths, file, foundFile)) {

               if (!read_config_file (foundFile, globalData, FileTypeAutoDetect, log)) {

                  error = True;
                  _state.error.flush () << "Unable to read config file: "<< foundFile;
               }
            }
            else {

               error = True;
               _state.error.flush () << "Unable to find config file: " << file;
            }

            if (error) { nextArg = False; }
            else { nextArg = args.get_next_arg (file); }
         }
      }

      if (error) { done = True; }
      else { done  = !Opts.get_next_option (args); }
   }

   if (!error) {

   }

   return !error;
}


//! Gets last parse error.
dmz::String
dmz::CommandLineConfig::get_error () { return _state.error; }

