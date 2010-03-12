#ifndef DMZ_APP_SHELL_EXT_DOT_H
#define DMZ_APP_SHELL_EXT_DOT_H

#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

class Application;
class CommandLineArgs;
class Config;
class RuntimeContext;

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

\brief Typedef of function pointer used to invoke the shell initialization extension.
\ingroup Foundation

*/
typedef void *(*init_shell_extension) (AppShellInitStruct &init);


/*!

\brief Struct used in shell resources extension.
\ingroup Foundation

*/
struct AppShellResourcesStruct {
   
   const String &ExtName; //!< Name of extension library name.
   Application &app; //!< Reference to the Application object.
   //! Return value to indicate if all resources were validated by the
   //! resources extension.
   Boolean validated;
   
   //! \cond
   AppShellResourcesStruct (
      const String &TheExtName,
      Application &theApp) :
      ExtName (TheExtName),
      app (theApp),
      validated (False) {;}
   //! \endcond
};

/*!

\brief Typedef of function pointer used to invoke the shell resources extension.
\ingroup Foundation

*/
typedef void *(*validate_resources_extension) (AppShellResourcesStruct &resources);

};

#endif // DMZ_APP_SHELL_EXT_DOT_H
