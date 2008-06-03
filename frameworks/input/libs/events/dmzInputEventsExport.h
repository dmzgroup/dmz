#ifndef DMZ_INPUT_EVENTS_EXPORT_DOT_H
#define DMZ_INPUT_EVENTS_EXPORT_DOT_H

#ifdef _WIN32
#   ifdef DMZ_INPUT_EVENTS_EXPORT
#      define DMZ_INPUT_EVENTS_LINK_SYMBOL __declspec (dllexport)
#   else
#      define DMZ_INPUT_EVENTS_LINK_SYMBOL __declspec (dllimport)
#   endif
#else
#   define DMZ_INPUT_EVENTS_LINK_SYMBOL
#endif

#endif // DMZ_INPUT_EVENTS_EXPORT_DOT_H
