#ifndef DMZ_XML_UTIL_DOT_H
#define DMZ_XML_UTIL_DOT_H

#include <dmzTypesBase.h>
#include <dmzXMLExport.h>

namespace dmz {

   class Config;
   class Log;
   class Stream;
   class Version;

   DMZ_XML_LINK_SYMBOL Boolean xml_string_to_config (
      const String &Value,
      Config &data,
      Log *log = 0);

   DMZ_XML_LINK_SYMBOL Boolean xml_to_config (
      const String &File,
      Config &data,
      Log *log = 0);

   DMZ_XML_LINK_SYMBOL Boolean xml_to_version (
      const String &File,
      Version &value,
      Log *log = 0);

   DMZ_XML_LINK_SYMBOL void write_xml_header (Stream &stream);

   DMZ_XML_LINK_SYMBOL void format_config_to_xml (
      const Config &Config,
      Stream &stream,
      const Boolean StripGlobal = False);

};

#endif // DMZ_XML_UTIL_DOT_H
