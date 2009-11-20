#ifndef DMZ_QT_UTIL_DOT_H
#define DMZ_QT_UTIL_DOT_H

#include <dmzQtUtilExport.h>
#include <dmzQtConfigRead.h>
#include <dmzQtConfigWrite.h>

class QObject;
class QWidget;
class QString;


namespace dmz {

   class Config;
   class ObjectType;

   QPointF to_qpointf (const Vector &Source);
   Vector to_dmz_vector (const QPointF &Source);

   QSizeF to_qsizef (const Vector &Source);
   Vector to_dmz_vector (const QSizeF &Source);

   DMZ_QT_UTIL_LINK_SYMBOL QLatin1String to_qstring (const String &Text);
   DMZ_QT_UTIL_LINK_SYMBOL QLatin1String to_qstring (const Handle &ObjectHandle);
   DMZ_QT_UTIL_LINK_SYMBOL QLatin1String to_qstring (const UUID &Identity);
   DMZ_QT_UTIL_LINK_SYMBOL QLatin1String to_qstring (const ObjectType &Type);

   DMZ_QT_UTIL_LINK_SYMBOL Float64 get_qmatrix_scale (const QMatrix &Source);

   void set_qobject_properties (const Config &Source, QObject *object);

   DMZ_QT_UTIL_LINK_SYMBOL void set_qobject_properties (
      const String &Name,
      const Config &Source,
      QObject *object);

   Boolean set_qwidget_stylesheet (const Config &Source, QWidget *widget);

   DMZ_QT_UTIL_LINK_SYMBOL Boolean set_qwidget_stylesheet (
      const String &Name,
      const Config &Source,
      QWidget *widget);


   void set_qwidget_contents_margins (const Config &Source, QWidget *widget);

   DMZ_QT_UTIL_LINK_SYMBOL void set_qwidget_contents_margins (
      const String &Name,
      const Config &Source,
      QWidget *widget);

   DMZ_QT_UTIL_LINK_SYMBOL QString get_save_file_name_with_extension (
      QWidget *parent,
      const QString &title,
      QString dir,
      const QString &filter,
      const QString &extension);

   DMZ_QT_UTIL_LINK_SYMBOL Boolean rename_file (
      const QString &OldName,
      const QString &NewName);
};


inline QPointF
dmz::to_qpointf (const Vector &Source) {

   QPointF result (Source.get_x (), Source.get_y ());
   return result;
}


inline dmz::Vector
dmz::to_dmz_vector (const QPointF &Source) {

   Vector result (Source.x (), Source.y (), 0.0);
   return result;
}


inline QSizeF
dmz::to_qsizef (const Vector &Source) {

   QSizeF result (Source.get_x (), Source.get_y ());
   return result;
}


inline dmz::Vector
dmz::to_dmz_vector (const QSizeF &Source) {

   Vector result (Source.width (), Source.height (), 0.0);
   return result;
}


// inline QMatrix
// dmz::to_qmatrix (const Matrix &Source) {
//
//    Float64 data[9];
//    Source.to_array (data);
//
//    QMatrix result (data[0], data[1], data[2], data[3], data[5], data[6]);
//    return result;
// }


inline void
dmz::set_qobject_properties (const Config &Source, QObject *object) {

   set_qobject_properties ("", Source, object);
}


inline dmz::Boolean
dmz::set_qwidget_stylesheet (const Config &Source, QWidget *widget) {

   return set_qwidget_stylesheet ("", Source, widget);
}


inline void
dmz::set_qwidget_contents_margins (const Config &Source, QWidget  *widget) {

   set_qwidget_contents_margins ("", Source, widget);
}


#endif // DMZ_QT_UTIL_DOT_H

