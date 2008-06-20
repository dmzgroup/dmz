#include <dmzTypesStringTokenizer.h>
#include <dmzTypesString.h>
#include <dmzTypesBase.h>
#include <dmzTest.h>

#include <string.h>

using namespace dmz;


int main (int argc, char *argv[]) {

   // Sample string inputs used for testing
   String emptyString ("");
   String spaceDelim ("item1 item2 item3 item4");
   String spaceDelimCopy (spaceDelim);
   String commaDelim ("item1,item2,item3");
   String commaDelimCopy (commaDelim);
   String newlineDelim ("item1\nitem2\nitem3");
   String newlineDelimCopy (newlineDelim);
   String nullDelim ("item1\0item2\0item3", 17);
   String sequentialDelim ("item1&&item2&item3");
   String prefixDelim (",item1,item2");
   String suffixDelim ("item1,item2,");

   String item1 ("item1");
   String item1Copy (item1);
   String item2 ("item2");
   String item2Copy (item2);
   String item3 ("item3");
   String item3Copy (item3);
   String item4 ("item4");
   String item4Copy (item4);


   Test test ("dmzTypesStringTokenizerTest", argc, argv);

   // Constructor Test - Default delimiter, Single Item

   StringTokenizer stringTokConsDefaultSingleItem (item1);
   test.validate (
      "Default constructor with space delimiter and single item - item 1",
      (stringTokConsDefaultSingleItem.get_next () == item1Copy) &&
      item1 == item1Copy); // Ensure that original inputted string was not modified

   test.validate (
      "Default constructor with space delimiter and single items - item after last - 1",
      stringTokConsDefaultSingleItem.get_next ().is_null());

   test.validate (
      "Default constructor with space delimiter and single items - item after last - 2",
      stringTokConsDefaultSingleItem.get_next ().is_null ());



   // Constructor Test - Default delimiter, Multiple Items

   StringTokenizer stringTokConsDefaultMultiItem (spaceDelim);

   test.validate (
      "Default constructor with space delimiter and multiple items - item 1",
      (stringTokConsDefaultMultiItem.get_next () == item1) &&
      // Ensure that original inputted string was not modified
      (spaceDelim == spaceDelimCopy));

   test.validate (
      "Default constructor with space delimiter and multiple items - item 2",
      (stringTokConsDefaultMultiItem.get_next () == item2));

   test.validate (
      "Default constructor with space delimiter and multiple items - item 3",
      (stringTokConsDefaultMultiItem.get_next () == item3));

   test.validate (
      "Default constructor with space delimiter and multiple items - item 4",
      (stringTokConsDefaultMultiItem.get_next () == item4));

   test.validate (
      "Default constructor with space delimiter and multiple items - item after last - 1",
      stringTokConsDefaultMultiItem.get_next ().is_null ());

   test.validate (
      "Default constructor with space delimiter and multiple items - item after last - 2",
      stringTokConsDefaultMultiItem.get_next ().is_null ());



  // Constructor Test - Comma delimiter

   StringTokenizer stringTokConsComma (commaDelim, ',');

   test.validate (
      "Comma delimiter - item 1",
      (stringTokConsComma.get_next () == item1));

   test.validate (
      "Comma delimiter - item 2",
      (stringTokConsComma.get_next () == item2));

   test.validate (
      "Comma delimiter - item 3",
      (stringTokConsComma.get_next () == item3));



   // Constructor Test - Newline delimiter

   StringTokenizer stringTokConsNewline (newlineDelim, '\n');

   test.validate (
      "Newline delimiter - item 1",
      (stringTokConsNewline.get_next () == item1));

   test.validate (
      "Newline delimiter - item 2",
      (stringTokConsNewline.get_next () == item2));

   test.validate (
      "Newline delimiter - item 3",
      (stringTokConsNewline.get_next () == item3));


   // Constructor Test - Null character delimiter

   StringTokenizer stringTokConsNull (nullDelim, '\0');

   test.validate (
      "Null Character delimiter - item 1",
      (stringTokConsNull.get_next () == item1));

   test.validate (
      "Null Character delimiter - item 2",
      (stringTokConsNull.get_next () == item2));

   test.validate (
      "Null Character delimiter - item 3",
      (stringTokConsNull.get_next () == item3));


   // Constructor Test - Sequential delimiter

   StringTokenizer stringTokConsSequential (sequentialDelim, '&');

   test.validate (
      "Sequential delimiter - item 1",
      (stringTokConsSequential.get_next () == item1));

   test.validate (
      "Sequential delimiter - item 2",
      (stringTokConsSequential.get_next () == item2));

   test.validate (
      "Sequential delimiter - item 3",
      (stringTokConsSequential.get_next () == item3));

   // Constructor Test -  Delimiter prefix

   StringTokenizer stringTokConsPrefix (prefixDelim, ',');

   test.validate (
      "Delimiter prefix - item 1",
      (stringTokConsPrefix.get_next () == item1));

   test.validate (
      "Delimiter prefix - item 2",
      (stringTokConsPrefix.get_next () == item2));


   // Constructor Test -  Delimiter suffix

   StringTokenizer stringTokConsSuffix (suffixDelim, ',');

   test.validate (
      "Delimiter suffix - item 1",
      (stringTokConsSuffix.get_next () == item1));

   test.validate (
      "Delimiter suffix - item 2",
      (stringTokConsSuffix.get_next () == item2));


   // Reset Test

   StringTokenizer stringTokReset (spaceDelim);
   stringTokReset.get_next ();
   stringTokReset.get_next ();
   stringTokReset.reset ();
   test.validate (
      "Reset function",
      (stringTokReset.get_next () == item1));


   return test.result ();
}
