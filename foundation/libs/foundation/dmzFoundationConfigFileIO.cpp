#include <dmzFoundationConfigFileIO.h>
#include <dmzFoundationInterpreterJSONConfig.h>
#include <dmzFoundationInterpreterXMLConfig.h>
#include <dmzFoundationJSONUtil.h>
#include <dmzFoundationParserJSON.h>
#include <dmzFoundationParserXML.h>
#include <dmzFoundationStreamZip.h>
#include <dmzFoundationXMLUtil.h>
#include <dmzFoundationReaderWriterFile.h>
#include <dmzFoundationReaderWriterZip.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeLog.h>
#include <dmzSystem.h>
#include <dmzSystemFile.h>
#include <dmzSystemStreamFile.h>

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


static Boolean
local_read_file (const String &SourceName, Reader &reader, Parser &parser, Log *log) {

   Boolean result (True);

   char buffer[BufferSize];

   Boolean done = False;

   while (!done) {

      const Int32 Size = reader.read_file (buffer, BufferSize);

      if (!parser.parse_buffer (buffer, Size, Size < BufferSize)) {

         done = True;
         result = False;

         if (log) {

            log->error << "In file: " << reader.get_file_name ();
            if (SourceName) { log->error << " in zip archive: " << SourceName; }
            log->error << " : " << parser.get_error () << endl;
         }
      }
      else if (Size < BufferSize) { done = True; }
   }

   return result;
}

};


dmz::Boolean
dmz::read_config_file (
      const String &FileName,
      Config &data,
      const UInt32 Type,
      Log *log) {

   const Boolean IsZip = is_zip_file (FileName);

   PathContainer files;

   if (IsZip) {

      ReaderZip rz;

      if (rz.open_zip_file (FileName)) { rz.get_file_list (files); }
   }
   else { files.add_path (FileName); }

   const String NullString;

   return read_config_files (IsZip ? FileName : NullString, files, data, Type, log);
}


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

   if (!data) {

      Config tmp ("global");
      data = tmp;
   }

   PathContainerIterator it;
   String file;

   Reader *reader (0);

   if (SourceName) {

      ReaderZip *zr = new ReaderZip;

      if (zr && zr->open_zip_file (SourceName)) { reader = zr; }
      else {

         if (log) {

            log->error << "Failed opening zip archive: " << SourceName << endl;
         }

         result = False;
      }
   }
   else { reader = new ReaderFile; }

   if (reader) {

      while (Files.get_next (it, file) && result) {

         const UInt32 RType = local_file_type (Type, file, log);

         const Float64 StartTime = get_time ();

         if (reader->open_file (file)) {

            if (RType == XMLFileType) {

               ParserXML parser;
               InterpreterXMLConfig interpreter (data);
               parser.set_interpreter (&interpreter);

               result = local_read_file (SourceName, *reader, parser, log);
            }
            else if (RType == JSONFileType) {

               ParserJSON parser;
               InterpreterJSONConfig interpreter (data);
               parser.set_interpreter (&interpreter);

               result = local_read_file (SourceName, *reader, parser, log);
            }

            if (result && log) {

               log->info << "Parsed file: " << file;
               if (SourceName) { log->info << " from archive: " << SourceName; }
               log->info << " (" << get_time () - StartTime << "sec)" << endl;
            }
         }
         else {

            if (log) {

               log->error << "Failed opening file: " << file;

               if (SourceName) { log->error << " in zip archive: " << SourceName; }

               log->error  << endl;
            }

            result = False;
         }
      }
   }

   if (reader) { delete reader; reader = 0; }

   return result;
}


dmz::Boolean
dmz::write_config_file (
      const String &SourceName,
      const String &FileName,
      const Config &Data,
      const UInt32 Mode,
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

         result = format_config_to_xml (Data, *out, Mode);
      }
      else if (RType == JSONFileType) {

         result = format_config_to_json (Data, *out, Mode);
      }

      delete out; out = 0;
   }

   if (fd) { close_file (fd); fd = 0; }

   return result;
}

