#include <dmzTypesHashTableUInt32Template.h>
#include <dmzTypesString.h>
#include <dmzTypesBase.h>
#include <dmzTypesConsts.h>
#include <dmzTest.h>

#include <string.h>

using namespace dmz;

// test class to make sure the empty () call works
class DestructionTestClass
{
   public:
      ~DestructionTestClass() { destructionCount++; }

      static Int16 destructionCount;
};
Int16 DestructionTestClass::destructionCount=0;

int
main (int argc, char *argv[]) {

   // <CODER --> JADE>
   // ---------------{

   Test test ("dmzTypesHashTableUInt32TemplateTest", argc, argv);

   // ============================================================================ //
   // data
   String dataStringAGreatDeal( "A Great Deal" );
   String dataStringAlbertBurton( "AlbertBurton" );
   String dataStringAnd( "And" );
   String dataStringBeans( "Beans" );
   String dataStringCheese( "Cheese" );
   String dataStringFish( "Fish" );
   String dataStringHeart( "Heart" );
   String dataStringI( "I" );
   String dataStringLondon( "London" );
   String dataStringLove( "Love" );
   String dataStringMeat( "Meat" );
   String dataStringNewJersey( "New Jersey" );
   String dataStringWithRice( "WithRice" );

   // ============================================================================ //
   // <validate constructors>

   // default constructor
   HashTableUInt32Template<String> nullHashTable;
   test.validate (
      "HashTableUInt32Template default constructor.",
      !nullHashTable.get_size () && !nullHashTable.get_count () );

   // standard constructor
   HashTableUInt32Template<String> testHashTable (4, 0, 0);
   test.validate (
      "HashTableUInt32Template standard constructor.",
      (testHashTable.get_size () == 4) && !testHashTable.get_count () &&
      !testHashTable.get_lock () );

   // copy constructor

   HashTableUInt32Template<String> copyOfNullHashTable (nullHashTable);
   HashTableUInt32Template<String> copyOfTestHashTable (testHashTable);
   test.validate (
      "HashTableUInt32Template copy constructor.",
      !copyOfNullHashTable.get_size () && !copyOfNullHashTable.get_count () &&
      (copyOfTestHashTable.get_size () == 4) && !copyOfTestHashTable.get_count () &&
      !copyOfTestHashTable.get_lock () );

   // --- lock constructor ---

   // ?

   // </validate constructors>
   // ============================================================================ //
   // <basic data access>

   // check for overflow catching
   HashTableUInt32Template<String> fixedSizeHashTable (4, 0, 0);
   fixedSizeHashTable.store (1248, &dataStringFish );
   fixedSizeHashTable.store (3425, &dataStringLove );
   fixedSizeHashTable.store (6743, &dataStringMeat );
   fixedSizeHashTable.store (3246, &dataStringAnd );
   test.validate (
      "HashTableUInt32Template test fixed size hashTable overflow.",
      !fixedSizeHashTable.store (5344, &dataStringCheese ) ); // try to overload

   // check autogrow
   HashTableUInt32Template<String> autoGrowHashTable (2, dmz::AutoGrow, 0);
   autoGrowHashTable.store (1248, &dataStringFish );
   autoGrowHashTable.store (3425, &dataStringLove );
   test.validate (
      "HashTableUInt32Template test autogrow feature.",
      (autoGrowHashTable.get_size () == 2) && // check initial size
      autoGrowHashTable.store (6743, &dataStringMeat ) && // stick something new in
      (autoGrowHashTable.get_size () == 4) ); // did it double size?

   // validate lookup NULL return
   test.validate (
      "HashTableUInt32Template test lookup failure on an invalid key.",
      !autoGrowHashTable.lookup (1249) );

   // validate lookup
   test.validate (
      "HashTableUInt32Template test lookup on a stored value.",
      autoGrowHashTable.lookup (1248) &&
      (autoGrowHashTable.lookup (1248) == &dataStringFish) );

   // validate remove
   test.validate (
      "HashTableUInt32Template test remove function and verify with lookup.",
      autoGrowHashTable.remove (1248) &&
      !autoGrowHashTable.lookup (1248) );

   // what if we try storing a null string
   // autoGrowHashTable should have count 2 at the moment ... { Love, Meat }
   test.validate (
      "HashTableUInt32Template make sure we can't add a NULL pointer.",
      (autoGrowHashTable.get_count () == 2) &&
      !autoGrowHashTable.store (6666, 0) );

   // </basic data access>
   // ============================================================================ //
   // <validate get_size & get_count functions>
   test.validate (
      "HashTableUInt32Template test getSize&getCount on fixed-size hash table -- A) Verify initial length.",
      (fixedSizeHashTable.get_size () == 4) && (fixedSizeHashTable.get_count () == 4) );
   test.validate (
      "B) Verify removing an item decreases count.",
      fixedSizeHashTable.remove (6743) && // remove an item
      (fixedSizeHashTable.get_size () == 4) && (fixedSizeHashTable.get_count () == 3) );
   test.validate (
      "C) Verify adding a NULL pointer doesn't increase count.",
      !fixedSizeHashTable.store (6666, 0) && // attempt to overload
      (fixedSizeHashTable.get_size () == 4) && (fixedSizeHashTable.get_count () == 3) );
   test.validate (
      "D) Verify adding and item increases count.",
      // stick something new in
      fixedSizeHashTable.store (1111, &dataStringAlbertBurton) &&
      (fixedSizeHashTable.get_size () == 4) && (fixedSizeHashTable.get_count () == 4) );
   test.validate (
      "E) Verify overloading doesn't increase count.",
      !fixedSizeHashTable.store (9950, &dataStringLondon) && // attempt to overload
      (fixedSizeHashTable.get_size () == 4) && (fixedSizeHashTable.get_count () == 4) );
   test.validate (
      "F) Verify removing an invalid item doesn't effect count",
      !fixedSizeHashTable.remove (5531) &&
      (fixedSizeHashTable.get_size () == 4) && (fixedSizeHashTable.get_count () == 4) );
   test.validate (
      "G) Verify removing everything gives count of 0",
      fixedSizeHashTable.remove (1111) && fixedSizeHashTable.remove (1248) &&
      fixedSizeHashTable.remove (3425) && fixedSizeHashTable.remove (3246) &&
      (fixedSizeHashTable.get_size () == 4) && !fixedSizeHashTable.get_count () );

   test.validate (
      "HashTableUInt32Template test getSize&getCount on autoGrow hash table -- A) Verify initial length.",
      // start at 4-2
      autoGrowHashTable.get_size () == 4 && autoGrowHashTable.get_count () == 2 );
   test.validate (
      "B) Fill in the list and verify the size in unchanged and count is full.",
      autoGrowHashTable.store (1234, &dataStringAnd ) &&
      autoGrowHashTable.store (1235, &dataStringBeans ) &&
      autoGrowHashTable.get_size () == 4 && autoGrowHashTable.get_count () == 4 );
   test.validate (
      "C) Add an item to make the list grow, verify size and count.",
      autoGrowHashTable.store (9021, &dataStringWithRice ) && // autoGrow
      autoGrowHashTable.get_size () == 8 && autoGrowHashTable.get_count () == 5 );

   // </validate get_size & get_count functions>
   // ============================================================================ //
   // <validate clear & empty functions>
   // testHashTable should be empty
   testHashTable.store (1, &dataStringAlbertBurton );
   testHashTable.store (2, &dataStringAnd );
   testHashTable.store (3, &dataStringLondon );
   testHashTable.store (4, &dataStringCheese );
   String * dataPointer = testHashTable.lookup (3);
   testHashTable.clear();
   test.validate (
      "HashTableUInt32Template test clear () on fixedSize.",
      (testHashTable.get_size () == 4) && !testHashTable.get_count () ); // empty
   test.validate (
      "HashTableUInt32Template test that clear () didn't clear memory.",
      (dataPointer == &dataStringLondon ) ); // data is still there

   HashTableUInt32Template<DestructionTestClass> emptyTestHashTable (4, 0, 0);
   emptyTestHashTable.store (3521, new DestructionTestClass);
   emptyTestHashTable.store (8154, new DestructionTestClass);
   emptyTestHashTable.store (2575, new DestructionTestClass);
   emptyTestHashTable.store (4545, new DestructionTestClass);
   DestructionTestClass::destructionCount = 0;
   emptyTestHashTable.empty();
   test.validate (
      "HashTableUInt32Template test empty () on fixedSize hashTable.",
      (emptyTestHashTable.get_size () == 4) && !emptyTestHashTable.get_count ());
   test.validate (
      "HashTableUInt32Template test that prev empty () cleared it memory.",
      DestructionTestClass::destructionCount == 4 ); // data is still there

   HashTableUInt32Template<DestructionTestClass>
      emptyTestAutoGrowHashTable (1, AutoGrow, 0);
   emptyTestAutoGrowHashTable.store (4252, new DestructionTestClass);
   emptyTestAutoGrowHashTable.store (6856, new DestructionTestClass);
   DestructionTestClass::destructionCount = 0;
   emptyTestAutoGrowHashTable.empty();
   test.validate (
      "HashTableUInt32Template test empty () on autoGrow hashTable.",
      (emptyTestAutoGrowHashTable.get_size () == 2) &&
         !emptyTestAutoGrowHashTable.get_count () );
   test.validate (
      "HashTableUInt32Template test that prev empty () cleared it memory.",
      DestructionTestClass::destructionCount == 2 ); // data is still there

   // </validate clear & empty functions>
   // ============================================================================ //
   // <validate assignment operator>
   //  Just to review... we currently have these hash maps:
   //  nullHashTable AGR 0 {}   // default to autogrow
   //  testHashTable STD 4 {}
   //  fixedSizeHashTable STD 4 {}
   //  autoGrowHashTable  AGR 8 { 3425-Love, 6743-Meat, 1234-And, 1235-Beans, 9021-With Rice }

   // start by filling up fixedSizeHashTable
   fixedSizeHashTable.store (1203, &dataStringI );
   fixedSizeHashTable.store (2361, &dataStringHeart );
   fixedSizeHashTable.store (8245, &dataStringNewJersey );
   fixedSizeHashTable.store (5674, &dataStringAGreatDeal );
   testHashTable = fixedSizeHashTable;
   test.validate (
      "HashTableUInt32Template test assignment operator: STD-size:4 = STD-size:4.",
      (fixedSizeHashTable.get_size () == 4) && (fixedSizeHashTable.get_count () == 4) &&
      (testHashTable.get_size () == 4) && (testHashTable.get_count () == 4));
   test.validate (
      "HashTableUInt32Template check contents of previous (1/4).",
      testHashTable.lookup (1203) &&
      (testHashTable.lookup (1203) == &dataStringI) );
   test.validate (
      "HashTableUInt32Template check contents of previous (2/4).",
      testHashTable.lookup (2361) &&
      (testHashTable.lookup (2361) == &dataStringHeart) );
   test.validate (
      "HashTableUInt32Template check contents of previous (3/4).",
      testHashTable.lookup (8245) &&
      (testHashTable.lookup (8245) == &dataStringNewJersey) );
   test.validate (
      "HashTableUInt32Template check contents of previous (4/4).",
      testHashTable.lookup (5674) &&
      (testHashTable.lookup (5674) == &dataStringAGreatDeal) );
   testHashTable.clear(); // back to STD-4 {}

   // set an autogrow 0 to a standard 4
   nullHashTable = fixedSizeHashTable;
   test.validate (
      "HashTableUInt32Template test assignment operator: AGrow-size:0 = STD-size:4.",
      (fixedSizeHashTable.get_size () == 4) && (fixedSizeHashTable.get_count () == 4) &&
      (nullHashTable.get_size () == 4) && (nullHashTable.get_count () == 4) );
   test.validate (
      "HashTableUInt32Template check contents of previous (1/4).",
      nullHashTable.lookup (1203) &&
      (nullHashTable.lookup (1203) == &dataStringI) );
   test.validate (
      "HashTableUInt32Template check contents of previous (2/4).",
      nullHashTable.lookup (2361) &&
      (nullHashTable.lookup (2361) == &dataStringHeart) );
   test.validate (
      "HashTableUInt32Template check contents of previous (3/4).",
      nullHashTable.lookup (8245) &&
      (nullHashTable.lookup (8245) == &dataStringNewJersey) );
   test.validate (
      "HashTableUInt32Template check contents of previous (4/4).",
      nullHashTable.lookup (5674) &&
      (nullHashTable.lookup (5674) == &dataStringAGreatDeal) );
   nullHashTable.clear(); // back to AGR-4 {}

   // </validate assignment operator>
   // ============================================================================ //
   // <validate copy function>

   // start by filling up fixedSizeHashTable
   testHashTable.copy (fixedSizeHashTable);
   test.validate (
      "HashTableUInt32Template test copy Function: STD-size:4 = STD-size:4.",
      (fixedSizeHashTable.get_size () == 4) && (fixedSizeHashTable.get_count () == 4) &&
      (testHashTable.get_size () == 4) && (testHashTable.get_count () == 4) );
   test.validate (
      "HashTableUInt32Template check contents of previous (1/4).",
      testHashTable.lookup (1203) &&
      !strcmp (testHashTable.lookup (1203)->get_buffer(), dataStringI.get_buffer()) );
   test.validate (
      "HashTableUInt32Template check contents of previous (2/4).",
      testHashTable.lookup (2361) &&
      !strcmp (testHashTable.lookup (2361)->get_buffer(), dataStringHeart.get_buffer()) );
   test.validate (
      "HashTableUInt32Template check contents of previous (3/4).",
      testHashTable.lookup (8245) &&
      !strcmp (testHashTable.lookup (8245)->get_buffer(),
         dataStringNewJersey.get_buffer()));
   test.validate (
      "HashTableUInt32Template check contents of previous (4/4).",
      testHashTable.lookup (5674) &&
      !strcmp (testHashTable.lookup (5674)->get_buffer(),
         dataStringAGreatDeal.get_buffer()));

   testHashTable.empty(); // back to STD-4 {}

   // try to set a size 4 hash table to a size 8 hash table
   HashTableUInt32Template<String> biggerFixedSizeHashTable (8, 0, 0);
   biggerFixedSizeHashTable.store (3452, &dataStringI);
   biggerFixedSizeHashTable.store (5623, &dataStringLove);
   biggerFixedSizeHashTable.store (1357, &dataStringFish);
   biggerFixedSizeHashTable.store (1234, &dataStringAnd);
   biggerFixedSizeHashTable.store (4363, &dataStringMeat);
   biggerFixedSizeHashTable.store (7254, &dataStringAnd);
   biggerFixedSizeHashTable.store (7242, &dataStringNewJersey);
   biggerFixedSizeHashTable.store (4523, &dataStringWithRice);
   testHashTable.copy (biggerFixedSizeHashTable);
   test.validate (
      "HashTableUInt32Template test copy Function: STD-size:4 = STD-size:8.",
      (biggerFixedSizeHashTable.get_size () == 8) &&
         (biggerFixedSizeHashTable.get_count () == 8) && // source 8-8
         (testHashTable.get_size () == 8) && (testHashTable.get_count () == 8));
   test.validate (
      "HashTableUInt32Template check contents of previous (1/4).",
      testHashTable.lookup (3452) &&
      !strcmp (testHashTable.lookup (3452)->get_buffer(), dataStringI.get_buffer()) );
   test.validate (
      "HashTableUInt32Template check contents of previous (2/4).",
      testHashTable.lookup (5623) &&
      !strcmp (testHashTable.lookup (5623)->get_buffer(), dataStringLove.get_buffer()) );
   test.validate (
      "HashTableUInt32Template check contents of previous (3/4).",
      testHashTable.lookup (1357) &&
      !strcmp (testHashTable.lookup (1357)->get_buffer(), dataStringFish.get_buffer()) );
   test.validate (
      "HashTableUInt32Template check contents of previous (4/4).",
      testHashTable.lookup (1234) &&
      !strcmp (testHashTable.lookup (1234)->get_buffer(), dataStringAnd.get_buffer()) );
   testHashTable.empty(); // back to STD-8 {}

   // set an autogrow 0 to a standard 4
   nullHashTable.copy (fixedSizeHashTable);
   test.validate (
      "HashTableUInt32Template test copy Function: AGrow-size:0 = STD-size:4.",
      (fixedSizeHashTable.get_size () == 4) && (fixedSizeHashTable.get_count () == 4) &&
      (nullHashTable.get_size () == 4) && (nullHashTable.get_count () == 4));
   test.validate (
      "HashTableUInt32Template check contents of previous (1/4).",
      nullHashTable.lookup (1203) &&
      !strcmp (nullHashTable.lookup (1203)->get_buffer(), dataStringI.get_buffer()) );
   test.validate (
      "HashTableUInt32Template check contents of previous (2/4).",
      nullHashTable.lookup (2361) &&
      !strcmp (nullHashTable.lookup (2361)->get_buffer(), dataStringHeart.get_buffer()) );
   test.validate (
      "HashTableUInt32Template check contents of previous (3/4).",
      nullHashTable.lookup (8245) &&
      !strcmp (
         nullHashTable.lookup (8245)->get_buffer(),
         dataStringNewJersey.get_buffer()));

   test.validate (
      "HashTableUInt32Template check contents of previous (4/4).",
      nullHashTable.lookup (5674) &&
      !strcmp (
         nullHashTable.lookup (5674)->get_buffer(),
         dataStringAGreatDeal.get_buffer()));
   nullHashTable.empty(); // back to AGR-4 {}

   // </validate copy function>
   // ============================================================================ //
   // <validate iterator functions>
   HashTableUInt32Iterator itor;
   String * s1 = fixedSizeHashTable.get_first (itor);
   String * s2 = fixedSizeHashTable.get_next (itor);
   UInt32 s2key = itor.get_hash_key();
   String * s3 = fixedSizeHashTable.get_next (itor);
   String * s4 = fixedSizeHashTable.get_next (itor);
   String * s5 = fixedSizeHashTable.get_next (itor);
   test.validate (
      "HashTableUInt32Template test Iterator get_first & get_next.",
      (s1 == &dataStringI) &&
      (s2 == &dataStringHeart) &&
      (s3 == &dataStringNewJersey) &&
      (s4 == &dataStringAGreatDeal) );
   test.validate (
      "HashTableUInt32Template test Iterator get_hash_key.",
      (s2key == 2361) );
   test.validate (
      "HashTableUInt32Template test Iterator offEnd (right).",
      !s5 );

   s1 = fixedSizeHashTable.get_last (itor);
   s2 = fixedSizeHashTable.get_prev (itor);
   s3 = fixedSizeHashTable.get_prev (itor);
   s4 = fixedSizeHashTable.get_prev (itor);
   s5 = fixedSizeHashTable.get_prev (itor);
   test.validate (
      "HashTableUInt32Template test Iterator get_last & get_prev.",
      (s1 == &dataStringAGreatDeal) &&
      (s2 == &dataStringNewJersey) &&
      (s3 == &dataStringHeart) &&
      (s4 == &dataStringI) );
   test.validate (
      "HashTableUInt32Template test Iterator offEnd (left).",
      !s5 );

   // </validate iterator functions>
   // ============================================================================ //
   // <validate HashTableLock>

   // ?

   // </validate HashTableLock>
   // ============================================================================ //

   return test.result ();

   //  }---------------
   // </CODER <-- JADE>
}

