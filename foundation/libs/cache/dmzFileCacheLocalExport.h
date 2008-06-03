#ifndef DMZ_FILE_CACHE_LOCAL_EXPORT_DOT_H
#define DMZ_FILE_CACHE_LOCAL_EXPORT_DOT_H

#ifdef _WIN32
#   ifdef DMZ_FILE_CACHE_LOCAL_EXPORT
#      define DMZ_FILE_CACHE_LOCAL_LINK_SYMBOL __declspec (dllexport)
#   else
#      define DMZ_FILE_CACHE_LOCAL_LINK_SYMBOL __declspec (dllimport)
#   endif
#else
#   define DMZ_FILE_CACHE_LOCAL_LINK_SYMBOL
#endif

#endif // DMZ_FILE_CACHE_LOCAL_EXPORT_DOT_H
