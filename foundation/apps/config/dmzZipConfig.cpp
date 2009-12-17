#include <dmzCommandLine.h>
#include <dmzRuntime.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeLogObserverBasic.h>
#include <dmzStreamZip.h>
#include <dmzSystemFile.h>
#include <dmzXMLUtil.h>

#include <stdlib.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   Runtime rt;
   LogObserverBasic obs (rt.get_context ());
   Log log ("", rt.get_context ());
   CommandLine cl (argc, argv);

   CommandLineArgs arg;

   String zipFile ("out.zip");
   UInt32 xmlMask = XMLPrettyPrint;

   for (
         Boolean found = cl.get_first_option (arg);
         found;
         found = cl.get_next_option (arg)) {

      const String Name = arg.get_name ();

      if ((Name == "h") || (Name == "-help")) {

         String path, file, ext;
         split_path_file_ext (argv[0], path, file, ext);
         
         log.out << file << " help:" << endl;
         log.out << "\t-f <file list>  List of files to store in a zip file." << endl;
         log.out << "\t-z <file name>  Name of zip file." << endl;
         log.out << "\t-h or --help    This help list." << endl;

         exit (0);
      }
      else if (Name == "z") { arg.get_first_arg (zipFile); }
   }

   StreamZip out;
   out.open_zip_file (zipFile);

   for (
         Boolean found = cl.get_first_option (arg);
         found;
         found = cl.get_next_option (arg)) {

      const String Name = arg.get_name ();

      if (Name == "f") {

         String fname;

         for (
               Boolean found = arg.get_first_arg (fname);
               found;
               found = arg.get_next_arg (fname)) {

            Config data ("global");

            if (xml_to_config (fname, data, &log)) {

               if (out.open_file (fname)) {

                  write_xml_header (out);
                  format_config_to_xml (data, out, XMLStripGlobal | xmlMask, &log);
                  out.close_file ();
               }
            }
         }
      }
      else { log.error << "Unknown option: " << Name << endl; exit (-1); }
   }

   out.close_zip_file ();

   return 0;
}
