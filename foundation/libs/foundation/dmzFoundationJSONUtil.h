#ifndef DMZ_FOUNDATION_JSON_UTIL_DOT_H
#define DMZ_FOUNDATION_JSON_UTIL_DOT_H

#include <dmzFoundationConsts.h>
#include <dmzFoundationExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class Config;
   class Log;
   class Stream;
   class Version;

   DMZ_FOUNDATION_LINK_SYMBOL Boolean json_is_number (const String &Value);

   DMZ_FOUNDATION_LINK_SYMBOL Boolean json_string_to_config (
      const String &Value,
      Config &data,
      Log *log = 0);

   DMZ_FOUNDATION_LINK_SYMBOL Boolean json_to_config (
      const String &FileName,
      Config &data,
      Log *log = 0);

   DMZ_FOUNDATION_LINK_SYMBOL Boolean json_to_version (
      const String &FileName,
      Version &value,
      Log *log = 0);

   DMZ_FOUNDATION_LINK_SYMBOL Boolean format_config_to_json (
      const Config &Config,
      Stream &stream,
      const UInt32 Mode = 0,
      Log *log = 0);

};

#endif // DMZ_FOUNDATION_JSON_UTIL_DOT_H
