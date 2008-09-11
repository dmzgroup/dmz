#ifndef DMZ_APPLICATION_IPHONE_DOT_H
#define DMZ_APPLICATION_IPHONE_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzTypesBase.h>

namespace dmz {

   class ApplicationState;
   class Config;
   class PathContainer;
   class RuntimeContext;

   class ApplicationiPhone {

      public:
         ApplicationiPhone (const String &Name, const String &Domain);
         ~ApplicationiPhone ();

         void set_quiet (const Boolean Value);

         Boolean is_error () const;
         String get_error () const;

         RuntimeContext *get_context ();
         void get_global_config (Config &data);
         void add_global_config (Config &data);
         void add_config (const String &Name, Config &data);

         Boolean load_session ();
         Boolean load_config (const PathContainer &Files);
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
         ApplicationiPhone ();
         ApplicationiPhone (const ApplicationiPhone &);
         ApplicationiPhone &operator= (const ApplicationiPhone &);
   };
};

#endif // DMZ_APPLICATION_IPHONE_DOT_H
