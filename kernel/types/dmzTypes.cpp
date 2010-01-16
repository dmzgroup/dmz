#include <dmzTypesString.h>
#include <dmzTypesBase.h>
#include <dmzTypesConsts.h>

#include <math.h>
#include <stdlib.h>
#include <limits.h>

#include <dmzTypesArrays.h>

/*!

\namespace dmz
\brief DMZ Platform namespace.
\details All Platform classes and functions are contained in the dmz namespace.

\defgroup Types DMZ Kernel Types Layer

\file dmzTypesBase.h
\brief Contains Platform typedefs and type conversion functions.
\details Defines Integers and Floats for use in the platform.


\addtogroup Types
@{

\enum dmz::BaseTypeEnum
\brief Enum for each basic type defined by the platform.
\details Defined in dmzTypesBase.h.
Used to define the native type of an attribute. @see Data

*/

#if defined (__APPLE__) || defined (MACOSX)
// Note: This is a work around for an issue with gcc and debug information.
// It seems that when an inline class is used, the debug info is associated
// with the dylib that is loaded first and when it is unloaded the debug symbols are
// destroyed even if other dylibs are using the debug symbols.
namespace {

static const dmz::ArrayUInt8 LocalWorkArroundArrayUInt8;
static const dmz::ArrayUInt16 LocalWorkArroundArrayUInt16;
static const dmz::ArrayUInt32 LocalWorkArroundArrayUInt32;
static const dmz::ArrayUInt64 LocalWorkArroundArrayUInt64;
static const dmz::ArrayInt8 LocalWorkArroundArrayInt8;
static const dmz::ArrayInt16 LocalWorkArroundArrayInt16;
static const dmz::ArrayInt32 LocalWorkArroundArrayInt32;
static const dmz::ArrayInt64 LocalWorkArroundArrayInt64;
static const dmz::ArrayFloat32 LocalWorkArroundArrayFloat32;
static const dmz::ArrayFloat64 LocalWorkArroundArrayFloat64;
static const dmz::ArrayHandle LocalWorkArroundArrayHandle;

};
#endif

//! Minimum Int8 Value
const dmz::Int8 dmz::MinInt8 = SCHAR_MIN;
//! Maximum Int8 Value
const dmz::Int8 dmz::MaxInt8 = SCHAR_MAX;
//! Maximum UInt8 Value
const dmz::UInt8 dmz::MaxUInt8 = UCHAR_MAX;
//! Minimum Int16 Value
const dmz::Int16 dmz::MinInt16 = SHRT_MIN;
//! Maximum Int16 Value
const dmz::Int16 dmz::MaxInt16 = SHRT_MAX;
//! Maximum UInt16 Value
const dmz::UInt16 dmz::MaxUInt16 = USHRT_MAX;
//! Minimum Int32 Value
const dmz::Int32 dmz::MinInt32 = LONG_MIN;
//! Maximum Int32 Value
const dmz::Int32 dmz::MaxInt32 = LONG_MAX;
//! Maximum UInt32 Value
const dmz::UInt32 dmz::MaxUInt32 = ULONG_MAX;
//! Minimum Int64 Value
const dmz::Int64 dmz::MinInt64 = LLONG_MIN;
//! Maximum Int64 Value
const dmz::Int64 dmz::MaxInt64 = LLONG_MAX;
//! Maximum UInt64 Value
const dmz::UInt64 dmz::MaxUInt64 = ULLONG_MAX;

//! Constant false value.
const dmz::Boolean dmz::False = false;
//! Constant true value.
const dmz::Boolean dmz::True = true;

//! Auto grow mask used by hash tables.
const dmz::UInt32 dmz::AutoGrow = 0x01;

//! 32 bit value of Pi
const dmz::Float32 dmz::Pi32 = dmz::Float32 (3.14159265358979323846f);
//! 32 bit value of half of Pi
const dmz::Float32 dmz::HalfPi32 = dmz::Float32 (dmz::Pi32 / 2.0);
//! 32 bit value of Pi * 2
const dmz::Float32 dmz::TwoPi32 = dmz::Float32 (2.0f * dmz::Pi32);

//! 64 bit value of Pi
const dmz::Float64 dmz::Pi64 = dmz::Float64 (3.14159265358979323846);
//! 64 bit value of half of Pi
const dmz::Float64 dmz::HalfPi64 = dmz::Float64 (dmz::Pi64 / 2.0);
//! 64 bit value of Pi * 2
const dmz::Float64 dmz::TwoPi64 = dmz::Float64 (2.0 * dmz::Pi64);

const dmz::Float32 dmz::Epsilon32 = 0.0001f; //!< 32 bit value of Epsilon
const dmz::Float64 dmz::Epsilon64 = 0.000000001; //!< 64 bit value of Epsilon

//! 32 bit value of Earth gravity.
const dmz::Float32 dmz::EarthGravity32 = 9.80665f;
//! 64 bit value of Earth gravity.
const dmz::Float64 dmz::EarthGravity64 = 9.80665;

/*!

\brief Converts a string value into a dmz::BaseTypeEnum.
\details Defined in dmzTypesBase.h.
The function is \b not case sensitive.
Valid string values are:
- "boolean" is converted to dmz::BaseTypeBoolean.
- "int8" is converted to dmz::BaseTypeInt8.
- "int16" is converted to dmz::BaseTypeInt16.
- "int32" is converted to dmz::BaseTypeInt32.
- "int64" is converted to dmz::BaseTypeInt64.
- "uint8" is converted to dmz::BaseTypeUInt8.
- "uint16" is converted to dmz::BaseTypeUInt16.
- "uint32" is converted to dmz::BaseTypeUInt32.
- "uint64" is converted to dmz::BaseTypeUInt64.
- "float32" is converted to dmz::BaseTypeFloat32.
- "float64" is converted to dmz::BaseTypeFloat64.
- "string" is converted to dmz::BaseTypeString.
\param[in] Value contains the string to be converted into a dmz::BaseTypeEnum
\return Returns dmz::UnknownType on failure.

*/
dmz::BaseTypeEnum
dmz::string_to_base_type_enum  (const String &Value) {

   const String LowerValue (Value.get_lower ());

   BaseTypeEnum result (BaseTypeUnknown);

   if (LowerValue == "boolean") { result = BaseTypeBoolean; }
   else if (LowerValue == "int8") { result = BaseTypeInt8; }
   else if (LowerValue == "int16") { result = BaseTypeInt16; }
   else if (LowerValue == "int32") { result = BaseTypeInt32; }
   else if (LowerValue == "int64") { result = BaseTypeInt64; }
   else if (LowerValue == "uint8") { result = BaseTypeUInt8; }
   else if (LowerValue == "uint16") { result = BaseTypeUInt16; }
   else if (LowerValue == "uint32") { result = BaseTypeUInt32; }
   else if (LowerValue == "uint64") { result = BaseTypeUInt64; }
   else if (LowerValue == "float32") { result = BaseTypeFloat32; }
   else if (LowerValue == "float64") { result = BaseTypeFloat64; }
   else if (LowerValue == "string") { result = BaseTypeString; }

   return result;
}


/*!

\brief Converts a dmz::BaseTypeEnum into a string value.
\details Defined in dmzTypesBase.h.
String result are:
- dmz::BaseTypeBoolean is converted to "Boolean".
- dmz::BaseTypeInt8 is converted to "Int8".
- dmz::BaseTypeInt16 is converted to "Int16".
- dmz::BaseTypeInt32 is converted to "Int32".
- dmz::BaseTypeInt64 is converted to "Int64".
- dmz::BaseTypeUInt8 is converted to "UInt8".
- dmz::BaseTypeUInt16 is converted to "UInt16".
- dmz::BaseTypeUInt32 is converted to "UInt32".
- dmz::BaseTypeUInt64 is converted to "UInt64".
- dmz::BaseTypeFloat32 is converted to "Float32".
- dmz::BaseTypeFloat64 is converted to "Float64".
- dmz::BaseTypeString is converted to "String".
\param[in] Value contains the dmz::BaseTypeEnum to be converted into a string.
\return Returns the string "Unknown" on failure.

*/
dmz::String
dmz::base_type_enum_to_string  (const BaseTypeEnum Value) {

   String result ("Unknown");

   if (Value == BaseTypeBoolean) { result = "Boolean"; }
   else if (Value == BaseTypeInt8) { result = "Int8"; }
   else if (Value == BaseTypeInt16) { result = "Int16"; }
   else if (Value == BaseTypeInt32) { result = "Int32"; }
   else if (Value == BaseTypeInt64) { result = "Int64"; }
   else if (Value == BaseTypeUInt8) { result = "UInt8"; }
   else if (Value == BaseTypeUInt16) { result = "UInt16"; }
   else if (Value == BaseTypeUInt32) { result = "UInt32"; }
   else if (Value == BaseTypeUInt64) { result = "UInt64"; }
   else if (Value == BaseTypeFloat32) { result = "Float32"; }
   else if (Value == BaseTypeFloat64) { result = "Float64"; }
   else if (Value == BaseTypeString) { result = "String"; }
//   else if (Value == UnknownType) { result = "Unknown"; }

   return result;
}


/*!

\brief Converts string into a dmz::Boolean value.
\details Defined in dmzTypesBase.h. This function is \b not case sensitive.
The following strings are converted to dmz::False:
- "false"
- "0"
- "no"
- "off"
- the empty string (i.e. Where \a !Value returns dmz::True)
\b All other values return dmz::True.

\param[in] Value contains the string to be converted into a dmz::Boolean.
\return Returns dmz::True if \a Value is not one of the above defined strings.

*/
dmz::Boolean
dmz::string_to_boolean (const String &Value) {

   Boolean result (True);

   if (!Value) { result = False; }
   else {

      const String Lower (Value.get_lower ());

      if ((Lower == "false") || (Lower == "0") || (Lower == "no") || (Lower == "off")) {

         result = False;
      }
   }

   return result;
}


/*!

\brief Converts string into a dmz::Int32 value.
\details Defined in dmzTypesBase.h. If \a Value contains an integer string that is
larger than may be stored in a dmz::Int32 data will be lost. Uses dmz::string_to_int64 ()
and casts the result back to a dmz::Int32.
\param[in] Value contains the string to be converted into a dmz::Int32.

*/
dmz::Int32
dmz::string_to_int32 (const String &Value) {

   return Int32 (string_to_int64 (Value));;
}


/*!

\brief Converts string into a dmz::UInt32 value.
\details Defined in dmzTypesBase.h. If \a Value contains an integer string that is
larger than may be stored in a dmz::uInt32 data will be lost.
Uses dmz::string_to_uint64 () and casts the result back to a dmz::UInt32.
\param[in] Value contains the string to be converted into a dmz::UInt32.

*/
dmz::UInt32
dmz::string_to_uint32 (const String &Value) {

   return UInt32 (string_to_uint64 (Value));;
}


/*!

\brief Converts string into a dmz::Int64 value.
\details Defined in dmzTypesBase.h.
Uses dmz::string_to_uint64 () and casts the result back to a dmz::Int64.
If the string has a leading "-", the value is negated before return. Leading
white space (i.e. space and tab characters) is ignored.
\param[in] Value contains the string to be converted into a dmz::Int64.

*/
dmz::Int64
dmz::string_to_int64 (const String &Value) {

   Int64 result (0);
   Boolean negative (False);
   const char *Buffer = Value.get_buffer ();

   if (Buffer) {

      while ((*Buffer == ' ') || (*Buffer == '\t')) { Buffer++; }
      if (*Buffer == '-') { negative = True; }

      result = Int64 (string_to_uint64 (Value));
   }

   return negative ? -result : result;
}


/*!

\brief Converts string into a dmz::UInt64 value.
\details Defined in dmzTypesBase.h. This function is \b not case sensitive.
If the string has a leading "-", it is ignored. Leading
white space (i.e. space and tab characters) is ignored. This function will
convert string that represent integers in Base 8, 10, and 16. If a number string starts
with "0", the number is parsed as a base 8 number (a.k.a. Octal). If a number
string start with "0x" or "0X", it is parsed as a base 16 number (a.k.a. Hexadecimal).
Otherwise the number sting is parsed as a base 10 number. The first non-number
character encountered in the number string stops parsing. Commas (",") are \b not
treated as special characters.
- The decimal string " 12345fabc" returns the integer 12345
- The decimal string "42  541" returns the integer 42
- The decimal string "1,544" returns the integer 1
- The hexadecimal string "0x13FA" returns the integer 5114
- The octal string "01247" returns the integer 679

\param[in] Value contains the string to be converted into a dmz::UInt64.

*/
dmz::UInt64
dmz::string_to_uint64 (const String &Value) {

   UInt64 result (0);

   Int32 len (-1);
   Int32 count (0);
   const char *Buffer = Value.get_buffer (len);

   if (Buffer) {

      while ((*Buffer == ' ') || (*Buffer == '\t')) { Buffer++; count++;  }
      if (*Buffer == '-') { Buffer++; count++; }
      while ((*Buffer == ' ') || (*Buffer == '\t')) { Buffer++; count++;  }

      if (*Buffer == '0') { // hex or octal

         Buffer++; count++;

         if ((*Buffer == 'x') || (*Buffer == 'X')) { // hex

            Buffer++; count++;
            Boolean done (False);

            while (!done) {

               if ((*Buffer >= '0') && (*Buffer <= '9')) {

                  result = result << 8;
                  result |= UInt64 (*Buffer - '0');
               }
               else if ((*Buffer >= 'a') && (*Buffer <= 'f')) {

                  result = result << 8;
                  result |= UInt64 ((*Buffer - 'a') + 10);
               }
               else if ((*Buffer >= 'A') && (*Buffer <= 'F')) {

                  result = result << 8;
                  result |= UInt64 ((*Buffer - 'a') + 10);
               }
               else { done = True; }

               Buffer++; count++;
               if (count >= len) { done = True; }
            }
         }
         else { // octal

            while ((*Buffer >= '0') && (*Buffer <= '7') && (count < len)) {

               result = result << 4;
               result |= UInt64 (*Buffer - '0');
               Buffer++; count++;
            }
         }
      }
      else { // decimal

         while ((*Buffer >= '0') && (*Buffer  <= '9') && (count < len)) {

            result *= 10;
            result += UInt64 ((*Buffer) - '0');
            Buffer++;
            count++;
         }
      }
   }

   return result;
}


/*!

\brief Converts string to dmz::Float32
\details Defined in dmzTypesBase.h. Use the Standard C function atof and casts the result
to a dmz::Float32. If the string contains a value large than may be stored in a
32 bit floating point number, data may be lost.
\param[in] Value contains the string to be converted into a dmz::Float32.

*/
dmz::Float32
dmz::string_to_float32 (const String &Value) {

   return Float32 (atof (Value.get_buffer ()));
}


/*!

\brief Converts string to dmz::Float64
\details Defined in dmzTypesBase.h. Use the Standard C function atof.
\param[in] Value contains the string to be converted into a dmz::Float64.

*/
dmz::Float64
dmz::string_to_float64 (const String &Value) {

   return Float64 (atof (Value.get_buffer ()));
}


/*!

\brief Tests if 32 bit \a Value is near zero.
\details Defined in dmzTypesBase.h.
\param[in] Value contains the dmz::Float32 to be tested.
\return Returns dmz::True if: fabs (Value) \< dmz::Epsilon32.

*/
dmz::Boolean
dmz::is_zero32 (const Float32 Value) {

   return fabs (Value) < Epsilon32;
}


/*!

\brief Tests if 32 bit \a Value is near zero.
\details Defined in dmzTypesBase.h.
\param[in] Value contains the dmz::Float32 to be tested.
\param[in] Epsilon contains the dmz::Float32 to use for the min and max range.
\return Returns dmz::True if: fabs (Value) \< \a Epsilon.

*/
dmz::Boolean
dmz::is_zero32 (const Float32 Value, const Float32 Epsilon) {

   return fabs (Value) < fabs (Epsilon);
}


/*!

\brief Tests if 64 bit \a Value is near zero.
\details Defined in dmzTypesBase.h.
\param[in] Value contains the dmz::Float64 to be tested.
\return Returns dmz::True if: fabs (Value) \< dmz::Epsilon64.

*/
dmz::Boolean
dmz::is_zero64 (const Float64 Value) {

   return fabs (Value) < fabs (Epsilon64);
}


/*!

\brief Tests if 64 bit \a Value is near zero.
\details Defined in dmzTypesBase.h.
\param[in] Value contains the dmz::Float64 to be tested.
\param[in] Epsilon contains the dmz::Float64 to use for the min and max range.
\return Returns dmz::True if: fabs (Value) \< \a Epsilon.

*/
dmz::Boolean
dmz::is_zero64 (const Float64 Value, const Float64 Epsilon) {

   return fabs (Value) < fabs (Epsilon);
}


/*!

\brief Converts 64 bit \a Value from radians to degrees.
\details Defined in dmzTypesBase.h.
\param[in] Value contains the dmz::Float64 to be converted to degrees.

*/
dmz::Float64
dmz::to_degrees (const Float64 Value) { return (Value / Pi64) * 180.0; }


/*!

\brief Converts 64 bit \a Value from degrees to radians.
\details Defined in dmzTypesBase.h.
\param[in] Value contains the dmz::Float64 to be converted to radians.

*/
dmz::Float64
dmz::to_radians (const Float64 Value) { return (Value / 180.0) * Pi64; }

//! @}
