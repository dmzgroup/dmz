#ifndef DMZ_OBJECT_UTIL_EXPORT_DOT_H
#define DMZ_OBJECT_UTIL_EXPORT_DOT_H

#ifdef _WIN32
#   ifdef DMZ_OBJECT_UTIL_EXPORT
#      define DMZ_OBJECT_UTIL_LINK_SYMBOL __declspec (dllexport)
#   else
#      define DMZ_OBJECT_UTIL_LINK_SYMBOL __declspec (dllimport)
#   endif
#else
#   define DMZ_OBJECT_UTIL_LINK_SYMBOL
#endif

#endif // DMZ_OBJECT_UTIL_EXPORT_DOT_H
