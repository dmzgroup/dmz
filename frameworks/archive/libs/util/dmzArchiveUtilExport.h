#ifndef DMZ_ARCHIVE_UTIL_EXPORT_DOT_H
#define DMZ_ARCHIVE_UTIL_EXPORT_DOT_H

#ifdef _WIN32
#   ifdef DMZ_ARCHIVE_UTIL_EXPORT
#      define DMZ_ARCHIVE_UTIL_LINK_SYMBOL __declspec (dllexport)
#   else
#      define DMZ_ARCHIVE_UTIL_LINK_SYMBOL __declspec (dllimport)
#   endif
#else
#   define DMZ_ARCHIVE_UTIL_LINK_SYMBOL
#endif

#endif // DMZ_ARCHIVE_UTIL_EXPORT_DOT_H
