#ifndef DMZ_FOUNDATION_COMMAND_LINE_CONFIG_DOT_H
#define DMZ_FOUNDATION_COMMAND_LINE_CONFIG_DOT_H

#include <dmzFoundationExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class CommandLine;
   class Config;
   class Log;
   class StringContainer;

   class DMZ_FOUNDATION_LINK_SYMBOL CommandLineConfig {

      public:
         CommandLineConfig ();
         CommandLineConfig (const StringContainer &Container);
         ~CommandLineConfig ();

         void set_search_path (const StringContainer &Container);

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

#endif // DMZ_FOUNDATION_COMMAND_LINE_CONFIG_DOT_H
