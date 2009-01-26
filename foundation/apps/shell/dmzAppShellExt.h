#ifndef DMZ_APP_SHELL_EXT_DOT_H
#define DMZ_APP_SHELL_EXT_DOT_H

#include <dmzTypesBase.h>

namespace dmz {

class Application;
class Config;

struct AppShellInitExtStruct {

   const String LaunchFile;
   Boolean quit;

   AppShellInitExtStruct (const String &TheLaunchFile) :
         LaunchFile (TheLaunchFile),
         quit (False) {;}
};

typedef Boolean *(*init_shell_extension) (
   Config &manifest,
   Application &app,
   AppShellInitExtStruct &init);

};

#endif // DMZ_APP_SHELL_EXT_DOT_H
