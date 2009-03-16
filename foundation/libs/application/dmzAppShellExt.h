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
   const String LaunchFile; //!< File used to launch application.
   Config &manifest; //!< Config of application manifest file.
   Application &app; //!< Reference to the Application object.
   CommandLineArgs &files; //!< List of config files to load.

   //! \cond
   AppShellInitStruct (
         const String &TheLaunchFile,
         const String &TheVersionFile, 
         Config &theManifest,
         Application &theApp,
         CommandLineArgs &theFiles) :
         LaunchFile (TheLaunchFile),
         VersionFile (TheVersionFile),
         manifest (theManifest),
         app (theApp),
         files (theFiles) {;}
   //! \endcond
};

/*!

\brief Typedef of function pointer used to invoke  the shell initialization extension.
\ingroup Foundation

*/
typedef void *(*init_shell_extension) (AppShellInitStruct &init);

};

#endif // DMZ_APP_SHELL_EXT_DOT_H
