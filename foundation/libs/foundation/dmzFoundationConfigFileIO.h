#ifndef DMZ_FOUNDATION_CONFIG_FILE_IO_DOT_H
#define DMZ_FOUNDATION_CONFIG_FILE_IO_DOT_H

#include <dmzFoundationExport.h>
#include <dmzTypesString.h>

namespace dmz {

class Config;
class Log;
class PathContainer;

const UInt32 AutoDetectFileType = 0;
const UInt32 XMLFileType = 1;
const UInt32 JSONFileType = 2;

DMZ_FOUNDATION_LINK_SYMBOL Boolean
read_config_file (
   const String &SourceName,
   const String &FileName,
   Config &data,
   const UInt32 Type = AutoDetectFileType,
   Log *log = 0);

DMZ_FOUNDATION_LINK_SYMBOL Boolean
read_config_files (
   const String &SourceName,
   const PathContainer &Files,
   Config &data,
   const UInt32 Type = AutoDetectFileType,
   Log *log = 0);

DMZ_FOUNDATION_LINK_SYMBOL Boolean
write_config_file (
   const String &SourceName,
   const String &FileName,
   const Config &Data,
   const UInt32 Type = AutoDetectFileType,
   Log *log = 0);

};

#endif // DMZ_FOUNDATION_CONFIG_FILE_IO_DOT_H
