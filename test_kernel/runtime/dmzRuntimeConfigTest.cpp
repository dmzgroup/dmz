#include <dmzRuntime.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeLogObserverBasic.h>
#include <dmzXMLUtil.h>

int
main (int argc, char *argv[]) {

   dmz::Runtime rt;
   dmz::Log log ("test", rt.get_context ());
   dmz::LogObserverBasic obs (rt.get_context ());

   dmz::Config root ("dmz");

   root.store_attribute ("version", "1.0");

   for (dmz::Int32 ix = 0; ix < 10; ix++) {

      dmz::Config element ("element");
      dmz::String value ("value");
      element.append_value (value << ix, dmz::False);
      element.store_attribute ("myatt", value.flush () << ix);
      root.add_config ("foo.bar", element);
      dmz::Config tmp ("foo");
      tmp.add_config ("bar", element);
      root.add_config (tmp);
   }

   dmz::format_config_to_xml (root, log.out);

   log.out << dmz::endl;


   dmz::Config elData;

   if (root.lookup_all_config ("foo.bar.element", elData)) {

      dmz::format_config_to_xml (elData, log.out);
   }
   else { log.error << "Unable to find elements" << dmz::endl; }

   dmz::Config foo ("count_test");

   for (dmz::Int32 jy = 0; jy < 40; jy++) {

      dmz::Config count ("count");
      dmz::String value;
      count.append_value (value << jy, dmz::False);
      if (!foo.add_config (count)) {
         log.error << "Unable to add: " << jy << dmz::endl;
      }
   }

   dmz::format_config_to_xml (foo, log.out);

   return 0;
}
