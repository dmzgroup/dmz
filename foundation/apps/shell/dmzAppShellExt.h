#ifndef DMZ_APP_SHELL_EXT_DOT_H
#define DMZ_APP_SHELL_EXT_DOT_H

#include <dmzTypesBase.h>

namespace dmz {

class Application;
class CommandLineArgs;
class Config;

struct AppShellInitStruct {

   const String VersionFile;
   const String LaunchFile;
   Config &manifest;
   Application &app;
   CommandLineArgs &files;

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
};

typedef void *(*init_shell_extension) (AppShellInitStruct &init);

};

#endif // DMZ_APP_SHELL_EXT_DOT_H
