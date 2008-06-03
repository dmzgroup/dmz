#include <dmzSystem.h>
#include <dmzSystemMarshal.h>
#include <dmzSystemUnmarshal.h>
#include <dmzTest.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   Test test ("dmzSystemMarshalTest", argc, argv);

   Marshal in (ByteOrderBigEndian);

   const Int8 V1 (1);
   const Int16 V2 (2);
   const Int32 V3 (3);
   const Int64 V4 (4);
   const UInt8 V5 (5);
   const UInt16 V6 (6);
   const UInt32 V7 (7);
   const UInt64 V8 (8);
   const Float32 V9 (9.0f);
   const Float64 V10 (10.0);
   const String V11 ("eleven");

   in.set_next_int8 (V1);
   in.set_next_int16 (V2);
   in.set_next_int32 (V3);
   in.set_next_int64 (V4);
   in.set_next_string (V11);
   in.set_next_uint8 (V5);
   in.set_next_uint16 (V6);
   in.set_next_uint32 (V7);
   in.set_next_uint64 (V8);
   in.set_next_fixed_string (V11, 3);
   in.set_next_float32 (V9);
   in.set_next_float64 (V10);

   Unmarshal out (ByteOrderBigEndian);
   out.set_buffer (in.get_length (), in.get_buffer ());

   test.validate (
     "Int data is marshalled correctly",
     (out.get_next_int8 () == V1) &&
     (out.get_next_int16 () == V2) &&
     (out.get_next_int32 () == V3) &&
     (out.get_next_int64 () == V4));

   String value;
   out.get_next_string (value);
   test.validate ("String data is marshalled correctly", V11 == value);

   test.validate (
     "Unsigned Int data is marshalled correctly",
     (out.get_next_uint8 () == V5) &&
     (out.get_next_uint16 () == V6) &&
     (out.get_next_uint32 () == V7) &&
     (out.get_next_uint64 () == V8));

   String valueFixed;
   out.get_next_fixed_string (3, valueFixed);
   test.validate ("Fixed String data is marshalled correctly", valueFixed == "ele");

   test.validate (
     "Float data is marshalled correctly",
     (out.get_next_float32 () == V9) &&
     (out.get_next_float64 () == V10));

   return test.result ();
}

