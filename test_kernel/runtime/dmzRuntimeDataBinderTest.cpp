#include <dmzRuntimeInit.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeDataBinder.h>
#include <dmzSystem.h>
#include <dmzTest.h>

#include <dmzTypesMask.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesUUID.h>
#include <dmzTypesVector.h>

using namespace dmz;


int
main (int argc, char *argv[]) {

   Test test ("dmzRuntimeDataBinderTest", argc, argv);
   RuntimeContext *context (test.rt.get_context ());

   Int32 value (12345);
   Int32 array[3] = { 1, 2, 3 };
   DataBinder binder (test.rt.get_context (), &(test.log));
   binder.bind ("value", 0, value);
   binder.bind ("array", 0, array[0]);
   binder.bind ("array", 1, array[1]);
   binder.bind ("array", 2, array[2]);

   Data data (test.rt.get_context ());
   test.validate ("Binder returned true on write", binder.write_data (data));

   test.log.out << data << endl;

   value = 654321l;
   binder.write_data (data);
   test.log.out << data << endl;

   return test.result ();
}
