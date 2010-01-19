#ifndef DMZ_RUNTIME_DATA_CONVERTER_UUID_DOT_H
#define DMZ_RUNTIME_DATA_CONVERTER_UUID_DOT_H

#include <dmzRuntimeData.h>
#include <dmzKernelExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>
#include <dmzTypesUUID.h>

namespace dmz {

class PluginInfo;
class RuntimeContext;

class DMZ_KERNEL_LINK_SYMBOL DataConverterUUID {

   public:
      DataConverterUUID (const PluginInfo &Info);
      DataConverterUUID (RuntimeContext *context);
      ~DataConverterUUID ();

      Handle set_handle (const String &HandleName, RuntimeContext *context);

      UUID to_uuid (const Data &Value, const Int32 Element = 0);
      UUID to_uuid (const Data *Value, const Int32 Element = 0);
      Data to_data (const UUID &Value);
      void write_data (const UUID &Value, Data &data, const Int32 Element = 0);

   protected:
      //! \cond
      struct State;
      State &_state;
      //! \endcond

   private:
      DataConverterUUID ();
      DataConverterUUID (const DataConverterUUID &);
      DataConverterUUID &operator= (const DataConverterUUID &);
};

};

#endif // DMZ_RUNTIME_DATA_CONVERTER_UUID_DOT_H
