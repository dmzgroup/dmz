#ifndef DMZ_RENDER_UTIL_OSG_EXPORT_DOT_H
#define DMZ_RENDER_UTIL_OSG_EXPORT_DOT_H

#ifdef _WIN32
#   ifdef DMZ_RENDER_UTIL_OSG_EXPORT
#      define DMZ_RENDER_UTIL_OSG_LINK_SYMBOL __declspec (dllexport)
#   else
#      define DMZ_RENDER_UTIL_OSG_LINK_SYMBOL __declspec (dllimport)
#   endif
#else
#   define DMZ_RENDER_UTIL_OSG_LINK_SYMBOL
#endif

#endif // DMZ_RENDER_UTIL_OSG_EXPORT_DOT_H
