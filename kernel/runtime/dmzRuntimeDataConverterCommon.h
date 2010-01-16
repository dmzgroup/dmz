#ifndef DMZ_RUNTIME_DATA_CONVERTER_COMMON_DOT_H
#define DMZ_RUNTIME_DATA_CONVERTER_COMMON_DOT_H

namespace dmz {

//! \cond
struct DataConverterCommonStruct {

   Handle handle;

   Handle set_handle (const String &HandleName, RuntimeContext *context) {

      Definitions defs (context);

      handle = defs.create_named_handle (HandleName);

      return handle;
   }

    DataConverterCommonStruct () : handle (0) {;}
};
//! \endcond

};

#endif // DMZ_RUNTIME_DATA_CONVERTER_COMMON_DOT_H
