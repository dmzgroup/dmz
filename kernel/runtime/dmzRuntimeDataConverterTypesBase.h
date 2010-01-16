#ifndef DMZ_RUNTIME_DATA_CONVERTER_TYPES_BASE_DOT_H
#define DMZ_RUNTIME_DATA_CONVERTER_TYPES_BASE_DOT_H

#include <dmzRuntimeData.h>
#include <dmzKernelExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class PluginInfo;
   class RuntimeContext;

   class DMZ_KERNEL_LINK_SYMBOL DataConverterBoolean {

      public:
         DataConverterBoolean (const PluginInfo &Info);
         DataConverterBoolean (RuntimeContext *context);
         ~DataConverterBoolean ();

         Handle set_handle (const String &HandleName, RuntimeContext *context);

         Boolean to_boolean (const Data &Value, const Int32 Element = 0);
         Boolean to_boolean (const Data *Value, const Int32 Element = 0);
         Data to_data (const Boolean Value);
         void write_data (const Boolean Value, Data &data, const Int32 Element = 0);

      protected:
         //! \cond
         struct State;
         State &_state;
         //! \endcond

      private:
         DataConverterBoolean ();
         DataConverterBoolean (const DataConverterBoolean &);
         DataConverterBoolean &operator= (const DataConverterBoolean &);
   };

   class DMZ_KERNEL_LINK_SYMBOL DataConverterFloat64 {

      public:
         DataConverterFloat64 (const PluginInfo &Info);
         DataConverterFloat64 (RuntimeContext *context);
         ~DataConverterFloat64 ();

         Handle set_handle (const String &HandleName, RuntimeContext *context);

         Float64 to_float64 (const Data &Value, const Int32 Element = 0);
         Float64 to_float64 (const Data *Value, const Int32 Element = 0);
         Data to_data (const Float64 Value);
         void write_data (const Float64 Value, Data &data, const Int32 Element = 0);

      protected:
         //! \cond
         struct State;
         State &_state;
         //! \endcond

      private:
         DataConverterFloat64 ();
         DataConverterFloat64 (const DataConverterFloat64 &);
         DataConverterFloat64 &operator= (const DataConverterFloat64 &);
   };


   class DMZ_KERNEL_LINK_SYMBOL DataConverterString {

      public:
         DataConverterString (const PluginInfo &Info);
         DataConverterString (RuntimeContext *context);
         ~DataConverterString ();

         Handle set_handle (const String &HandleName, RuntimeContext *context);

         String to_string (const Data &Value, const Int32 Element = 0);
         String to_string (const Data *Value, const Int32 Element = 0);
         Data to_data (const String &Value);
         Data to_data (const String *Value);
         void write_data (const String &Value, Data &data, const Int32 Element = 0);

      protected:
         //! \cond
         struct State;
         State &_state;
         //! \endcond

      private:
         DataConverterString ();
         DataConverterString (const DataConverterString &);
         DataConverterString &operator= (const DataConverterString &);
   };

   class DMZ_KERNEL_LINK_SYMBOL DataConverterHandle {

      public:
         DataConverterHandle (const PluginInfo &Info);
         DataConverterHandle (RuntimeContext *context);
         ~DataConverterHandle ();

         Handle set_handle (const String &HandleName, RuntimeContext *context);

         Handle to_handle (const Data &Value, const Int32 Element = 0);
         Handle to_handle (const Data *Value, const Int32 Element = 0);
         Data to_data (const Handle &Value);
         void write_data (const Handle Value, Data &data, const Int32 Element = 0);

      protected:
         //! \cond
         struct State;
         State &_state;
         //! \endcond

      private:
         DataConverterHandle ();
         DataConverterHandle (const DataConverterHandle &);
         DataConverterHandle &operator= (const DataConverterHandle &);
   };
};

#endif // DMZ_RUNTIME_DATA_CONVERTER_TYPES_BASE_DOT_H
