#ifndef DMZ_FOUNDATION_CONFIG_FILE_IO_DOT_H
#define DMZ_FOUNDATION_CONFIG_FILE_IO_DOT_H

#include <dmzFoundationExport.h>
#include <dmzTypesString.h>

namespace dmz {

class Config;
class Log;
class StringContainer;

const UInt32 FileTypeAutoDetect = 0;
const UInt32 FileTypeXML = 1;
const UInt32 FileTypeJSON = 2;

DMZ_FOUNDATION_LINK_SYMBOL Boolean
read_config_file (
   const String &FileName,
   Config &data,
   const UInt32 Type = FileTypeAutoDetect,
   Log *log = 0);

DMZ_FOUNDATION_LINK_SYMBOL Boolean
read_config_file (
   const String &ArchiveName,
   const String &FileName,
   Config &data,
   const UInt32 Type = FileTypeAutoDetect,
   Log *log = 0);

DMZ_FOUNDATION_LINK_SYMBOL Boolean
read_config_files (
   const String &ArchiveName,
   const StringContainer &Files,
   Config &data,
   const UInt32 Type = FileTypeAutoDetect,
   Log *log = 0);

DMZ_FOUNDATION_LINK_SYMBOL Boolean
write_config_file (
   const String &ArchiveName,
   const String &FileName,
   const Config &Data,
   const UInt32 Mode = 0,
   const UInt32 Type = FileTypeAutoDetect,
   Log *log = 0);

};

#endif // DMZ_FOUNDATION_CONFIG_FILE_IO_DOT_H
