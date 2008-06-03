#ifndef DMZ_RUNTIME_DATA_BINDER_DOT_H
#define DMZ_RUNTIME_DATA_BINDER_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class Config;
   class Data;
   class Log;
   class Mask;
   class Matrix;
   class RuntimeContext;
   class String;
   class Vector;
   class UUID;

   class DMZ_KERNEL_LINK_SYMBOL DataBinder {

      public:
         DataBinder (RuntimeContext *context, Log *log = 0);
         ~DataBinder ();
        
         Boolean read_data (const Data &InData);
         Boolean write_data (Data &outData) const;

         Boolean bind (const String &Name, const Int32 Element, Boolean &value);
         Boolean bind (const String &Name, const Int32 Element, UInt32 &value);
         Boolean bind (const String &Name, const Int32 Element, UInt64 &value);
         Boolean bind (const String &Name, const Int32 Element, Int32 &value);
         Boolean bind (const String &Name, const Int32 Element, Int64 &value);
         Boolean bind (const String &Name, const Int32 Element, Float32 &value);
         Boolean bind (const String &Name, const Int32 Element, Float64 &value);
         Boolean bind (const String &Name, const Int32 Element, String &value);
         Boolean bind (const String &Name, const Int32 Element, Vector &value);
         Boolean bind (const String &Name, const Int32 Element, Matrix &value);
         Boolean bind (const String &Name, const Int32 Element, UUID &value);
         Boolean bind (const String &Name, Mask &value);

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         DataBinder ();
         DataBinder (const DataBinder &);
         DataBinder &operator= (const DataBinder &);
   };
};

#endif // DMZ_RUNTIME_DATA_BINDER_DOT_H

