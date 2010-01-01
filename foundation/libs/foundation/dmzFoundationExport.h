#ifndef DMZ_FOUNDATION_EXPORT_DOT_H
#define DMZ_FOUNDATION_EXPORT_DOT_H

#ifdef _WIN32
#   ifdef DMZ_FOUNDATION_EXPORT
#      define DMZ_FOUNDATION_LINK_SYMBOL __declspec (dllexport)
#   else
#      define DMZ_FOUNDATION_LINK_SYMBOL __declspec (dllimport)
#   endif
#else
#   define DMZ_FOUNDATION_LINK_SYMBOL
#endif

#endif // DMZ_FOUNDATION_EXPORT_DOT_H
