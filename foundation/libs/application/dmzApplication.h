#ifndef DMZ_APPLICATION_DOT_H
#define DMZ_APPLICATION_DOT_H

#include <dmzApplicationExport.h>
#include <dmzRuntimeLog.h>
#include <dmzTypesBase.h>

namespace dmz {

   class ApplicationState;
   class CommandLine;
   class Config;
   class RuntimeContext;

   class DMZ_APPLICATION_LINK_SYMBOL Application {

      public:
         Application (
            const String &Name,
            const String &Domain);

         ~Application ();

         Boolean is_running () const;
         void quit (const String &Reason);

         void set_quiet (const Boolean Value);

         void set_error (const String &ErrorMessage);
         Boolean is_error () const;
         String get_error () const;

         String get_name () const;
         String get_prefix () const;
         String get_domain () const;

         RuntimeContext *get_context () const;
         void get_global_config (Config &data) const;
         void add_global_config (Config &data);
         void add_config (const String &Name, Config &data);

         void get_session_config (Config &data) const;
         void set_session_config (Config &data);

         Boolean load_session ();
         Boolean process_command_line (const CommandLine &CL);
         Boolean load_plugins ();
         Boolean start ();
         Boolean update_time_slice ();
         Boolean stop ();
         Boolean unload_plugins ();
         Boolean save_session ();

      protected:
         struct State;
         State &_state; //!< Internal state.

      public:
         Log &log; //!< Reference to application log
         ApplicationState &state; //!< Application state.

      private:
         Application ();
         Application (const Application &);
         Application &operator= (const Application &);
   };
};

#endif // DMZ_APPLICATION_DOT_H
