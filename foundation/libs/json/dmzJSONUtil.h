#ifndef DMZ_JSON_UTIL_DOT_H
#define DMZ_JSON_UTIL_DOT_H

#include <dmzTypesBase.h>
#include <dmzJSONExport.h>

namespace dmz {

   class Config;
   class Log;
   class Stream;
   class Version;

   const UInt32 JSONStripGlobal = 0x01;
   const UInt32 JSONPrettyPrint = 0x02;

   DMZ_JSON_LINK_SYMBOL Boolean json_is_number (const String &Value);

   DMZ_JSON_LINK_SYMBOL Boolean json_string_to_config (
      const String &Value,
      Config &data,
      Log *log = 0);

   DMZ_JSON_LINK_SYMBOL Boolean json_to_config (
      const String &FileName,
      Config &data,
      Log *log = 0);

   DMZ_JSON_LINK_SYMBOL Boolean json_to_version (
      const String &FileName,
      Version &value,
      Log *log = 0);

   DMZ_JSON_LINK_SYMBOL Boolean format_config_to_json (
      const Config &Config,
      Stream &stream,
      const UInt32 Mode = 0,
      Log *log = 0);

};

#endif // DMZ_JSON_UTIL_DOT_H
