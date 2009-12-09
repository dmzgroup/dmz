#include <dmzRuntime.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeLogObserverBasic.h>
#include <dmzSystemFile.h>
#include <dmzJSONParser.h>
#include <dmzJSONInterpreterConfig.h>

#include <stdio.h>

int
main (int argc, char *argv[]) {

   dmz::Runtime rt;
   dmz::LogObserverBasic obs (rt.get_context ());

   if (argc == 2) {

      FILE *fd = dmz::open_file (argv[1], "rb");

      if (fd) {

         dmz::Config gdata ("global");
         dmz::JSONParser parser;
         dmz::JSONInterpreterConfig inter (gdata);
         parser.set_interpreter (&inter);

         char buffer[1056];
         dmz::Int32 size (0);

         do {

            size = fread (buffer, sizeof (char), 1056, fd);

            if (size && !parser.parse_buffer (buffer, size, size < 1056)) {

               size = 0;

               fprintf (stderr, "Error: %s\n", parser.get_error ().get_buffer ());
            }

         } while (size);

         dmz::close_file (fd);

         dmz::Log log ("", rt.get_context ());
         log.info << gdata << dmz::endl;
      }
   }

   return 0;
}
