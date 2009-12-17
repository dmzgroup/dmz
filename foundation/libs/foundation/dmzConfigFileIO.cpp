#include <dmzConfigFileIO.h>
#include <dmzJSONInterpreterConfig.h>
#include <dmzJSONParser.h>
#include <dmzJSONUtil.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeLog.h>
#include <dmzStreamZip.h>
#include <dmzSystem.h>
#include <dmzSystemFile.h>
#include <dmzSystemStreamFile.h>
#include <dmzXMLInterpreterConfig.h>
#include <dmzXMLParser.h>
#include <dmzXMLUtil.h>
#include <dmzZip.h>

//const UInt32 AutoDetectFileType = 0;
//const UInt32 XMLFileType = 1;
//const UInt32 JSONFileType = 2;

using namespace dmz;

namespace {

static const UInt32 MinType = XMLFileType;
static const UInt32 MaxType = JSONFileType;
static const Int32 BufferSize = 2048;

static UInt32
local_file_type (const UInt32 Type, const String &FileName, Log *log) {

   UInt32 result ((Type >= MinType) && (Type <=MaxType) ? Type : 0);

   if (!result) {

      String path, file, ext;
      split_path_file_ext (FileName, path, file, ext);

      const String Type = ext.get_lower ();

      // NOTE: If a new extension is added, be sure to increase MaxType defined above
      //       So that the new explicit type will work.
      if (Type == ".xml") { result = XMLFileType; }
      else if (Type == ".json") { result = JSONFileType; }
      else if (log) {

         log->warn << "Unknown extension type: \"" << ext
            << "\". Defaulting to XML file type." << endl;

         result = XMLFileType;
      }
   }

   return result;
}

};

dmz::Boolean
dmz::read_config_file (
      const String &SourceName,
      const String &FileName,
      Config &data,
      const UInt32 Type,
      Log *log) {

   const Boolean IsZip = is_zip_file (SourceName);

   PathContainer files;
   files.add_path (IsZip ? FileName : SourceName);

   const String NullString;

   return read_config_files (IsZip ? SourceName : NullString, files, data, Type, log);
}


dmz::Boolean
dmz::read_config_files (
      const String &SourceName,
      const PathContainer &Files,
      Config &data,
      const UInt32 Type,
      Log *log) {

   Boolean result (True);

   const Float64 StartTime = get_time ();

   if (!data) {

      Config tmp ("global");
      data = tmp;
   }

   PathContainerIterator it;
   String file;

   if (SourceName) {

      ZipReader zr;

      if (!zr.open_zip_file (SourceName)) {

         if (log) {

            log->error << "Failed opening zip archive: " << SourceName << endl;
         }

         result = False;
      }

      char buffer[BufferSize];

      while (Files.get_next (it, file) && result) {

         const UInt32 RType = local_file_type (Type, file, log);

         if (zr.open_file (file)) {

            if (RType == XMLFileType) {

               XMLParser parser;
               XMLInterpreterConfig interpreter (data);
               parser.set_interpreter (&interpreter);
               Boolean done = False;

               while (!done) {

                  const Int32 Size = zr.read_file (buffer, BufferSize);

                  if (!parser.parse_buffer (buffer, Size, Size < BufferSize)) {

                     done = True;
                     result = False;

                     if (log) {

                        log->error << "In file: " << file << " in zip archive: "
                           << SourceName << " : " << parser.get_error () << endl;
                     }
                  }
                  else if (Size < BufferSize) { done = True; }
               }
            }
            else if (RType == JSONFileType) {

               JSONParser parser;
               JSONInterpreterConfig interpreter (data);
               parser.set_interpreter (&interpreter);
               Boolean done = False;

               while (!done) {

                  const Int32 Size = zr.read_file (buffer, BufferSize);

                  if (!parser.parse_buffer (buffer, Size, Size < BufferSize)) {

                     done = True;
                     result = False;

                     if (log) {

                        log->error << "In file: " << file << " in zip archive: "
                           << SourceName << " : " << parser.get_error () << endl;
                     }
                  }
                  else if (Size < BufferSize) { done = True; }
               }

            }

            zr.close_file ();
         }
         else {

            if (log) {

               log->error << "Failed opening file: " << file << " in zip archive: "
                  << SourceName << endl;
            }

            result = False;
         }
      }

      zr.close_zip_file ();
   }
   else {

      while (Files.get_next (it, file) && result) {

         const UInt32 RType = local_file_type (Type, file, log);

         if (RType == XMLFileType) {

            result = xml_to_config (file, data, log);
         }
         else if (RType == JSONFileType) {

            result = json_to_config (file, data, log);
         }
      }
   }

   if (log) { log->info << "Parse Time: " << get_time () - StartTime << endl; }

   return result;
}


dmz::Boolean
dmz::write_config_file (
      const String &SourceName,
      const String &FileName,
      const Config &Data,
      const UInt32 Type,
      Log *log) {

   Boolean result (False);

   const UInt32 RType = local_file_type (Type, FileName, log);

   Stream *out (0);
   FILE *fd (0);

   if (SourceName) {

      StreamZip *sz = new StreamZip;

      if (sz) {

         if (sz->open_zip_file (SourceName)) {

            sz->open_file (FileName);
            out = sz;
         }
         else {

            delete sz; sz = 0;

            if (log) {

               log->error << "Failed creating zip archive: " << SourceName
                  << " while attempting to store file: " << FileName << endl;
            }
         }
      }
   }
   else {

      fd = open_file (FileName, "wb");

      if (fd) {

         out = new StreamFile (fd);
      }
      else if (log) { log->error << "Failed creating file: " << FileName << endl; }
   }

   if (out) {

      if (RType == XMLFileType) {

         write_xml_header (*out);
         format_config_to_xml (Data, *out, XMLPrettyPrint);
         result = True;
      }
      else if (RType == JSONFileType) {

         result = format_config_to_json (Data, *out, JSONPrettyPrint);
      }

      delete out; out = 0;
   }

   if (fd) { close_file (fd); fd = 0; }

   return result;
}

