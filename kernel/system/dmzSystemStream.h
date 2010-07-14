#ifndef DMZ_SYSTEM_STREAM_DOT_H
#define DMZ_SYSTEM_STREAM_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class HandleContainer;
   class Mask;
   class Matrix;
   class Stream;
   class String;
   class StringContainer;
   class UUID;
   class Vector;

   //! \brief Stream function typedef used by by
   //! \ingroup System
   //! dmz::Stream &dmz::Stream::operator<<(dmz::stream_operator_func func)
   typedef Stream &(*stream_operator_func) (Stream &);

   DMZ_KERNEL_LINK_SYMBOL Stream &flush (Stream &stream);
   DMZ_KERNEL_LINK_SYMBOL Stream &endl (Stream &stream);

   class Stream {

      public:
         //! Destructor.
         virtual ~Stream () {;}
         //! Write raw binary data the stream.
         virtual Stream &write_raw_data (const UInt8 *Data, const Int32 Size) = 0;
         //! Flush stream.
         virtual Stream &flush () = 0;
         //! Add newline to stream.
         virtual Stream &newline () = 0;
         //! Appends dmz::UInt16 value to stream.
         virtual Stream &operator<< (const UInt16 Value) = 0;
         //! Appends dmz::UInt32 value to stream.
         virtual Stream &operator<< (const UInt32 Value) = 0;
         //! Appends dmz::UInt64 value to stream.
         virtual Stream &operator<< (const UInt64 Value) = 0;
         //! Appends dmz::Int16 value to stream.
         virtual Stream &operator<< (const Int16 Value) = 0;
         //! Appends dmz::Int32 value to stream.
         virtual Stream &operator<< (const Int32 Value) = 0;
         //! Appends dmz::Int64 value to stream.
         virtual Stream &operator<< (const Int64 Value) = 0;
         //! Appends dmz::Float32 value to stream.
         virtual Stream &operator<< (const Float32 Value) = 0;
         //! Appends dmz::Float64 value to stream.
         virtual Stream &operator<< (const Float64 Value) = 0;
         //! Appends dmz::String value to stream.
         virtual Stream &operator<< (const String &Value) = 0;
         //! Appends 8 bit character value to stream.
         virtual Stream &operator<< (const char Value) = 0;
         //! Appends a NULL terminated character array to stream.
         virtual Stream &operator<< (const char *Data) = 0;
         //! Appends void pointer value to stream.
         virtual Stream &operator<< (const void *Value) = 0;
         //! Invokes stream_operator_func.
         virtual Stream &operator<< (stream_operator_func func) = 0;

      protected:
         //! Base constructor.
         Stream () {;}

      private:
         Stream (const Stream &);
         const Stream &operator= (const Stream &);
   };
};


DMZ_KERNEL_LINK_SYMBOL dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::Mask &Value);

DMZ_KERNEL_LINK_SYMBOL dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::Matrix &Value);

DMZ_KERNEL_LINK_SYMBOL dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::HandleContainer &Value);

DMZ_KERNEL_LINK_SYMBOL dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::StringContainer &Value);

DMZ_KERNEL_LINK_SYMBOL dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::UUID &Value);

DMZ_KERNEL_LINK_SYMBOL dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::Vector &Value);

#endif // DMZ_SYSTEM_STREAM_DOT_H
