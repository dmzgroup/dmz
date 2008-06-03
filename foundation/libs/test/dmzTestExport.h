#ifndef DMZ_TEST_EXPORT_DOT_H
#define DMZ_TEST_EXPORT_DOT_H

#ifdef _WIN32
#   ifdef DMZ_TEST_EXPORT
#      define DMZ_TEST_LINK_SYMBOL __declspec (dllexport)
#   else
#      define DMZ_TEST_LINK_SYMBOL __declspec (dllimport)
#   endif
#else
#   define DMZ_TEST_LINK_SYMBOL
#endif

#endif // DMZ_TEST_EXPORT_DOT_H
