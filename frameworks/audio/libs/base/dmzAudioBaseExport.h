#ifndef DMZ_AUDIO_BASE_EXPORT_DOT_H
#define DMZ_AUDIO_BASE_EXPORT_DOT_H

#ifdef _WIN32
#   ifdef DMZ_AUDIO_BASE_EXPORT
#      define DMZ_AUDIO_BASE_LINK_SYMBOL __declspec (dllexport)
#   else
#      define DMZ_AUDIO_BASE_LINK_SYMBOL __declspec (dllimport)
#   endif
#else
#   define DMZ_AUDIO_BASE_LINK_SYMBOL
#endif

#endif // DMZ_AUDIO_BASE_EXPORT_DOT_H
