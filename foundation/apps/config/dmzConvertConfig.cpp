#include <dmzCommandLine.h>
#include <dmzJSONUtil.h>
#include <dmzRuntime.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeLogObserverBasic.h>
#include <dmzSystemFile.h>
#include <dmzSystemStreamFile.h>
#include <dmzXMLUtil.h>

#include <stdlib.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   Runtime rt;
   LogObserverBasic obs (rt.get_context ());
   Log log ("", rt.get_context ());
   CommandLine cl (argc, argv);

   String targetPath;

   UInt32 jsonMask = 0;
   UInt32 xmlMask = 0;

   CommandLineArgs arg;

   for (
         Boolean found = cl.get_first_option (arg);
         found;
         found = cl.get_next_option (arg)) {

      const String Name = arg.get_name ();

      if ((Name == "h") || (Name == "-help")) {

         String path, file, ext;
         split_path_file_ext (argv[0], path, file, ext);
         
         log.out << file << " help:" << endl;
         log.out << "\t-p <true|false> Pretty print output." << endl;
         log.out << "\t-o <path>       Output directory." << endl;
         log.out << "\t-f <file list>  List of files to convert." << endl;
         log.out << "\t-h or --help    This help list." << endl;

         exit (0);
      }
   }

   for (
         Boolean found = cl.get_first_option (arg);
         found;
         found = cl.get_next_option (arg)) {

      const String Name = arg.get_name ();

      if (Name == "p") {

         jsonMask |= JSONPrettyPrint;
         xmlMask |= XMLPrettyPrint;

         String value;

         if (arg.get_first_arg (value)) {

            if (value == "false") {

               jsonMask &= ~JSONPrettyPrint;
               xmlMask &= ~XMLPrettyPrint;
            }
         }
      }
      else if (Name == "o") {

         arg.get_first_arg (targetPath);
      }
      else if (Name == "f") {

         String fname;
         for (
               Boolean found = arg.get_first_arg (fname);
               found;
               found = arg.get_next_arg (fname)) {

            String path, file, ext;
            split_path_file_ext (fname, path, file, ext);
            if (targetPath) { path = targetPath + "/"; }
            Config data ("global");

            if (ext == ".json") {

               if (json_to_config (fname, data, &log)) {

                  FILE *fp = open_file (path + file + ".xml", "wb");

                  if (fp) {

                     StreamFile out (fp);

                     write_xml_header (out);
                     format_config_to_xml (data, out, XMLStripGlobal | xmlMask, &log);
                     close_file (fp); fp = 0;
                  }
                  else {

                     log.error << "Failed to create file: " << path << file << ".xml"
                        << endl;
                  }
               }
            }
            else {

               if (xml_to_config (fname, data, &log)) {

                  FILE *fp = open_file (path + file + ".json", "wb");

                  if (fp) {

                     StreamFile out (fp);

                     format_config_to_json (data, out, JSONStripGlobal | jsonMask, &log);
                     close_file (fp); fp = 0;
                  }
                  else {

                     log.error << "Failed to create file: " << path << file << ".json"
                        << endl;
                  }
               }
            }
         }
      }
      else { log.error << "Unknown option: " << Name << endl; exit (-1); }
   }

   return 0;
}
