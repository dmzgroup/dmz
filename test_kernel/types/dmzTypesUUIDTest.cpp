#include <dmzTypesUUID.h>
#include <dmzTypesBase.h>
#include <dmzTypesConsts.h>
#include <dmzTest.h>

#include <string.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   Test test ("dmzTypesUUIDTest", argc, argv);

   const UInt8 Array0[16] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

   const UInt8 Array1[16] = {
      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
      0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

   const UInt8 Array2[16] = {
      0x0F, 0x1E, 0x2D, 0x3C, 0x4B, 0x5A, 0x69, 0x78,
      0x87, 0x96, 0xA5, 0xB4, 0xC3, 0xD2, 0xE1, 0xF0 };

   const String Str0 ("00000000-0000-0000-0000-000000000000");
   const String Str1 ("00010203-0405-0607-0809-0a0b0c0d0e0f");
   const String Str2 ("0f1e2d3c-4b5a-6978-8796-a5b4c3d2e1f0");
   const String StrDash (
      "-0-f-1-e-2-d-3-c---4-b-5-a--6-9-7-8--8-7-9-6-a-5-b-4-c-3-d-2-e-1-f-0-");

   // The z at the end makes this invalid.
   const String StrInvalid ("0f1e2d3c-4b5a-6978-8796-a5b4c3d2e1fz");

   UUID uuid0;
   UUID uuidArray1 (Array1);
   UUID uuidArray2 (Array2);
   UUID uuidStr1 (Str1);
   UUID uuidStr2 (Str2);

   test.validate (
      Str0 + " is produced by uuid0.to_string",
      Str0 == uuid0.to_string ());

   test.validate ("!uuid0 == True", (!uuid0) == True);
   test.validate ("!uuidArray1 == False", (!uuidArray1) == False);
   test.validate ("!uuidArray2 == False", (!uuidArray2) == False);

   test.validate (
      Str1 + " is produced by uuidArray1.to_string",
      Str1 == uuidArray1.to_string ());

   test.validate (
      Str1 + " is produced by uuidStr1.to_string",
      Str1 == uuidStr1.to_string ());

   test.validate (
      Str2 + " is produced by uuidArray2.to_string",
      Str2 == uuidArray2.to_string ());

   test.validate (
      Str2 + " is produced by uuidStr2.to_string",
      Str2 == uuidStr2.to_string ());

   test.validate ("uuidStr1 == uuidArray1", uuidStr1 == uuidArray1);
   test.validate ("uuidStr2 == uuidArray2", uuidStr2 == uuidArray2);
   test.validate ("uuidStr1 != uuidArray2", uuidStr1 != uuidArray2);
   test.validate ("uuidStr2 != uuidArray1", uuidStr2 != uuidArray1);

   UUID uuid3;

   Boolean outOfRange (False);

   for (Int32 ix = 0; ix < 16; ix++) {

      if (!uuid3.set_element (ix, Array2[ix])) { outOfRange = True; }
   }

   test.validate ("set_element result is correct while index is in range", !outOfRange);
   outOfRange = !uuid3.set_element (16, 0xFF);
   test.validate ("set_element fails when index is over range", outOfRange);
   outOfRange = !uuid3.set_element (-1, 0xFF);
   test.validate ("set_element fails when index is under range", outOfRange);
   test.validate ("UUID initialized using set_element is valid", uuid3 == uuidArray2);

   UInt8 out[16] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

   for (Int32 jy = 0; jy < 16; jy++) {

      out[jy] = uuidArray1.get_element (jy);
   }

   test.validate (
      "Array created from get_element is valid",
      (out[0] == Array1[0]) &&
      (out[1] == Array1[1]) &&
      (out[2] == Array1[2]) &&
      (out[3] == Array1[3]) &&
      (out[4] == Array1[4]) &&
      (out[5] == Array1[5]) &&
      (out[6] == Array1[6]) &&
      (out[7] == Array1[7]) &&
      (out[8] == Array1[8]) &&
      (out[9] == Array1[9]) &&
      (out[10] == Array1[10]) &&
      (out[11] == Array1[11]) &&
      (out[12] == Array1[12]) &&
      (out[13] == Array1[13]) &&
      (out[14] == Array1[14]) &&
      (out[15] == Array1[15]));

   UInt8 out2[16] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

   uuidArray2.to_array (out2);

   test.validate (
      "Array created from to_array is valid",
      (out2[0] == Array2[0]) &&
      (out2[1] == Array2[1]) &&
      (out2[2] == Array2[2]) &&
      (out2[3] == Array2[3]) &&
      (out2[4] == Array2[4]) &&
      (out2[5] == Array2[5]) &&
      (out2[6] == Array2[6]) &&
      (out2[7] == Array2[7]) &&
      (out2[8] == Array2[8]) &&
      (out2[9] == Array2[9]) &&
      (out2[10] == Array2[10]) &&
      (out2[11] == Array2[11]) &&
      (out2[12] == Array2[12]) &&
      (out2[13] == Array2[13]) &&
      (out2[14] == Array2[14]) &&
      (out2[15] == Array2[15]));

   UUID uuidDash (StrDash);

   test.validate (
      "UUID created from string with too many dashes has correct value",
      uuidDash == uuidArray2);

   UUID uuidCopy (uuidArray2);

   test.validate ("UUID copy constructor", uuidCopy == uuidArray2);

   UUID uuid4;
   uuid4 = uuidArray2;
   test.validate ("UUID assignment operator", uuid4 == uuidArray2);

   UUID uuid5 (StrInvalid);
   test.validate ("Invalid string creates empty UUID", uuid0 == uuid5);

   UUID uuid6 (Str1);
   test.validate (
      "Invalid string passed into from_string returns False",
      !uuid6.from_string (StrInvalid));

   UUID uuid7 (Str1);
   test.validate (
      "Invalid string passed into from_string does not over write previous value",
      uuid6 == uuid7);

   return test.result ();
}
