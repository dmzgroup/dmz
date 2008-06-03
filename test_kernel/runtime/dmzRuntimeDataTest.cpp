#include <dmzRuntimeData.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeInit.h>
#include <dmzSystem.h>
#include <dmzTest.h>

#include <dmzTypesMask.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesUUID.h>
#include <dmzTypesVector.h>

using namespace dmz;


int
main (int argc, char *argv[]) {

   Test test ("dmzRuntimeDataTest", argc, argv);
   RuntimeContext *context (test.rt.get_context ());

   Definitions defs (context, &(test.log));

   const Handle StringHandle (defs.create_named_handle ("string"));
   const Handle UInt32Handle (defs.create_named_handle ("uint32"));
   const Handle UInt64Handle (defs.create_named_handle ("uint64"));
   const Handle Int32Handle (defs.create_named_handle ("int32"));
   const Handle Int64Handle (defs.create_named_handle ("int64"));
   const Handle Float32Handle (defs.create_named_handle ("float32"));
   const Handle Float64Handle (defs.create_named_handle ("float64"));

   Data data (test.rt.get_context ());

   const String TestStringValue ("Test Value");
   const UInt32 TestUInt32Value (1001);
   const UInt64 TestUInt64Value (DMZ_UINT64_DATA (1000000000001));
   const Int32 TestInt32Value (-2002);
   const Int64 TestInt64Value (DMZ_INT64_DATA (-2000000000002));
   const Float32 TestFloat32Value (3003.3f);
   const Float64 TestFloat64Value (3000000000003.3);

   test.validate (
      "Storing string",
      data.store_string (StringHandle, 0, TestStringValue));

   test.validate (
      "Storing UInt32",
      data.store_uint32 (UInt32Handle, 0, TestUInt32Value));

   test.validate (
      "Storing UInt64",
      data.store_uint64 (UInt64Handle, 0, TestUInt64Value));

   test.validate (
      "Storing Int32",
      data.store_int32 (Int32Handle, 0, TestInt32Value));

   test.validate (
      "Storing Int64",
      data.store_int64 (Int64Handle, 0, TestInt64Value));

   test.validate (
      "Storing Float32",
      data.store_float32 (Float32Handle, 0, TestFloat32Value));

   test.validate (
      "Storing Float64",
      data.store_float64 (Float64Handle, 0, TestFloat64Value));

   String stringValue;
   UInt32 uint32Value (0);
   UInt64 uint64Value (0);
   Int32 int32Value (0);
   Int64 int64Value (0);
   Float32 float32Value (0.0f);
   Float64 float64Value (0.0);

   test.validate (
      "Looking up string",
      data.lookup_string (StringHandle, 0, stringValue));

   test.validate (
      "Looking up UInt32",
      data.lookup_uint32 (UInt32Handle, 0, uint32Value));

   test.validate (
      "Looking up UInt64",
      data.lookup_uint64 (UInt64Handle, 0, uint64Value));

   test.validate (
      "Looking up Int32",
      data.lookup_int32 (Int32Handle, 0, int32Value));

   test.validate (
      "Looking up Int64",
      data.lookup_int64 (Int64Handle, 0, int64Value));

   test.validate (
      "Looking up Float32",
      data.lookup_float32 (Float32Handle, 0, float32Value));

   test.validate (
      "Looking up Float64",
      data.lookup_float64 (Float64Handle, 0, float64Value));

   test.validate (
      "Returned string contains correct value",
      TestStringValue == stringValue);

   test.validate (
      "Returned UInt32 contains correct value",
      TestUInt32Value == uint32Value);

   test.validate (
      "Returned UInt64 contains correct value",
      TestUInt64Value == uint64Value);

   test.validate (
      "Returned Int32 contains correct value",
      TestInt32Value == int32Value);

   test.validate (
      "Returned Int64 contains correct value",
      TestInt64Value == int64Value);

   test.validate (
      "Returned Float32 contains correct value",
      TestFloat32Value == float32Value);

   test.validate (
      "Returned Float64 contains correct value",
      TestFloat64Value == float64Value);

   return test.result ();
}
