#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeInit.h>
#include <dmzRuntimeMessaging.h>
#include <dmzTest.h>

#include <string.h>


using namespace dmz;


int
main (int argc, char *argv[]) {

   
   Test test ("dmzRuntimeMessageTest", argc, argv);
   RuntimeContext *context (test.rt.get_context ());

   Config config;

   if (test.validate (
         "Looking up runtime table",
         test.config.lookup_config ("dmz.runtime", config))) {

      runtime_init (config, test.rt.get_context (), &(test.log));
   }

   const String TestTypeName ("testType");
   const String RootTypeName ("rootType");

   Message type;

   Definitions defs (context, &(test.log));

   test.validate (
      "Looking up Message: testType",
      defs.lookup_message_type (TestTypeName, type));

   test.validate (
      "Message name is set to: testType",
      type.get_name () == TestTypeName);

   test.validate (
      "Message handle is non-zero",
      type.get_handle () != 0);

   Message parent (type.get_parent ());

   test.validate (
      "Fetching parent message type",
      parent);

   test.validate (
      "Message parent name is set to: rootType",
      parent.get_name () == RootTypeName);

   test.validate (
      "Message parent handle is non-zero",
      parent.get_handle () != 0);

   // null constructor
   Message nullMessage;
   test.validate (
      "Default constructor",
      !nullMessage.get_message_type_context ());

   // standard constructors
   Message testMessageName ("testType", context);
   test.validate (
      "Constructor with \"Name\" and \"RuntimeContext\" supplied.",
      testMessageName.get_message_type_context () &&
      !(strcmp (testMessageName.get_name ().get_buffer (), "testType")));
   Message testMessageHandle (testMessageName.get_handle (), context);

   test.validate (
      "Constructor with \"Handle\" and \"RuntimeContext\" supplied.",
      testMessageHandle.get_message_type_context () &&
      !(strcmp (testMessageHandle.get_name ().get_buffer (), "testType")) &&
      (testMessageHandle == testMessageName));

   Message testMessage (testMessageHandle.get_message_type_context ());
   test.validate (
      "Constructor with \"MessageContext\" supplied.",
      testMessage.get_message_type_context () &&
      !(strcmp (testMessage.get_name ().get_buffer (), "testType")) &&
      (testMessage == testMessageName) &&
      (testMessage == testMessageHandle));

   // copy constructor
   Message copyOfTestMessage (testMessage);
   test.validate (
      "Copy Constructor.",
      copyOfTestMessage.get_message_type_context () &&
      !(strcmp (copyOfTestMessage.get_name ().get_buffer (), "testType")) &&
      (copyOfTestMessage == testMessage) &&
      (copyOfTestMessage == testMessageName) &&
      (copyOfTestMessage == testMessageHandle));


   // operators

   Message sameType (testMessage);
   Message differentType ("rootType", context);

   // == operator
   test.validate (
      "== operator",
      (testMessage == sameType) &&
      !(testMessage == differentType));

   // != operator
   test.validate (
      "!= operator",
      !(testMessage != sameType) &&
      (testMessage != differentType));

   // ! operator
   Message anotherType;
   test.validate (
      "! operator",
      (!anotherType) &&
      !(!testMessage));

   // = operator
   anotherType = testMessage;
   test.validate (
      "Assignment operator",
      (testMessage == anotherType));


   // validate set_type
   
   anotherType = differentType;
   test.validate (
      "set_type",
      (testMessage != anotherType) &&
      anotherType.set_type (testMessage.get_handle (), context) &&
      (testMessage == anotherType));

   // is_of_type and is_of_exact_type

   Message testMessageRoot ("rootType", context);
   Message testMessageTest ("testType", context);

   test.validate (
      "is_of_type",
      testMessageRoot.is_of_type (testMessageRoot) &&
      !testMessageRoot.is_of_type (testMessageTest) &&
      testMessageTest.is_of_type (testMessageRoot) &&
      testMessageTest.is_of_type (testMessageTest));

   test.validate (
      "is_of_exact_type",
      testMessageRoot.is_of_exact_type (testMessageRoot) &&
      !testMessageRoot.is_of_exact_type (testMessageTest) &&
      !testMessageTest.is_of_exact_type (testMessageRoot) &&
      testMessageTest.is_of_exact_type (testMessageTest));

   // get and set functions

   test.validate (
      "get_name",
      !strcmp (testMessageRoot.get_name ().get_buffer (), "rootType") &&
      !strcmp (testMessageTest.get_name ().get_buffer (), "testType"));

   test.validate (
      "get_handle",
      !nullMessage.get_handle () &&
      testMessage.get_handle ());

   testMessage = testMessageTest.get_parent ();
   test.validate (
      "get_parent",
      (testMessage == testMessageRoot));

   testMessage = testMessageTest;
   test.validate (
      "become_parent",
      (testMessage != testMessageRoot) &&
      testMessage.become_parent () &&
      (testMessage == testMessageRoot));


   return test.result ();
}
