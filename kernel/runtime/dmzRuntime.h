#ifndef DMZ_RUNTIME_DOT_H
#define DMZ_RUNTIME_DOT_H

#include <dmzKernelExport.h>

namespace dmz {

   class Config;
   class RuntimeContext;

   class DMZ_KERNEL_LINK_SYMBOL Runtime {

      public:
         Runtime ();
         ~Runtime ();

         RuntimeContext *get_context ();

         void set_session_config (const Config &Session);
         void get_session_config (Config &session);

         void sync ();

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         Runtime (const Runtime &);
         const Runtime &operator= (const Runtime &);
   };
};

#endif // DMZ_RUNTIME_DOT_H
