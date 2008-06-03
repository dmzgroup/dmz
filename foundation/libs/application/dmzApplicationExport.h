#ifndef DMZ_APPLICATION_EXPORT_DOT_H
#define DMZ_APPLICATION_EXPORT_DOT_H

#ifdef _WIN32
#   ifdef DMZ_APPLICATION_EXPORT
#      define DMZ_APPLICATION_LINK_SYMBOL __declspec (dllexport)
#   else
#      define DMZ_APPLICATION_LINK_SYMBOL __declspec (dllimport)
#   endif
#else
#   define DMZ_APPLICATION_LINK_SYMBOL
#endif

#endif // DMZ_APPLICATION_EXPORT_DOT_H
