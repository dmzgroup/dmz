#ifndef DMZ_RUNTIME_DATA_CONVERTER_COMMON_DOT_H
#define DMZ_RUNTIME_DATA_CONVERTER_COMMON_DOT_H

#include "dmzRuntimeContext.h"
#include <dmzRuntimeDefinitions.h>
#include <dmzTypesBase.h>

namespace dmz {

class String;

//! \cond
struct DataConverterCommonStruct {

   Handle handle;
   RuntimeContext *context;

   Handle set_handle (const String &HandleName, RuntimeContext *theContext) {

      Definitions defs (theContext);

      handle = defs.create_named_handle (HandleName);

      if (context) { context->unref (); }
      context = theContext;
      if (context) { context->ref (); }

      return handle;
   }

    DataConverterCommonStruct () : handle (0), context (0) {;}
    ~DataConverterCommonStruct () { if (context) { context->unref (); context = 0; } }
};
//! \endcond

};

#endif // DMZ_RUNTIME_DATA_CONVERTER_COMMON_DOT_H
