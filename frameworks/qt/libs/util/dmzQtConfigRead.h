#ifndef DMZ_QT_CONFIG_READ_DOT_H
#define DMZ_QT_CONFIG_READ_DOT_H

#include <dmzQtUtilExport.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzTypesString.h>
#include <dmzTypesVector.h>
#include <QtCore/QByteArray>
#include <QtCore/QPointF>
#include <QtCore/QRectF>
#include <QtCore/QSizeF>
#include <QtGui/QColor>
#include <QtGui/QIcon>
#include <QtGui/QMatrix>

class QAbstractButton;
class QAction;
class QGraphicsView;
class QPushButton;
class QToolBar;
class QToolButton;


namespace dmz {

   QPointF config_to_qpointf (const Config &Source);
   QPointF config_to_qpointf (const String &Name, const Config &Source);
   QPointF config_to_qpointf (const Config &Source, const QPointF &DefaultValue);

   DMZ_QT_UTIL_LINK_SYMBOL QPointF config_to_qpointf (
      const String &Name,
      const Config &Source,
      const QPointF &DefaultValue);

   QSizeF config_to_qsizef (const Config &Source);
   QSizeF config_to_qsizef (const String &Name, const Config &Source);
   QSizeF config_to_qsizef (const Config &Source, const QSizeF &DefaultValue);

   DMZ_QT_UTIL_LINK_SYMBOL QSizeF config_to_qsizef (
      const String &Name,
      const Config &Source,
      const QSizeF &DefaultValue);

   QRectF config_to_qrectf (const Config &Source);
   QRectF config_to_qrectf (const String &Name, const Config &Source);
   QRectF config_to_qrectf (const Config &Source, const QRectF &DefaultValue);

   DMZ_QT_UTIL_LINK_SYMBOL QRectF config_to_qrectf (
      const String &Name,
      const Config &Source,
      const QRectF &DefaultValue);

   QMatrix config_to_qmatrix (const Config &Source);
   QMatrix config_to_qmatrix (const String &Name, const Config &Source);
   QMatrix config_to_qmatrix (const Config &Source, const QMatrix &DefaultValue);

   DMZ_QT_UTIL_LINK_SYMBOL QMatrix config_to_qmatrix (
      const String &Name,
      const Config &Source,
      const QMatrix &DefaultValue);

   QByteArray config_to_qbytearray (const Config &Source);
   QByteArray config_to_qbytearray (const String &Name, const Config &Source);
   QByteArray config_to_qbytearray (const Config &Source, const QByteArray &DefaultValue);

   DMZ_QT_UTIL_LINK_SYMBOL QByteArray config_to_qbytearray (
      const String &Name,
      const Config &Source,
      const QByteArray &DefaultValue);

   QColor config_to_qcolor (const Config &Source);
   QColor config_to_qcolor (const String &Name, const Config &Source);
   QColor config_to_qcolor (const Config &Source, const QColor &DefaultValue);

   DMZ_QT_UTIL_LINK_SYMBOL QColor config_to_qcolor (
      const String &Name,
      const Config &Source,
      const QColor &DefaultValue);

   DMZ_QT_UTIL_LINK_SYMBOL void qicon_config_read (
      const String &Name,
      const Config &Source,
      QIcon *icon);

   DMZ_QT_UTIL_LINK_SYMBOL void qobject_config_read (
      const String &Name,
      const Config &Source,
      QObject *object);

   DMZ_QT_UTIL_LINK_SYMBOL void qwidget_config_read (
      const String &Name,
      const Config &Source,
      QWidget *widget);

   DMZ_QT_UTIL_LINK_SYMBOL void qaction_config_read (
      const String &Name,
      const Config &Source,
      QAction *action);

   DMZ_QT_UTIL_LINK_SYMBOL void qtoolbar_config_read (
      const String &Name,
      const Config &Source,
      QToolBar *toolBar);

   DMZ_QT_UTIL_LINK_SYMBOL void qgraphicsview_config_read (
      const String &Name,
      const Config &Source,
      QGraphicsView *view);

   DMZ_QT_UTIL_LINK_SYMBOL void qabstractbutton_config_read (
      const String &Name,
      const Config &String,
      QAbstractButton *button);

   DMZ_QT_UTIL_LINK_SYMBOL void qpushbutton_config_read (
      const String &Name,
      const Config &String,
      QPushButton *button);

   DMZ_QT_UTIL_LINK_SYMBOL void qtoolbutton_config_read (
      const String &Name,
      const Config &String,
      QToolButton *button);
};


inline QPointF
dmz::config_to_qpointf (const Config &Data) {

   const QPointF Value;
   return config_to_qpointf ("", Data, Value);
}


inline QPointF
dmz::config_to_qpointf (const String &Name, const Config &Data) {

   const QPointF Value;
   return config_to_qpointf (Name, Data, Value);
}


inline QPointF
dmz::config_to_qpointf (const Config &Data, const QPointF &Value) {

   return config_to_qpointf ("", Data, Value);
}


inline QSizeF
dmz::config_to_qsizef (const Config &Data) {

   const QSizeF Value;
   return config_to_qsizef ("", Data, Value);
}


inline QSizeF
dmz::config_to_qsizef (const String &Name, const Config &Data) {

   const QSizeF Value;
   return config_to_qsizef (Name, Data, Value);
}


inline QSizeF
dmz::config_to_qsizef (const Config &Data, const QSizeF &Value) {

   return config_to_qsizef ("", Data, Value);
}


inline QRectF
dmz::config_to_qrectf (const Config &Data) {

   const QRectF Value;
   return config_to_qrectf ("", Data, Value);
}


inline QRectF
dmz::config_to_qrectf (const String &Name, const Config &Data) {

   const QRectF Value;
   return config_to_qrectf (Name, Data, Value);
}


inline QRectF
dmz::config_to_qrectf (const Config &Data, const QRectF &Value) {

   return config_to_qrectf ("", Data, Value);
}


inline QMatrix
dmz::config_to_qmatrix (const Config &Data) {

   const QMatrix Value;
   return config_to_qmatrix ("", Data, Value);
}


inline QMatrix
dmz::config_to_qmatrix (const String &Name, const Config &Data) {

   const QMatrix Value;
   return config_to_qmatrix (Name, Data, Value);
}


inline QMatrix
dmz::config_to_qmatrix (const Config &Data, const QMatrix &Value) {

   return config_to_qmatrix ("", Data, Value);
}


inline QByteArray
dmz::config_to_qbytearray (const Config &Data) {

   const QByteArray Value;
   return config_to_qbytearray ("", Data, Value);
}


inline QByteArray
dmz::config_to_qbytearray (const String &Name, const Config &Data) {

   const QByteArray Value;
   return config_to_qbytearray (Name, Data, Value);
}


inline QByteArray
dmz::config_to_qbytearray (const Config &Data, const QByteArray &Value) {

   return config_to_qbytearray ("", Data, Value);
}


inline QColor
dmz::config_to_qcolor (const Config &Data) {

   const QColor Value;
   return config_to_qcolor ("", Data, Value);
}


inline QColor
dmz::config_to_qcolor (const String &Name, const Config &Data) {

   const QColor Value;
   return config_to_qcolor (Name, Data, Value);
}


inline QColor
dmz::config_to_qcolor (const Config &Data, const QColor &Value) {

   return config_to_qcolor ("", Data, Value);
}


// inline QIcon
// dmz::config_to_qicon (const Config &Data) {
//
//    const QIcon Value;
//    return config_to_qicon ("", Data, Value);
// }
//
//
// inline QIcon
// dmz::config_to_qicon (const String &Name, const Config &Data) {
//
//    const QIcon Value;
//    return config_to_qicon (Name, Data, Value);
// }
//
//
// inline QIcon
// dmz::config_to_qicon (const Config &Data, const QIcon &Value) {
//
//    return config_to_qicon ("", Data, Value);
// }


#endif // DMZ_QT_CONFIG_READ_DOT_H

