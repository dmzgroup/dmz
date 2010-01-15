#ifndef DMZ_RUNTIME_DATA_CONVERTER_PATH_CONTAINER_DOT_H
#define DMZ_RUNTIME_DATA_CONVERTER_PATH_CONTAINER_DOT_H

#include <dmzRuntimeData.h>
#include <dmzKernelExport.h>
#include <dmzSystemFile.h>

namespace dmz {

   class PluginInfo;
   class RuntimeContext;

   class DMZ_KERNEL_LINK_SYMBOL DataConverterPathContainer {

      public:
         DataConverterPathContainer (const PluginInfo &Info);
         DataConverterPathContainer (RuntimeContext *context);
         ~DataConverterPathContainer ();

         Handle set_handle (const String &HandleName, RuntimeContext *context);

         PathContainer to_string_container (const Data &Value);
         PathContainer to_string_container (const Data *Value);
         Data to_data (const PathContainer &Value);
         Data to_data (const PathContainer *Value);

      protected:
         //! \cond
         struct State;
         State &_state;
         //! \endcond

      private:
         DataConverterPathContainer ();
         DataConverterPathContainer (const DataConverterPathContainer &);
         DataConverterPathContainer &operator= (const DataConverterPathContainer &);
   };
};

#endif // DMZ_RUNTIME_DATA_CONVERTER_PATH_CONTAINER_DOT_H
