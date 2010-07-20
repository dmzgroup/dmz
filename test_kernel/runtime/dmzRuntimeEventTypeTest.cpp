#include <dmzRuntimeData.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeInit.h>
#include <dmzRuntimeLog.h>
#include <dmzTest.h>

#include <dmzRuntimeEventType.h>
#include <string.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   Test test ("dmzRuntimeEventTypeTest", argc, argv);

   // test null contructor
   EventType testEventType;
   test.validate (
      "test default constructor",
      !testEventType.get_type_context () &&
      !testEventType.get_name ().get_length ());


   RuntimeContext *context (test.rt.get_context ());
   Config config;

   if (test.validate (
         "Looking up runtime table",
         test.config.lookup_config ("dmz.runtime", config))) {

      runtime_init (config, test.rt.get_context (), &(test.log));
   }

   const String CaracalName ("caracal");
   const String CatName ("cat");

   EventType rootEventType;

   Definitions defs (context, &(test.log));

   test.validate (
      "Looking up EventType: Caracal",
      defs.lookup_event_type (CaracalName, testEventType));

   test.validate (
      "Looking up EventType: Cat",
      defs.lookup_event_type (CatName, rootEventType));

   // ============================================================================ //
   // <validate constructors>

   EventType copyOfCaracalContext (testEventType.get_type_context ());
   test.validate (
      "test (context) constructor",
      copyOfCaracalContext == testEventType);

   EventType anotherCaracal (CaracalName, context);
   EventType anotherCat (CatName, context);

   test.validate (
      "test (Name, RuntimeContext) constructor",
      anotherCat == rootEventType &&
      anotherCaracal == testEventType);

   EventType yetAnotherCaracal (anotherCaracal.get_handle (), context);
   EventType yetAnotherCat (anotherCat.get_handle (), context);
   test.validate (
      "test (Handle, RuntimeContext) constructor",
      yetAnotherCat == rootEventType &&
      yetAnotherCaracal == testEventType);

   EventType stillAnotherCaracal (anotherCaracal);
   EventType stillAnotherCat (anotherCat);
   test.validate (
      "test copy constructor",
      stillAnotherCat == rootEventType &&
      stillAnotherCaracal == testEventType);

   // </validate constructors>
   // ============================================================================ //
   // <validate operators>

   EventType shouldBeNothing;
   EventType nothingThatBecomesSomething;
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

   EventType shouldBeACaracal (CaracalName, context);
   EventType shouldBeACat (CatName, context);
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

   EventType newTestEvent;

   test.validate (
      "test is_of_type operator on a null type event",
      !newTestEvent.is_of_type (shouldBeNothing) &&
      !newTestEvent.is_of_type (anotherCat) &&
      !newTestEvent.is_of_type (anotherCaracal));
   test.validate (
      "test is_of_exact_type operator on a null type event",
      !newTestEvent.is_of_exact_type (shouldBeNothing) &&
      !newTestEvent.is_of_exact_type (anotherCat) &&
      !newTestEvent.is_of_exact_type (anotherCaracal));

   newTestEvent = anotherCat;
   test.validate (
      "test is_of_type operator on a base type event",
      !newTestEvent.is_of_type (shouldBeNothing) &&
      newTestEvent.is_of_type (anotherCat) &&
      !newTestEvent.is_of_type (anotherCaracal));
   test.validate (
      "test is_of_exact_type operator on a base type event",
      !newTestEvent.is_of_exact_type (shouldBeNothing) &&
      newTestEvent.is_of_exact_type (anotherCat) &&
      !newTestEvent.is_of_exact_type (anotherCaracal));

   newTestEvent = anotherCaracal;
   test.validate (
      "test is_of_type operator on a child type event",
      !newTestEvent.is_of_type (shouldBeNothing) &&
      newTestEvent.is_of_type (anotherCat) &&
      newTestEvent.is_of_type (anotherCaracal));
   test.validate (
      "test is_of_exact_type operator on a child type event",
      !newTestEvent.is_of_exact_type (shouldBeNothing) &&
      !newTestEvent.is_of_exact_type (anotherCat) &&
      newTestEvent.is_of_exact_type (anotherCaracal));

   // test set_type
   EventType anotherTestEvent;
   test.validate (
      "test set_type (Name, context)",
      newTestEvent.set_type (CatName, context) &&
      newTestEvent.is_of_type (anotherCat) &&
      newTestEvent.is_of_exact_type (anotherCat));

   anotherTestEvent = shouldBeNothing;
   test.validate (
      "test set_type (Handle, context)",
      newTestEvent.set_type (anotherCaracal.get_handle (), context) &&
      newTestEvent.is_of_type (anotherCaracal) &&
      newTestEvent.is_of_exact_type (anotherCaracal));

   test.validate (
      "test get_name",
      newTestEvent.get_name () == CaracalName);

   test.validate (
      "test get_handle",
      (newTestEvent.get_handle () == anotherCaracal.get_handle ()));

   // get / become parent
   EventType newKitty (CaracalName, context);
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
//   test.log.out << "M: <" << data << ">" << endl;
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

   RuntimeIterator oIt;
   EventType otype;
   test.validate (
      "test iterator get_first_child ()",
      anotherCat.get_first_child (oIt, otype) &&
      otype == anotherCaracal);

   EventType aServal("serval", context);
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
