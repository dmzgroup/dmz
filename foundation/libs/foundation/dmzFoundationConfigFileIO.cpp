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

/*!

\var dmz::FileTypeAutoDetect
\ingroup Foundation
\brief Specifies that the Config file type should be auto detected.
\details Defined in dmzFoundatioConfigFileIO.h
\sa dmz::read_config_file \n dmz::read_config_files

*/

/*!

\var dmz::FileTypeXML
\ingroup Foundation
\brief Specifies that the Config file type should be XML.
\details Defined in dmzFoundatioConfigFileIO.h
\sa dmz::read_config_file \n dmz::read_config_files

*/

/*!

\var dmz::FileTypeJSON
\ingroup Foundation
\brief Specifies that the Config file type should be JSON.
\details Defined in dmzFoundatioConfigFileIO.h
\sa dmz::read_config_file \n dmz::read_config_files

*/

using namespace dmz;

namespace {

static const UInt32 MinType = FileTypeXML;
static const UInt32 MaxType = FileTypeJSON;
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
      if (Type == ".xml") { result = FileTypeXML; }
      else if (Type == ".json") { result = FileTypeJSON; }
      else if (log) {

         log->warn << "Unknown extension type: \"" << ext
            << "\". Defaulting to XML file type." << endl;

         result = FileTypeXML;
      }
   }

   return result;
}


static Boolean
local_read_file (const String &ArchiveName, Reader &reader, Parser &parser, Log *log) {

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
            if (ArchiveName) { log->error << " in zip archive: " << ArchiveName; }
            log->error << " : " << parser.get_error () << endl;
         }
      }
      else if (Size < BufferSize) { done = True; }
   }

   return result;
}

};


/*!

\brief Reads a Config file.
\ingroup Foundation
\details Parsers Config files and stores the result in the Config object data. Config
files may be XML or JSON. If the file is a zip archive containing multiple Config files.
All Config files contained in the zip archive are read. An archive may contain both XML
and JSON Config files.
\param[in] FileName String containing the name of the file to read.
\param[out] data Config object used to store parsed Config data.
\param[in] Type File type.
\param[in] log Pointer to the Log to use for reporting.
\return Returns dmz::True if the file was parsed without errors.
\sa dmz::FileTypeAutoDetect \n dmz::FileTypeXML \n dmz::FileTypeJSON

*/
dmz::Boolean
dmz::read_config_file (
      const String &FileName,
      Config &data,
      const UInt32 Type,
      Log *log) {

   const Boolean IsZip = is_zip_file (FileName);

   StringContainer files;

   if (IsZip) {

      ReaderZip rz;

      if (rz.open_zip_file (FileName)) { rz.get_file_list (files); }
   }
   else { files.append (FileName); }

   const String NullString;

   return read_config_files (IsZip ? FileName : NullString, files, data, Type, log);
}


/*!

\brief Reads a Config file.
\ingroup Foundation
\details Reads a single Config file. If an archive is specified, the file will attempted
to be read from the archive. If no archive name is given. The file will be read from
the file system.
\param[in] ArchiveName String containing name of the archive to read from. Optional.
\param[in] FileName String containing the name of the Config file to read.
\param[out] data Config object used to store parsed Config data.
\param[in] Type File type.
\param[in] log Pointer to the Log to use for reporting.
\return Returns dmz::True if the file was parsed without errors.
\sa dmz::FileTypeAutoDetect \n dmz::FileTypeXML \n dmz::FileTypeJSON

*/
dmz::Boolean
dmz::read_config_file (
      const String &ArchiveName,
      const String &FileName,
      Config &data,
      const UInt32 Type,
      Log *log) {

   StringContainer files;
   files.append (FileName);

   return read_config_files (ArchiveName, files, data, Type, log);
}


/*!

\brief Reads a list of Config files.
\ingroup Foundation
\details Reads a list of Config files. If an archive is specified, the file will attempted
to be read from the archive. If no archive name is given. The file will be read from
the file system.
\param[in] ArchiveName String containing name of the archive to read from. Optional.
\param[in] Files StringContainer containing the list of Config files to read.
\param[out] data Config object used to store parsed Config data.
\param[in] Type File type.
\param[in] log Pointer to the Log to use for reporting.
\return Returns dmz::True if the files were parsed without errors.
\sa dmz::FileTypeAutoDetect \n dmz::FileTypeXML \n dmz::FileTypeJSON

*/
dmz::Boolean
dmz::read_config_files (
      const String &ArchiveName,
      const StringContainer &Files,
      Config &data,
      const UInt32 Type,
      Log *log) {

   Boolean result (True);

   if (!data) {

      Config tmp ("global");
      data = tmp;
   }

   StringContainerIterator it;
   String file;

   Reader *reader (0);

   if (ArchiveName) {

      ReaderZip *zr = new ReaderZip;

      if (zr && zr->open_zip_file (ArchiveName)) { reader = zr; }
      else {

         if (log) {

            log->error << "Failed opening zip archive: " << ArchiveName << endl;
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

            if (RType == FileTypeXML) {

               ParserXML parser;
               InterpreterXMLConfig interpreter (data);
               parser.set_interpreter (&interpreter);

               result = local_read_file (ArchiveName, *reader, parser, log);
            }
            else if (RType == FileTypeJSON) {

               ParserJSON parser;
               InterpreterJSONConfig interpreter (data);
               parser.set_interpreter (&interpreter);

               result = local_read_file (ArchiveName, *reader, parser, log);
            }

            if (result && log) {

               log->info << "Parsed file: " << file;
               if (ArchiveName) { log->info << " from archive: " << ArchiveName; }
               log->info << " (" << get_time () - StartTime << "sec)" << endl;
            }
         }
         else {

            if (log) {

               log->error << "Failed opening file: " << file;
               if (ArchiveName) { log->error << " in zip archive: " << ArchiveName; }
               log->error  << endl;
            }

            result = False;
         }
      }
   }

   if (reader) { delete reader; reader = 0; }

   return result;
}


/*!

\brief Write Config object to a file.
\ingroup Foundation
\details If an archive name is specified, the file is stored in a zip archive.
\param[in] ArchiveName String containing name of archive to store file. Optional.
\param[in] FileName String containing the name of the file to store the Config data.
\param[in] Data Config object to store in a file.
\param[in] Mode Specifies how the Config object should be written out.
\param[in] Type File type.
\param[in] log Pointer to the Log to use for reporting.
\return Returns dmz::True if the file was written without errors.
\sa dmz::ConfigPrettyPrint \n dmz::ConfigStripGlobal \n dmz::FileTypeAutoDetect \n dmz::FileTypeXML \n dmz::FileTypeJSON 
*/
dmz::Boolean
dmz::write_config_file (
      const String &ArchiveName,
      const String &FileName,
      const Config &Data,
      const UInt32 Mode,
      const UInt32 Type,
      Log *log) {

   Boolean result (False);

   const UInt32 RType = local_file_type (Type, FileName, log);

   Stream *out (0);
   FILE *fd (0);

   if (ArchiveName) {

      StreamZip *sz = new StreamZip;

      if (sz) {

         if (sz->open_zip_file (ArchiveName)) {

            sz->open_file (FileName);
            out = sz;
         }
         else {

            delete sz; sz = 0;

            if (log) {

               log->error << "Failed creating zip archive: " << ArchiveName
                  << " while attempting to store file: " << FileName << endl;
            }
         }
      }
   }
   else {

      fd = open_file (FileName, "wb");

      if (fd) { out = new StreamFile (fd); }
      else if (log) { log->error << "Failed creating file: " << FileName << endl; }
   }

   if (out) {

      if (RType == FileTypeXML) {

         result = format_config_to_xml (Data, *out, Mode);
      }
      else if (RType == FileTypeJSON) {

         result = format_config_to_json (Data, *out, Mode);
      }

      delete out; out = 0;
   }

   if (fd) { close_file (fd); fd = 0; }

   return result;
}

