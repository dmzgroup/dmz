#include <dmzJSONUtil.h>
#include <dmzRuntime.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeLogObserverBasic.h>
#include <dmzSystemFile.h>
#include <dmzSystemStreamFile.h>
#include <dmzXMLUtil.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   dmz::Runtime rt;
   dmz::LogObserverBasic obs (rt.get_context ());
   dmz::Log log ("", rt.get_context ());

   for (int ix = 1; ix < argc; ix++) {

      String path, file, ext;
      split_path_file_ext (argv[ix], path, file, ext);
      Config data ("global");

      if (ext == ".json") {

         if (json_to_config (argv[ix], data, &log)) {

            FILE *fp = open_file (path + file + ".xml", "wb");

            if (fp) {

               StreamFile out (fp);

               dmz::write_xml_header (out);
               dmz::format_config_to_xml (data, out, XMLStripGlobal | XMLPrettyPrint, &log);
               close_file (fp); fp = 0;
            }
         }
      }
      else {

         if (xml_to_config (argv[ix], data, &log)) {

            FILE *fp = open_file (path + file + ".json", "wb");

            if (fp) {

               StreamFile out (fp);

               dmz::format_config_to_json (data, out, JSONPrettyPrint | JSONStripGlobal, &log);
               close_file (fp); fp = 0;
            }
         }
      }
   }

   return 0;
}
