#ifndef DMZ_RUNTIME_DATA_CONVERTER_STRING_CONTAINER_DOT_H
#define DMZ_RUNTIME_DATA_CONVERTER_STRING_CONTAINER_DOT_H

#include <dmzRuntimeData.h>
#include <dmzKernelExport.h>
#include <dmzTypesStringContainer.h>

namespace dmz {

   class PluginInfo;
   class RuntimeContext;

   class DMZ_KERNEL_LINK_SYMBOL DataConverterStringContainer {

      public:
         DataConverterStringContainer (const PluginInfo &Info);
         DataConverterStringContainer (RuntimeContext *context);
         ~DataConverterStringContainer ();

         Handle set_handle (const String &HandleName, RuntimeContext *context);

         StringContainer to_string_container (const Data &Value);
         StringContainer to_string_container (const Data *Value);
         Data to_data (const StringContainer &Value);
         Data to_data (const StringContainer *Value);
         void write_data (const StringContainer &Value, Data &data);

      protected:
         //! \cond
         struct State;
         State &_state;
         //! \endcond

      private:
         DataConverterStringContainer ();
         DataConverterStringContainer (const DataConverterStringContainer &);
         DataConverterStringContainer &operator= (const DataConverterStringContainer &);
   };
};

#endif // DMZ_RUNTIME_DATA_CONVERTER_STRING_CONTAINER_DOT_H
