#ifndef DMZ_TYPES_STRING_DOT_H
#define DMZ_TYPES_STRING_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesBooleanOperator.h>

namespace dmz {

   class HandleContainer;
   class Mask;
   class Matrix;
   class StringContainer;
   class UUID;
   class Vector;

   enum NumberBaseEnum {
      Octal,       //!< Base 8
      Decimal,     //!< Base 10
      Hexadecimal  //!< Base 16
   };

   class DMZ_KERNEL_LINK_SYMBOL String {

      public:
         static String number (const char Value);
         static String number (const short Value);
         static String number (const int Value);
         static String number (const long Value);
         static String number (const Int64 Value);
         static String number (const unsigned char Value);
         static String number (const unsigned char Value, const NumberBaseEnum Base);
         static String number (const unsigned short Value);
         static String number (const unsigned short Value, const NumberBaseEnum Base);
         static String number (const unsigned int Value);
         static String number (const unsigned int Value, const NumberBaseEnum Base);
         static String number (const unsigned long Value);
         static String number (const unsigned long Value, const NumberBaseEnum Base);
         static String number (const UInt64 Value);
         static String number (const UInt64 Value, const NumberBaseEnum Base);
         static String number (const Float32 Value);
         static String number (const Float32 Value, const Int32 Precision);

         static String number (
            const Float32 Value,
            const Int32 Precision,
            const char Mode);

         static String number (const Float64 Value);
         static String number (const Float64 Value, const Int32 Precision);

         static String number (
            const Float64 Value,
            const Int32 Precision,
            const char Mode);

         String ();
         String (const String &Str);
         String (const char *Buffer, const Int32 Length, const Int32 Size);
         String (const char *Buffer, const Int32 Length);
         String (const char *Buffer);
         ~String ();

         Boolean operator== (const String &Buffer) const;
         Boolean operator!= (const String &Buffer) const;
         String &operator= (const String &Buffer);

         String operator+ (const String &Buffer) const;
         String &operator+= (const String &Buffer);
         String &operator+= (const char Value);
         Boolean operator! () const;
         DMZ_BOOLEAN_OPERATOR;

         void set_length (const Int32 Length, String *remainder = 0);
         Int32 get_length () const;
         void set_size (const Int32 Size);
         Int32 get_size () const;
         void set_buffer (const char *Buffer, const Int32 Length, const Int32 Size);

         void set_buffer (const char *Buffer, const Int32 Length) {

            set_buffer (Buffer, Length, Length);
         }

         const char *get_buffer (Int32 &length) const;
         const char *get_buffer () const;
         char get_char (const Int32 Index) const;
         Boolean set_char (const Int32 Index, const char Value);

         String &empty ();
         String &flush ();
         String &resize (const Int32 Size);
         String &append (const String &Value);
         String &repeat (const String &Buffer, const Int32 Count);
         String &shift (const Int32 ShiftValue, const char FillChar = ' ');

         Boolean is_null () const;

         String &to_upper ();
         String &to_lower ();
         String get_upper () const;
         String get_lower () const;

         Boolean find_sub (
            const String &Sub,
            Int32 &index,
            const Int32 Start = 0) const;

         String get_sub (const Int32 Start, const Int32 End = -1) const;

         Int32 replace_sub (
            const String &Sub,
            const String &Target,
            const Int32 Start = 0);

         Boolean contains_sub (const String &Sub, const Int32 Start = 0) const;

      protected:
         char * _buffer; //!< Character buffer.
         Int32 _length; //!< Number of bytes used in buffer.
         Int32 _size; //!< Buffer size in bytes.
   };
};

//! Concatenates dmz::String to end of buffer.
DMZ_KERNEL_LINK_SYMBOL dmz::String &
operator<< (dmz::String &str, const dmz::String &Value);

//! Concatenates C string to end of buffer.
DMZ_KERNEL_LINK_SYMBOL dmz::String &
operator<< (dmz::String &str, const char *Value);

//! Converts unsigned short to string and concatenates it to end of buffer.
DMZ_KERNEL_LINK_SYMBOL dmz::String &
operator<< (dmz::String &str, const unsigned short Value);

//! Converts unsigned int to string and concatenates it to end of buffer.
DMZ_KERNEL_LINK_SYMBOL dmz::String &
operator<< (dmz::String &str, const unsigned int Value);

//! Converts unsigned long to string and concatenates it to end of buffer.
DMZ_KERNEL_LINK_SYMBOL dmz::String &
operator<< (dmz::String &str, const unsigned long Value);

//! Converts unsigned 64 bit integer  to string and concatenates it to end of buffer.
DMZ_KERNEL_LINK_SYMBOL dmz::String &
operator<< (dmz::String &str, const dmz::UInt64 Value);

//! Converts short to string and concatenates it to end of buffer.
DMZ_KERNEL_LINK_SYMBOL dmz::String &
operator<< (dmz::String &str, const short Value);

//! Converts int to string and concatenates it to end of buffer.
DMZ_KERNEL_LINK_SYMBOL dmz::String &
operator<< (dmz::String &str, const int Value);

//! Converts long to string and concatenates it to end of buffer.
DMZ_KERNEL_LINK_SYMBOL dmz::String &
operator<< (dmz::String &str, const long Value);

//! Converts 64 bit integer  to string and concatenates it to end of buffer.
DMZ_KERNEL_LINK_SYMBOL dmz::String &
operator<< (dmz::String &str, const dmz::Int64 Value);

//! Converts 32 bit float to string and concatenates it to end of buffer.
DMZ_KERNEL_LINK_SYMBOL dmz::String &
operator<< (dmz::String &str, const dmz::Float32 Value);

//! Converts 64 but float to string and concatenates it to end of buffer.
DMZ_KERNEL_LINK_SYMBOL dmz::String &
operator<< (dmz::String &str, const dmz::Float64 Value);

//! Converts pointer to string and concatenates it to end of buffer.
DMZ_KERNEL_LINK_SYMBOL dmz::String &
operator<< (dmz::String &str, const void *Value);

//! Converts dmz::Vector to string and concatenates it to end of buffer.
DMZ_KERNEL_LINK_SYMBOL dmz::String &
operator<< (dmz::String &str, const dmz::Vector &Value);

//! Converts dmz::Mask to string and concatenates it to end of buffer.
DMZ_KERNEL_LINK_SYMBOL dmz::String &
operator<< (dmz::String &str, const dmz::Mask &Value);

//! Converts dmz::Matrix to string and concatenates it to end of buffer.
DMZ_KERNEL_LINK_SYMBOL dmz::String &
operator<< (dmz::String &str, const dmz::Matrix &Value);

//! Converts dmz::HandleContainer to string and concatenates it to end of buffer.
DMZ_KERNEL_LINK_SYMBOL dmz::String &
operator<< (dmz::String &str, const dmz::HandleContainer &Value);

//! Converts dmz::StringContainer to string and concatenates it to end of buffer.
DMZ_KERNEL_LINK_SYMBOL dmz::String &
operator<< (dmz::String &str, const dmz::StringContainer &Value);

//! Converts dmz::UUID to string and concatenates it to end of buffer.
DMZ_KERNEL_LINK_SYMBOL dmz::String &
operator<< (dmz::String &str, const dmz::UUID &Value);

#endif // DMZ_TYPES_STRING_DOT_H
