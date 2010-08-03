#include <dmzTypesString.h>
#include <dmzTypesStringSub.h>
#include <dmzTest.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   Test test ("dmzTypesStringSubTest", argc, argv);

   StringSub sub1;

   sub1.store ("foo", "new value");
   sub1.store ("moo", "different");
   sub1.store ("goo", "a value");

   String s1 ("This is a $(foo).");

   test.validate (
      "Convert $(foo) to 'new value'.",
      sub1.convert (s1) == "This is a new value.");

   s1 = "This is a $(zoo).";

   test.validate (
      "Convert $(zoo) to empty string.",
      sub1.convert (s1) == "This is a .");

   s1 = "This is a $(foo.";

   test.validate (
      "No conversion of open variable",
      sub1.convert (s1) == s1);

   s1 = "This is a $(foo $(moo).";

   test.validate (
      "Conversion inside open variable",
      sub1.convert (s1) == "This is a $(foo different.");

   s1 = "This is a \\$(foo).";

   test.validate (
      "Escape char.",
      sub1.convert (s1) == "This is a $(foo).");


   StringSub sub2;

   sub2.store ("foo-one", "new value");
   sub2.store ("foo-two", "different");
   sub2.store ("moo", "one");
   sub2.store ("goo", "two");

   String s2 ("This is a $(foo-$(moo)).");

   test.validate (
      "Convert $(foo-$(moo)) to 'new value'.",
      sub2.convert (s2) == "This is a new value.");

   StringSub sub3;

   sub3.store ("foo", "$(goo)");
   sub3.store ("goo", "new value");

   String s3 ("This is a $(foo).");

   test.validate (
      "Convert $(foo) to 'new value' where foo == $(goo).",
      sub3.convert (s3) == "This is a new value.");

   return test.result ();
}
