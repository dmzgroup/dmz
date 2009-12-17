#ifndef DMZ_ZIP_EXPORT_DOT_H
#define DMZ_ZIP_EXPORT_DOT_H

#ifdef _WIN32
#   ifdef DMZ_ZIP_EXPORT
#      define DMZ_ZIP_LINK_SYMBOL __declspec (dllexport)
#   else
#      define DMZ_ZIP_LINK_SYMBOL __declspec (dllimport)
#   endif
#else
#   define DMZ_ZIP_LINK_SYMBOL
#endif

#endif // DMZ_ZIP_EXPORT_DOT_H
