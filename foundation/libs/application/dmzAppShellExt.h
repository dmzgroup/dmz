#ifndef DMZ_APP_SHELL_EXT_DOT_H
#define DMZ_APP_SHELL_EXT_DOT_H

#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

class Application;
class CommandLineArgs;
class Config;

/*!

\brief Struct used in shell initialization extension.
\ingroup Foundation

*/
struct AppShellInitStruct {

   const String VersionFile; //!< Config file containing versioning information.
   String &launchFile; //!< File used to launch application.
   Config &manifest; //!< Config of application manifest file.
   Application &app; //!< Reference to the Application object.
   CommandLineArgs &files; //!< List of config files to load.
   //! Return value to indicate if the file list has been populated by the
   //! initialization extension.
   Boolean fileListPopulated;

   //! \cond
   AppShellInitStruct (
         const String &TheVersionFile, 
         String &theLaunchFile,
         Config &theManifest,
         Application &theApp,
         CommandLineArgs &theFiles) :
         launchFile (theLaunchFile),
         VersionFile (TheVersionFile),
         manifest (theManifest),
         app (theApp),
         files (theFiles),
         fileListPopulated (True) {;}
   //! \endcond
};

/*!

\brief Typedef of function pointer used to invoke  the shell initialization extension.
\ingroup Foundation

*/
typedef void *(*init_shell_extension) (AppShellInitStruct &init);

};

#endif // DMZ_APP_SHELL_EXT_DOT_H
