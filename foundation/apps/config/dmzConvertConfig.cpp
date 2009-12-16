#include <dmzCommandLine.h>
#include <dmzJSONUtil.h>
#include <dmzRuntime.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeLogObserverBasic.h>
#include <dmzSystem.h>
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

   Boolean convert (True);
   Boolean doTime (False);

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
         log.out << "\t-c <true|false> Convert." << endl;
         log.out << "\t-f <file list>  List of files to convert." << endl;
         log.out << "\t-o <path>       Output directory." << endl;
         log.out << "\t-p <true|false> Pretty print output." << endl;
         log.out << "\t-t <true|false> Time file parse and write." << endl;
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
      else if (Name == "c") {

         String value;

         if (arg.get_first_arg (value)) {

            if (value == "false") { convert = False; }
            else { convert = True; }
         }
         else { convert = True; }
      }
      else if (Name == "t") {

         String value;

         if (arg.get_first_arg (value)) {

            if (value == "false") { doTime = False; }
            else { doTime = True; }
         }
         else { doTime = True; }
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

            Boolean toXML (True);

            if (ext == ".json") { toXML = True; }
            else { toXML = False; }

            if (!convert) { toXML = !toXML; }

            if (!convert && !targetPath) { file << ".convert"; }

            Boolean valid (False);

            Float64 StartTime (doTime ? get_time () : 0.0);

            if (ext == ".json") { valid = json_to_config (fname, data, &log); }
            else { valid = xml_to_config (fname, data, &log); }

            if (valid && doTime) {

               const Float64 Delta = get_time () - StartTime;

               log.out << fname << " parsed: " << Delta << " sec." << endl;
            }

            if (valid) {

               String ftype (toXML ? ".xml" : ".json");

               FILE *fp = open_file (path + file + ftype, "wb");

               if (fp) {

                  const Float64 StartTime (doTime ? get_time () : 0.0);

                  StreamFile out (fp);

                  if (toXML) {

                     write_xml_header (out);
                     format_config_to_xml (data, out, XMLStripGlobal | xmlMask, &log);
                  }
                  else {

                     format_config_to_json (data, out, JSONStripGlobal | jsonMask, &log);
                  }

                  close_file (fp); fp = 0;

                  if (doTime) {

                     const Float64 Delta = get_time () - StartTime;

                     log.out << path << file << ftype  << " written: " << Delta
                        << " sec." << endl;
                  }
               }
               else {

                  log.error << "Failed to create file: " << path << file << ftype
                     << endl;
               }
            }
         }
      }
      else { log.error << "Unknown option: " << Name << endl; exit (-1); }
   }

   return 0;
}
