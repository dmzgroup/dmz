#ifndef DMZ_RUNTIME_DATA_CONVERTER_VECTOR_DOT_H
#define DMZ_RUNTIME_DATA_CONVERTER_VECTOR_DOT_H

#include <dmzRuntimeData.h>
#include <dmzKernelExport.h>
#include <dmzTypesVector.h>

namespace dmz {

   class PluginInfo;
   class RuntimeContext;

   class DMZ_KERNEL_LINK_SYMBOL DataConverterVector {

      public:
         DataConverterVector (const PluginInfo &Info);
         DataConverterVector (RuntimeContext *context);
         ~DataConverterVector ();

         Handle set_handle (const String &HandleName, RuntimeContext *context);

         Vector to_vector (const Data &Value);
         Vector to_vector (const Data *Value);
         Data to_data (const Vector &Value);
         Data to_data (const Vector *Value);
         void write_data (const Vector &Value, Data &data);

      protected:
         //! \cond
         struct State;
         State &_state;
         //! \endcond

      private:
         DataConverterVector ();
         DataConverterVector (const DataConverterVector &);
         DataConverterVector &operator= (const DataConverterVector &);
   };
};

#endif // DMZ_RUNTIME_DATA_CONVERTER_VECTOR_DOT_H
