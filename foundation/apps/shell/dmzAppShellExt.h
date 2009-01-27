#ifndef DMZ_APP_SHELL_EXT_DOT_H
#define DMZ_APP_SHELL_EXT_DOT_H

#include <dmzTypesBase.h>

namespace dmz {

class Application;
class CommandLineArgs;
class Config;

struct AppShellInitStruct {

   Config &manifest;
   Application &app;
   CommandLineArgs &files;
   String &launchFile;

   AppShellInitStruct (
         Config &theManifest,
         Application &theApp,
         CommandLineArgs &theFiles,
         String &theLaunchFile) :
         manifest (theManifest),
         app (theApp),
         files (theFiles),
         launchFile (theLaunchFile) {;}
};

typedef Boolean *(*init_shell_extension) (AppShellInitStruct &init);

};

#endif // DMZ_APP_SHELL_EXT_DOT_H
