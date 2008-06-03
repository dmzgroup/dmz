#ifndef DMZ_SYSTEM_STREAM_STRING_DOT_H
#define DMZ_SYSTEM_STREAM_STRING_DOT_H

#include <dmzKernelExport.h>
#include <dmzSystemStream.h>
#include <dmzTypesBooleanOperator.h>
#include <dmzTypesString.h>

namespace dmz {

   class DMZ_KERNEL_LINK_SYMBOL StreamString : public Stream {

      public:
         StreamString ();
         StreamString (String &value);
         virtual ~StreamString ();

         void set_string (String *value);
         String *get_string ();

         Boolean operator! () const;
         DMZ_BOOLEAN_OPERATOR;

         virtual Stream &write_raw_data (const UInt8 *Data, const Int32 Size);
         virtual Stream &flush ();
         virtual Stream &newline ();

         virtual Stream &operator<< (const UInt16 Value);
         virtual Stream &operator<< (const UInt32 Value);
         virtual Stream &operator<< (const UInt64 Value);

         virtual Stream &operator<< (const Int16 Value);
         virtual Stream &operator<< (const Int32 Value);
         virtual Stream &operator<< (const Int64 Value);

         virtual Stream &operator<< (const Float32 Value);
         virtual Stream &operator<< (const Float64 Value);

         virtual Stream &operator<< (const String &Value);
         virtual Stream &operator<< (const char Value);
         virtual Stream &operator<< (const char *Value);

         virtual Stream &operator<< (const void *Value);
         virtual Stream &operator<< (stream_operator_func func);

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         StreamString (const StreamString &);
         StreamString &operator= (const StreamString &);
   };
};

#endif // DMZ_SYSTEM_STREAM_STRING_DOT_H
