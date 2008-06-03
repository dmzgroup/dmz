#ifndef DMZ_LUA_KERNEL_EXPORT_DOT_H
#define DMZ_LUA_KERNEL_EXPORT_DOT_H

#ifdef _WIN32
#   ifdef DMZ_LUA_KERNEL_EXPORT
#      define DMZ_LUA_KERNEL_LINK_SYMBOL __declspec (dllexport)
#   else
#      define DMZ_LUA_KERNEL_LINK_SYMBOL __declspec (dllimport)
#   endif
#else
#   define DMZ_LUA_KERNEL_LINK_SYMBOL
#endif

#endif // DMZ_LUA_KERNEL_EXPORT_DOT_H
