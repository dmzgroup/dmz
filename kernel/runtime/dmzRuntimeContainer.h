#ifndef DMZ_RUNTIME_CONTAINER_DOT_H
#define DMZ_RUNTIME_CONTAINER_DOT_H

#include <dmzKernelExport.h>

namespace dmz {

   class RuntimeContext;
   class PluginInfo;

   class DMZ_KERNEL_LINK_SYMBOL RuntimeContainer {

      public:
         RuntimeContainer (RuntimeContext *context);
         RuntimeContainer (const PluginInfo &Info);
         ~RuntimeContainer ();

         void clear ();

         void set_context (RuntimeContext *context);
         RuntimeContext *get_context () const;

      protected:
         //! \cond
         struct State;
         State &_state;
         //! \endcond

      private:
         RuntimeContainer ();
         RuntimeContainer (const RuntimeContainer &);
         const RuntimeContainer &operator= (const RuntimeContainer &);
   };
};

#endif // DMZ_RUNTIME_CONTAINER_DOT_H
