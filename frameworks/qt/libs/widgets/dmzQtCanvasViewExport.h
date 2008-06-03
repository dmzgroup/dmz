#ifndef DMZ_QT_CANVAS_VIEW_EXPORT_DOT_H
#define DMZ_QT_CANVAS_VIEW_EXPORT_DOT_H

#ifdef _WIN32

#ifdef DMZ_QT_CANVAS_VIEW_EXPORT
#define DMZ_QT_CANVAS_VIEW_LINK_SYMBOL __declspec (dllexport)
#else
#define DMZ_QT_CANVAS_VIEW_LINK_SYMBOL __declspec (dllimport)
#endif // DMZ_QT_CANVAS_VIEW_EXPORT

#else // !_WIN32

#define DMZ_QT_CANVAS_VIEW_LINK_SYMBOL

#endif // _WIN32

#endif // DMZ_QT_CANVAS_VIEW_EXPORT_DOT_H
