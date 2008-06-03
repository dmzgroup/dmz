#include <dmzRuntimeData.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeInit.h>
#include <dmzRuntimeLog.h>
#include <dmzTest.h>

#include <dmzRuntimeObjectType.h>
#include <string.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   Test test ("dmzRuntimeObjectTypeTest", argc, argv);

   // test null contructor
   ObjectType testObjectType;
   test.validate (
      "test default constructor",
      !testObjectType.get_type_context () && 
      !testObjectType.get_name ().get_length ());


   RuntimeContext *context (test.rt.get_context ());
   Config config;

   if (test.validate (
         "Looking up runtime table",
         test.config.lookup_config ("dmz.runtime", config))) {

      runtime_init (config, test.rt.get_context (), &(test.log));
   }

   Definitions defs (context, &(test.log));

   const String CaracalName ("caracal");
   const String CatName ("cat");

   ObjectType rootObjectType;

   test.validate (
      "Looking up ObjectType: Caracal",
      defs.lookup_object_type (CaracalName, testObjectType));

   test.validate (
      "Looking up ObjectType: Cat",
      defs.lookup_object_type (CatName, rootObjectType));

   // ============================================================================ //
   // <validate constructors>

   ObjectType copyOfCaracalContext (testObjectType.get_type_context ());
   test.validate (
      "test (context) constructor",
      copyOfCaracalContext == testObjectType);

   ObjectType anotherCaracal (CaracalName, context);
   ObjectType anotherCat (CatName, context);

   test.validate (
      "test (Name, RuntimeContext) constructor",
      anotherCat == rootObjectType &&
      anotherCaracal == testObjectType);

   ObjectType yetAnotherCaracal (anotherCaracal.get_handle (), context);
   ObjectType yetAnotherCat (anotherCat.get_handle (), context);
   test.validate (
      "test (Handle, RuntimeContext) constructor",
      yetAnotherCat == rootObjectType &&
      yetAnotherCaracal == testObjectType);

   ObjectType stillAnotherCaracal (anotherCaracal);
   ObjectType stillAnotherCat (anotherCat);
   test.validate (
      "test copy constructor",
      stillAnotherCat == rootObjectType &&
      stillAnotherCaracal == testObjectType);

   // </validate constructors>
   // ============================================================================ //
   // <validate operators>

   ObjectType shouldBeNothing;
   ObjectType nothingThatBecomesSomething;
   test.validate (
      "test = operator - (pre state)",
      (nothingThatBecomesSomething == shouldBeNothing) &&
      !(nothingThatBecomesSomething == anotherCat));
   
   nothingThatBecomesSomething = anotherCat;
   test.validate (
      "test = operator - (equal something)",
      !(nothingThatBecomesSomething == shouldBeNothing) &&
      (nothingThatBecomesSomething == anotherCat));
   
   nothingThatBecomesSomething = shouldBeNothing;
   test.validate (
      "test = operator - (equal null)",
      (nothingThatBecomesSomething == shouldBeNothing) &&
      !(nothingThatBecomesSomething == anotherCat));

   ObjectType shouldBeACaracal (CaracalName, context);
   ObjectType shouldBeACat (CatName, context);
   test.validate (
      "test == operator",
      !(shouldBeNothing == anotherCat) &&
      !(shouldBeNothing == anotherCaracal) &&
      !(shouldBeACaracal == anotherCat) &&
      (shouldBeACaracal == anotherCaracal) &&
      (shouldBeACat == anotherCat) &&
      !(shouldBeACat == anotherCaracal));
   test.validate (
      "test != operator",
      (shouldBeNothing != anotherCat) &&
      (shouldBeNothing != anotherCaracal) &&
      (shouldBeACaracal != anotherCat) &&
      !(shouldBeACaracal != anotherCaracal) &&
      !(shouldBeACat != anotherCat) &&
      (shouldBeACat != anotherCaracal));
   test.validate (
      "test ! operator",
      !shouldBeNothing &&
      !(!shouldBeACat));

   // </validate operators>
   // ============================================================================ //
   // <validate accessor functions>

   ObjectType newTestObject;

   test.validate (
      "test is_of_type operator on a null type object",
      !newTestObject.is_of_type (shouldBeNothing) &&
      !newTestObject.is_of_type (anotherCat) &&
      !newTestObject.is_of_type (anotherCaracal));
   test.validate (
      "test is_of_exact_type operator on a null type object",
      !newTestObject.is_of_exact_type (shouldBeNothing) &&
      !newTestObject.is_of_exact_type (anotherCat) &&
      !newTestObject.is_of_exact_type (anotherCaracal));

   newTestObject = anotherCat;
   test.validate (
      "test is_of_type operator on a base type object",
      !newTestObject.is_of_type (shouldBeNothing) &&
      newTestObject.is_of_type (anotherCat) &&
      !newTestObject.is_of_type (anotherCaracal));
   test.validate (
      "test is_of_exact_type operator on a base type object",
      !newTestObject.is_of_exact_type (shouldBeNothing) &&
      newTestObject.is_of_exact_type (anotherCat) &&
      !newTestObject.is_of_exact_type (anotherCaracal));

   newTestObject = anotherCaracal;
   test.validate (
      "test is_of_type operator on a child type object",
      !newTestObject.is_of_type (shouldBeNothing) &&
      newTestObject.is_of_type (anotherCat) &&
      newTestObject.is_of_type (anotherCaracal));
   test.validate (
      "test is_of_exact_type operator on a child type object",
      !newTestObject.is_of_exact_type (shouldBeNothing) &&
      !newTestObject.is_of_exact_type (anotherCat) &&
      newTestObject.is_of_exact_type (anotherCaracal));

   // test set_type
   ObjectType anotherTestObject;
   test.validate (
      "test set_type (Name, context)",
      newTestObject.set_type (CatName, context) &&
      newTestObject.is_of_type (anotherCat) &&
      newTestObject.is_of_exact_type (anotherCat));

   anotherTestObject = shouldBeNothing;
   test.validate (
      "test set_type (Handle, context)",
      newTestObject.set_type (anotherCaracal.get_handle (), context) &&
      newTestObject.is_of_type (anotherCaracal) &&
      newTestObject.is_of_exact_type (anotherCaracal));

   test.validate (
      "test get_name",
      (newTestObject.get_name () == CaracalName));

   test.validate (
      "test get_handle",
      (newTestObject.get_handle () == anotherCaracal.get_handle ()));

   // get / become parent
   ObjectType newKitty (CaracalName, context);
   test.validate (
      "test get_parent",
      newKitty.get_parent () == anotherCat);

   test.validate (
      "test become_parent",
      (newKitty == anotherCaracal) &&
      newKitty.become_parent () &&
      (newKitty == anotherCat));

   // </validate accessor functions>
   // ============================================================================ //
   // <validate lookup functions>

   String data;
   anotherCaracal.get_config ().lookup_attribute ("info.texture", data);
   test.validate (
      "test lookup_attribute",
      !data);

   String data2, data3;
   Config returnCaracalData;
   test.validate (
      "test lookup_config",
      anotherCaracal.get_config ().lookup_all_config_merged ("info", returnCaracalData));
   
   test.validate (
      "test lookup_config->lookup_attribute -- success",
      returnCaracalData.lookup_attribute ("ears", data2) &&
      data2 == "awesome");
   test.validate (
      "test lookup_config->lookup_attribute -- failure",
      !returnCaracalData.lookup_attribute ("howTo", data3) &&
      !data3);
//   test.log.out << "M: <" << data2 << ">" << endl;
//   test.log.out << "M: <" << data3 << ">" << endl;

   test.validate (
      "test lookup_all_config",
      anotherCaracal.get_config ().lookup_all_config ("info", returnCaracalData));

   ConfigIterator dataIt;
   Config moreSpecificData;
   test.validate (
      "test lookup_all_config->get_first_config",
      returnCaracalData.get_first_config (dataIt, moreSpecificData));

   ConfigIterator it;
   String dataName, dataValue;
   test.validate (
      "test lookup_all_config->get_first_attribute",
      moreSpecificData.get_first_attribute (it, dataName, dataValue) &&
      (dataName == "ears") &&
      (dataValue == "awesome"));
   test.validate (
      "test lookup_all_config->get_next_attribute",
      moreSpecificData.get_next_attribute (it, dataName, dataValue) &&
      (dataName == "madeOf") &&
      (dataValue == "win"));

   test.validate (
      "test lookup_all_config->get_next_attribute (failure)",
      !moreSpecificData.get_next_attribute (it, dataName, dataValue));
//   test.log.out << "M: <" << dataName1 << ":" << dataValue1 << ">" << endl;
//   test.log.out << "M: <" << dataName2 << ":" << dataValue2 << ">" << endl;

   test.validate (
      "test lookup_all_config->get_next_config",
      returnCaracalData.get_next_config (dataIt, moreSpecificData));

   test.validate (
      "test lookup_all_config->get_first_attribute",
      moreSpecificData.get_first_attribute (it, dataName, dataValue) &&
      (dataName == "motivation") &&
      (dataValue == "evil"));
//   test.log.out << "M: <" << dataName << ":" << dataValue << ">" << endl;

   // </validate lookup functions>
   // ============================================================================ //
   // <validate child iterator functions>

   ObjectTypeIterator oIt;
   ObjectType otype;
   test.validate (
      "test iterator get_first_child ()",
      anotherCat.get_first_child (oIt, otype) &&
      otype == anotherCaracal);

   ObjectType aServal("serval", context);
   test.validate (
      "test iterator get_next_child ()",
      anotherCat.get_next_child (oIt, otype) &&
      otype == aServal);

   test.validate (
      "test iterator get_next_child ()",
      !anotherCat.get_next_child (oIt, otype));

   // </validate child iterator functions>
   // ============================================================================ //

   return test.result ();
}
