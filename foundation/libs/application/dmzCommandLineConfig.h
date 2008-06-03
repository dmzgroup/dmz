#ifndef DMZ_COMMAND_LINE_CONFIG_DOT_H
#define DMZ_COMMAND_LINE_CONFIG_DOT_H

#include <dmzApplicationExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class CommandLine;
   class Config;
   class Log;
   class PathContainer;

   class DMZ_APPLICATION_LINK_SYMBOL CommandLineConfig {

      public:
         CommandLineConfig ();
         CommandLineConfig (const PathContainer &Container);
         ~CommandLineConfig ();

         void set_search_path (const PathContainer &Container);

         Boolean process_command_line (
            const CommandLine &Opts,
            Config &globalData,
            Log *log = 0);

         String get_error ();

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         CommandLineConfig (const CommandLineConfig &);
         CommandLineConfig &operator= (const CommandLineConfig &);
   };
};

#endif // DMZ_COMMAND_LINE_CONFIG_DOT_H
