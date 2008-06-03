#ifndef DMZ_XML_EXPORT_DOT_H
#define DMZ_XML_EXPORT_DOT_H

#ifdef _WIN32
#   ifdef DMZ_XML_EXPORT
#      define DMZ_XML_LINK_SYMBOL __declspec (dllexport)
#   else
#      define DMZ_XML_LINK_SYMBOL __declspec (dllimport)
#   endif
#else
#   define DMZ_XML_LINK_SYMBOL
#endif

#endif // DMZ_XML_EXPORT_DOT_H
