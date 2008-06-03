#ifndef DMZ_RUNTIME_INIT_DOT_H
#define DMZ_RUNTIME_INIT_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class Config;
   class Log;
   class RuntimeContext;

   DMZ_KERNEL_LINK_SYMBOL void runtime_init (
      const Config &Init,
      RuntimeContext *context,
      Log *log = 0);
};

#endif // DMZ_RUNTIME_INIT_DOT_H
