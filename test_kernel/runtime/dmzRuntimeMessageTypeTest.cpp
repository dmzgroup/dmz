#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeInit.h>
#include <dmzRuntimeMessaging.h>
#include <dmzTest.h>

#include <string.h>


using namespace dmz;


int
main (int argc, char *argv[]) {

   
   Test test ("dmzRuntimeMessageTypeTest", argc, argv);
   RuntimeContext *context (test.rt.get_context ());

   Config config;

   if (test.validate (
         "Looking up runtime table",
         test.config.lookup_config ("dmz.runtime", config))) {

      runtime_init (config, test.rt.get_context (), &(test.log));
   }

   const String TestTypeName ("testType");
   const String RootTypeName ("rootType");

   MessageType type;

   Definitions defs (context, &(test.log));

   test.validate (
      "Looking up MessageType: testType",
      defs.lookup_message_type (TestTypeName, type));

   test.validate (
      "MessageType name is set to: testType",
      type.get_name () == TestTypeName);

   test.validate (
      "MessageType handle is non-zero",
      type.get_handle () != 0);

   MessageType parent;

   test.validate (
      "Fetching parent message type",
      type.get_parent (parent));

   test.validate (
      "MessageType parent name is set to: rootType",
      parent.get_name () == RootTypeName);

   test.validate (
      "MessageType parent handle is non-zero",
      parent.get_handle () != 0);

   // null constructor
   MessageType nullMessageType;
   test.validate (
      "Default constructor",
      !nullMessageType.get_message_type_context ());

   // standard constructors
   MessageType testMessageTypeName ("testType", context);
   test.validate (
      "Constructor with \"Name\" and \"RuntimeContext\" supplied.",
      testMessageTypeName.get_message_type_context () &&
      !(strcmp (testMessageTypeName.get_name ().get_buffer (), "testType")));
   MessageType testMessageTypeHandle (testMessageTypeName.get_handle (), context);

   test.validate (
      "Constructor with \"Handle\" and \"RuntimeContext\" supplied.",
      testMessageTypeHandle.get_message_type_context () &&
      !(strcmp (testMessageTypeHandle.get_name ().get_buffer (), "testType")) &&
      (testMessageTypeHandle == testMessageTypeName));

   MessageType testMessageType (testMessageTypeHandle.get_message_type_context ());
   test.validate (
      "Constructor with \"MessageTypeContext\" supplied.",
      testMessageType.get_message_type_context () &&
      !(strcmp (testMessageType.get_name ().get_buffer (), "testType")) &&
      (testMessageType == testMessageTypeName) &&
      (testMessageType == testMessageTypeHandle));

   // copy constructor
   MessageType copyOfTestMessageType (testMessageType);
   test.validate (
      "Copy Constructor.",
      copyOfTestMessageType.get_message_type_context () &&
      !(strcmp (copyOfTestMessageType.get_name ().get_buffer (), "testType")) &&
      (copyOfTestMessageType == testMessageType) &&
      (copyOfTestMessageType == testMessageTypeName) &&
      (copyOfTestMessageType == testMessageTypeHandle));


   // operators

   MessageType sameType (testMessageType);
   MessageType differentType ("rootType", context);

   // == operator
   test.validate (
      "== operator",
      (testMessageType == sameType) &&
      !(testMessageType == differentType));

   // != operator
   test.validate (
      "!= operator",
      !(testMessageType != sameType) &&
      (testMessageType != differentType));

   // ! operator
   MessageType anotherType;
   test.validate (
      "! operator",
      (!anotherType) &&
      !(!testMessageType));

   // = operator
   anotherType = testMessageType;
   test.validate (
      "Assignment operator",
      (testMessageType == anotherType));


   // validate set_type
   
   anotherType = differentType;
   test.validate (
      "set_type",
      (testMessageType != anotherType) &&
      anotherType.set_type (testMessageType.get_handle (), context) &&
      (testMessageType == anotherType));

   // is_of_type and is_of_exact_type

   MessageType testMessageTypeRoot ("rootType", context);
   MessageType testMessageTypeTest ("testType", context);

   test.validate (
      "is_of_type",
      testMessageTypeRoot.is_of_type (testMessageTypeRoot) &&
      !testMessageTypeRoot.is_of_type (testMessageTypeTest) &&
      testMessageTypeTest.is_of_type (testMessageTypeRoot) &&
      testMessageTypeTest.is_of_type (testMessageTypeTest));

   test.validate (
      "is_of_exact_type",
      testMessageTypeRoot.is_of_exact_type (testMessageTypeRoot) &&
      !testMessageTypeRoot.is_of_exact_type (testMessageTypeTest) &&
      !testMessageTypeTest.is_of_exact_type (testMessageTypeRoot) &&
      testMessageTypeTest.is_of_exact_type (testMessageTypeTest));

   // get and set functions

   test.validate (
      "get_name",
      !strcmp (testMessageTypeRoot.get_name ().get_buffer (), "rootType") &&
      !strcmp (testMessageTypeTest.get_name ().get_buffer (), "testType"));

   test.validate (
      "get_handle",
      !nullMessageType.get_handle () &&
      testMessageType.get_handle ());

   testMessageType = testMessageTypeTest;
   test.validate (
      "get_parent",
      (testMessageType != testMessageTypeRoot) &&
      testMessageTypeTest.get_parent (testMessageType) &&
      (testMessageType == testMessageTypeRoot));

   testMessageType = testMessageTypeTest;
   test.validate (
      "become_parent",
      (testMessageType != testMessageTypeRoot) &&
      testMessageType.become_parent () &&
      (testMessageType == testMessageTypeRoot));


   return test.result ();
}
