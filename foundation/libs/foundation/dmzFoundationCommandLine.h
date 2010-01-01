#ifndef DMZ_FOUNDATION_COMMAND_LINE_DOT_H
#define DMZ_FOUNDATION_COMMAND_LINE_DOT_H

#include <dmzFoundationExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class DMZ_FOUNDATION_LINK_SYMBOL CommandLineArgs {

      public:
         CommandLineArgs ();
         CommandLineArgs (const CommandLineArgs &Args);
         CommandLineArgs (const String &Name);
         ~CommandLineArgs ();

         CommandLineArgs &operator= (const CommandLineArgs &);

         void set_name (const String &Name);
         String get_name () const;

         Int32 get_count () const;

         void append_arg (const String &Arg);
         Boolean get_first_arg (String &arg) const;
         Boolean get_next_arg (String &arg) const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };

   class DMZ_FOUNDATION_LINK_SYMBOL CommandLine {

      public:
         CommandLine ();
         CommandLine (int argc, char *argv[]);
         ~CommandLine ();

         Boolean parse (int argc, char *argv[]);

         void add_args (const CommandLineArgs &Args);

         Boolean get_first_option (CommandLineArgs &args) const;
         Boolean get_next_option (CommandLineArgs &args) const;

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         CommandLine (const CommandLine &);
         CommandLine &operator= (const CommandLine &);
   };
};

#endif // DMZ_FOUNDATION_COMMAND_LINE_DOT_H

