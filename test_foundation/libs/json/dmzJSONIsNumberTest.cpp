#include <dmzJSONUtil.h>
#include <dmzTypesString.h>
#include <dmzTest.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   Test test ("dmzJSONIsNumber", argc, argv);

   String str ("0");
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "1";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "1234567890";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "0.1";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "0e0";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "0.0";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "0.0e0";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "0.01";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "0.01e0";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "10";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "1.2";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "1.2e0";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "1.2e1";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "1.2e+1";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "1.2e-1";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "1.2E+1";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "1.2E-1";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "-1.2";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "10.002";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "-10.002";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "10.002e100";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "-10.002e100";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "10.002e-100";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "-10.002e-100";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = "10.002e+100";
   test.validate (
      str + " is a number",
      json_is_number (str));

   str = ".";
   test.validate (
      str + " is NOT a number",
      json_is_number (str) == False);

   str = "00";
   test.validate (
      str + " is NOT a number",
      json_is_number (str) == False);

   str = "0.";
   test.validate (
      str + " is NOT a number",
      json_is_number (str) == False);

   str = ".0";
   test.validate (
      str + " is NOT a number",
      json_is_number (str) == False);

   str = "1.2.3";
   test.validate (
      str + " is NOT a number",
      json_is_number (str) == False);

   str = "0.0e01";
   test.validate (
      str + " is MOT a number",
      json_is_number (str) == False);

   str = "0.01e01";
   test.validate (
      str + " is MOT a number",
      json_is_number (str) == False);

   str = "11e01";
   test.validate (
      str + " is MOT a number",
      json_is_number (str) == False);

   str = "1.1e01";
   test.validate (
      str + " is MOT a number",
      json_is_number (str) == False);

   str = "1e0.1";
   test.validate (
      str + " is NOT a number",
      json_is_number (str) == False);

   str = "1e-0.1";
   test.validate (
      str + " is NOT a number",
      json_is_number (str) == False);

   str = "1e1.1";
   test.validate (
      str + " is NOT a number",
      json_is_number (str) == False);

   str = "1e1e1";
   test.validate (
      str + " is NOT a number",
      json_is_number (str) == False);

   str = "a";
   test.validate (
      str + " is NOT a number",
      json_is_number (str) == False);

   str = "1a";
   test.validate (
      str + " is NOT a number",
      json_is_number (str) == False);

   str = "1.a";
   test.validate (
      str + " is NOT a number",
      json_is_number (str) == False);

   str = "1.0ea";
   test.validate (
      str + " is NOT a number",
      json_is_number (str) == False);

   str = "1.0e-a";
   test.validate (
      str + " is NOT a number",
      json_is_number (str) == False);

   return test.result ();
}

