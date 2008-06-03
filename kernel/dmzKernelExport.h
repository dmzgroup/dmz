#ifndef DMZ_KERNEL_EXPORT_DOT_H
#define DMZ_KERNEL_EXPORT_DOT_H

#ifdef _WIN32
#   ifdef DMZ_KERNEL_EXPORT
#      define DMZ_KERNEL_LINK_SYMBOL __declspec (dllexport)
#   else
#      define DMZ_KERNEL_LINK_SYMBOL __declspec (dllimport)
#   endif
#else
#   define DMZ_KERNEL_LINK_SYMBOL
#endif

#endif // DMZ_KERNEL_EXPORT_DOT_H
