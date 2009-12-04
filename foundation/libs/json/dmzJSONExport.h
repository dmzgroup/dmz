#ifndef DMZ_JSON_EXPORT_DOT_H
#define DMZ_JSON_EXPORT_DOT_H

#ifdef _WIN32
#   ifdef DMZ_JSON_EXPORT
#      define DMZ_JSON_LINK_SYMBOL __declspec (dllexport)
#   else
#      define DMZ_JSON_LINK_SYMBOL __declspec (dllimport)
#   endif
#else
#   define DMZ_JSON_LINK_SYMBOL
#endif

#endif // DMZ_JSON_EXPORT_DOT_H
