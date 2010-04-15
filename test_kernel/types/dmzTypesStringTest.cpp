#include <dmzTypesString.h>
#include <dmzTest.h>

#include <string.h>
#include <stdio.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   Test test ("dmzTypesStringTest", argc, argv);

   String str1;

   test.validate (
      "Default string constructor creates empty string.",
      !str1.get_buffer () && !str1.get_length () && !str1.get_size ());

   String str2 ("Foo");
   const Int32 Str2Len (str2.get_length ());
   test.validate (
      "String (char *) constructor creates string of correct length and size.",
      str2.get_buffer () && (Str2Len == 3) && (str2.get_size () > Str2Len));

   String str3 ("aaa");
   String str4 ("bbb");
   str3 << str4;
   const Int32 Str3Len (str3.get_length ());
   test.validate (
      "String to String stream operator.",
      !strcmp ("aaabbb", str3.get_buffer ()) &&
         (Str3Len == 6) &&
         (Str3Len < str3.get_size ()));

   const char *CharData = "Test String Value";
   const int CharDataLength = strlen (CharData);
   String inputValue (CharData);
   String copyCtrResult (inputValue);
   test.validate (
      "String copy constructor contains valid string.",
      (copyCtrResult.get_length () == CharDataLength) &&
      (!strcmp (copyCtrResult.get_buffer (), CharData)));

   String str5 ("..a...b....c.....d...");
   str5.replace_sub ("..", ".");
   test.validate (
      "String replace_sub sequential pattern.",
      !strcmp (str5.get_buffer (), ".a.b.c.d.") &&
         (str5.get_length () == 9) &&
         (str5.get_length () < str5.get_size ()));

   String str6 ("123456789");
   str6.flush ();
   test.validate (
      "String flush function.",
      !strcmp ("" , str6.get_buffer ()) &&
         (str6.get_length () == 0) &&
         (str6.get_size () >= 10));

   String str7 ("123456789");
   str7.empty ();
   test.validate (
      "String empty function.",
      (str7.get_buffer () == 0) &&
         (str7.get_length () == 0) &&
         (str7.get_size () == 0));

   String str8;
   str8.repeat ("123", 3);
   test.validate (
      "String repeat function.",
      !strcmp ("123123123", str8.get_buffer ()) &&
         (str8.get_length () == 9) &&
         (str8.get_size () >= 10));

   String str9 ("123456789aaa");
   str9.shift (-9);
   test.validate (
      "String shift left.",
      !strcmp ("aaa", str9.get_buffer ()) &&
      (str9.get_length () == 3) &&
      (str9.get_size () > 3));

   String str10 ("123456789");
   str10.shift (-9);
   test.validate (
      "String shift left to flush.",
      !strcmp ("", str10.get_buffer ()) &&
      (str10.get_length () == 0) &&
      (str10.get_size () >= 1));

   String str11 ("aaa");
   str11.shift (6, 'b');
   test.validate (
      "String shift right.",
      !strcmp ("bbbbbbaaa", str11.get_buffer ()) &&
      (str11.get_length () == 9) &&
      (str11.get_size () > 9));

   String str12 ("123456789");
   str12.flush ();
   test.validate (
      "Flush - String empty but not null.",
      !str12 && !str12.is_null ());

   String str13 ("123456789");
   str13.empty ();
   test.validate (
      "Empty - String empty and null.",
      !str13 && str13.is_null ());

   String strAppend1 ("12345");
   String strAppend2 ("67890");
   String strResultAppend1 = strAppend1 + strAppend2;
   String strResultAppend2 ("");
   strResultAppend2 = strResultAppend2.append (strAppend1);
   strResultAppend2 = strResultAppend2.append (strAppend2);

   test.validate (
      "Appending strings with plus operator.",
      !strcmp ("1234567890", strResultAppend1.get_buffer ()));

   test.validate (
      "Appending strings with append function.",
      !strcmp ("1234567890", strResultAppend2.get_buffer ()));

   String strRepeat ("repeat!");
   String strRepeatResult ("");
   strRepeatResult = strRepeatResult.repeat (strRepeat, 4);

   test.validate (
      "Repeating a string",
      !strcmp ("repeat!repeat!repeat!repeat!", strRepeatResult.get_buffer ()));

   String strBoolean1 ("this is a test string!");
   String strBoolean2 ("this is a test string!");
   String strBoolean3 ("this is 4 test string!");

   test.validate (
      "String equality operator",
      strBoolean1 == strBoolean2);

   test.validate (
      "String inequality operator",
      strBoolean1 != strBoolean3);

   String strToUpperInitial ("this is a test string with symbols$%^!&*(");
   String strToLowerInitial ("THIS IS A TEST STRING WITH SYMBOLS$%^!&*(");

   String strToUpperFuncResult = strToUpperInitial.get_upper ();
   String strToLowerFuncResult = strToUpperInitial.get_lower ();

   strToUpperInitial.to_upper ();
   strToLowerInitial.to_lower ();

   test.validate (
      "To upper case inplace",
      !strcmp ("THIS IS A TEST STRING WITH SYMBOLS$%^!&*(",
               strToUpperInitial.get_buffer ()));
   test.validate (
      "To lower case inplace",
      !strcmp ("this is a test string with symbols$%^!&*(",
               strToLowerInitial.get_buffer ()));

   test.validate (
      "To upper case conversion function",
      !strcmp ("THIS IS A TEST STRING WITH SYMBOLS$%^!&*(",
               strToUpperFuncResult.get_buffer ()));
   test.validate (
      "To lower case conversion function",
      !strcmp ("this is a test string with symbols$%^!&*(",
               strToLowerFuncResult.get_buffer ()));

   String strSetGetChar ("abcdefghijk");
   const char grabbedChar1 = strSetGetChar.get_char (3);
   const char grabbedChar2 = strSetGetChar.get_char (5);

   test.validate (
      "Get a character - 1",
      (grabbedChar1 == 'd'));

   test.validate (
      "Get a character - 2",
      (grabbedChar2 == 'f'));

   test.validate (
      "Set a character - 1",
      strSetGetChar.set_char (3, 'x') &&
      !strcmp ("abcxefghijk", strSetGetChar.get_buffer ()));

   test.validate (
      "Set a character - 2",
      strSetGetChar.set_char (5, 'y') &&
      !strcmp ("abcxeyghijk", strSetGetChar.get_buffer ()));

   String strGetSub ("a1b2c3d4e5f6g7");
   String strSubRecieved1 = strGetSub.get_sub (2);
   String strSubRecieved2 = strGetSub.get_sub (2, 3);
   String strSubRecieved3 = strGetSub.get_sub (2, 2);
   String strSubRecieved4 = strGetSub.get_sub (12, 13);
   String strSubRecieved5 = strGetSub.get_sub (14);
   String strSubRecieved6 = strGetSub.get_sub (-1);
   String strSubRecieved7 = strGetSub.get_sub (-5);
   String strSubRecieved8 = strGetSub.get_sub (-15);


   test.validate (
      "Get substring - single index",
      strSubRecieved1 == String ("b2c3d4e5f6g7"));

   test.validate (
      "Get substring - index, index + 1",
      strSubRecieved2 == String ("b2"));

   test.validate (
      "Get substring - index, index",
      strSubRecieved3 == String ("b"));

   test.validate (
      "Get substring - lastIndex - 1, lastIndex",
      strSubRecieved4 == String ("g7"));

   test.validate (
      "Get substring - out of bounds after last index",
      strSubRecieved5.is_null ());

   test.validate (
      "Get substring - negative index, last character",
      strSubRecieved6 ==  String ("7"));

   test.validate (
      "Get substring - negative index, last 5 characters",
      strSubRecieved7 ==  String ("5f6g7"));

   test.validate (
      "Get substring - negative index with magnitude too large",
      strSubRecieved8.is_null ());



   Int32 findIndex;
   String strFindSubTarget ("item");
   String strFindSub1 ("01234567item0123item012345item01");
   String strFindSub2 ("01234item");
   Boolean findSubResult1 = strFindSub1.find_sub (strFindSubTarget, findIndex);

   test.validate (
      "Find substring - initial occurance",
      findSubResult1 &&
      (findIndex == 8));

   Boolean findSubResult2 = strFindSub1.find_sub (
      strFindSubTarget,
      findIndex,
      findIndex + strFindSubTarget.get_length());

   test.validate (
      "Find substring - occurance 2",
      findSubResult2 &&
      (findIndex == 16));

   Boolean findSubResult3 = strFindSub1.find_sub (
      strFindSubTarget,
      findIndex,
      findIndex + strFindSubTarget.get_length());

   test.validate (
      "Find substring - initial occurance",
      findSubResult3 &&
      (findIndex == 26));

   Boolean findSubResult4 = strFindSub1.find_sub (
      strFindSubTarget,
      findIndex,
      findIndex + strFindSubTarget.get_length());

   test.validate (
      "Find substring - no more occurances",
      !findSubResult4);

   Int32 findIndex2;
   Boolean findSubResult5 = strFindSub2.find_sub (strFindSubTarget, findIndex2);
   test.validate (
      "Find substring - initial occurance (test 2)",
      findSubResult5 &&
      (findIndex2 == 5));

   Boolean findSubResult6 = strFindSub2.find_sub (
      strFindSubTarget,
      findIndex2,
      findIndex2 + strFindSubTarget.get_length());

   test.validate (
      "Find substring - no more occurances (test 2)",
      !findSubResult6);

   Int32 index (-1);
   String strFindSub3 ("String to compare.");
   String strFindSub4 (strFindSub3);

   test.validate (
      "find_sub returns True when sub string is the same",
      strFindSub3.find_sub (strFindSub4, index));


   return test.result ();
}
