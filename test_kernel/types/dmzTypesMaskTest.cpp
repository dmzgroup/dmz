#include <dmzTypesMask.h>
#include <dmzTypesBase.h>
#include <dmzTypesConsts.h>
#include <dmzTest.h>

#include <string.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   Test test ("dmzTypesMaskTest", argc, argv);

   // ============================================================================ //
   // <validate constructors>

   // default constructor
   Mask nullMask;
   test.validate (
      "Mask default constructor.",
      !nullMask.get_size () );

   Mask testMask;
   testMask.set_sub_mask (1, 0x01);
   // copy constructor
   Mask copyOfTestMask (testMask);
   test.validate (
      "Mask copy constructor.",
      (copyOfTestMask.get_size () == 2) && 
      (copyOfTestMask.get_sub_mask (0) == 0x00) &&
      (copyOfTestMask.get_sub_mask (1) == 0x01) );

   Mask shiftCtorTest (4);
   test.validate ("Mask shift constructor.", shiftCtorTest.get_sub_mask (0) == 0x010);

   Mask shiftCtorTest2 (30, 0x0F);
   test.validate (
      "Mask shift constructor with 32 bit mask.",
      (shiftCtorTest2.get_sub_mask (0) == 0xC0000000) &&
      (shiftCtorTest2.get_sub_mask (1) == 0x00000003));

   // </validate constructors>
   // ============================================================================ //
   // <bit manipulation functions>

   Mask bitFuncTestMask1;
   bitFuncTestMask1.set_sub_mask (0, 0);
   Mask bitFuncTestMask2;
   bitFuncTestMask2.set_sub_mask (0, 0x08);

   test.validate (
      "Mask contains () functions fails when it should.",
      !bitFuncTestMask1.contains (bitFuncTestMask2) );

   test.validate (
      "Mask test get_bit and set_bit and unset_bit.",
      !bitFuncTestMask1.get_bit (3) && 
      bitFuncTestMask1.set_bit (3) &&
      bitFuncTestMask1.set_bit (3) &&
      bitFuncTestMask1.get_bit (3));

   Mask unsetBitMask (3);
   test.validate ("Mask test unset_bit.", !unsetBitMask.unset_bit (3));

   test.validate (
      "Mask contains () functions succeeds when it should.",
      bitFuncTestMask1.contains (bitFuncTestMask2));

   test.validate (
      "Mask unset () function.",
      !bitFuncTestMask1.unset (bitFuncTestMask2) &&
      !bitFuncTestMask1.contains (bitFuncTestMask2) );

   // </bit manipulation functions>
   // ============================================================================ //
   // <validate operators>

   // assignment operator
   Mask operatorTestMask;
   operatorTestMask = testMask;
   test.validate (
      "Mask assignment operator.",
      (operatorTestMask.get_size () == 2) && 
      (operatorTestMask.get_sub_mask (0) == 0x00) &&
      (operatorTestMask.get_sub_mask (1) == 0x01) );

   // equality operator
   test.validate (
      "Mask equality operator.",
      (operatorTestMask == testMask) &&
      !(operatorTestMask == nullMask) );

   // inequality operator
   test.validate (
      "Mask inequality operator.",
      !(operatorTestMask != testMask) &&
      (operatorTestMask != nullMask) );

   // ! operator
   Mask zeroMask;
   zeroMask.set_sub_mask (0, 0x00);
   test.validate (
      "Mask ! operator.",
      !(!operatorTestMask) &&
      (!nullMask) &&
      (!zeroMask) );

   // ~ operator
   Mask invertedMask = ~testMask;
   test.validate (
      "Mask ~ operator.",
      (testMask.get_sub_mask (1) == 0x01) &&
      (invertedMask.get_sub_mask (1) == 0xFFFFFFFE) );

   Mask fullMask;
   fullMask.set_sub_mask (0, 0xFFFFFFFF);
   fullMask.set_sub_mask (1, 0xFFFFFFFF);
   test.validate (
      "Mask ^ operator.",
      ((testMask ^ invertedMask) == fullMask) &&
      ((testMask ^ zeroMask) == testMask) &&
      ((zeroMask ^ zeroMask) == zeroMask) &&
      ((zeroMask ^ fullMask) == fullMask) &&
      ((fullMask ^ fullMask) == zeroMask));

   test.validate (
      "Mask & operator.",
      ((testMask & invertedMask) == zeroMask) &&
      ((testMask & fullMask) == testMask) &&
      ((zeroMask & zeroMask) == zeroMask) &&
      ((zeroMask & fullMask) == zeroMask) &&
      ((fullMask & fullMask) == fullMask));

   test.validate (
      "Mask | operator.",
      ((testMask | invertedMask) == fullMask) &&
      ((testMask | fullMask) == fullMask) &&
      ((zeroMask | zeroMask) == zeroMask) &&
      ((zeroMask | fullMask) == fullMask) &&
      ((fullMask | fullMask) == fullMask));

   testMask ^= invertedMask;
   test.validate (
      "Mask ^= operator.",
      (testMask == fullMask) );

   testMask &= invertedMask;
   test.validate (
      "Mask &= operator.",
      (testMask == invertedMask) );

   Mask testEShiftMask;
   testEShiftMask.set_sub_mask (0, 0x04);
   testMask = zeroMask;
   testMask |= testEShiftMask;
   test.validate (
      "Mask |= operator.",
      (testMask == testEShiftMask) );

   Mask testMask2 (0, 0xF0000000);
   testMask2 << 2;
   Mask testMask3;
   testMask3.set_sub_mask (1, 0x00000003);
   testMask3.set_sub_mask (0, 0xC0000000);

   test.validate ("Mask << operator", testMask2 == testMask3);

   Mask testMask4;
   testMask4.set_sub_mask (0, 0x0F);
   testMask4.set_sub_mask (3, 0x0F);
   testMask4 >> 2;
   Mask testMask5;
   testMask5.set_sub_mask (0, 0x00000003);
   testMask5.set_sub_mask (2, 0xC0000000);
   testMask5.set_sub_mask (3, 0x00000003);

   test.validate ("Mask >> operator", testMask4 == testMask5);

   // </validate operators>
   // ============================================================================ //
   // <validate get_size and grow>

   test.validate (
      "Mask get_size () funcion.",
      (testMask.get_size () == 2) );

   test.validate (
      "Mask grow () funcion.",
      testMask.grow (4) &&
      (testMask.get_size () == 4) );

   // </validate get_size and grow>
   // ============================================================================ //
   // <empty and clear>

   testMask = fullMask;
   testMask.clear ();
   test.validate (
      "Mask clear () funcion.",
      testMask != fullMask &&
      testMask == zeroMask );

   testMask.empty ();
   test.validate (
      "Mask empty () funcion.",
      !testMask.get_size () &&
      !testMask.get_sub_mask (0) );

   // </empty and clear>
   // ============================================================================ //

   Mask testMask6 (12);
   Mask testMask7 (testMask6);

   test.validate (
      "Identical masks contain each other.",
      testMask6.contains (testMask7));

   return test.result ();
}

