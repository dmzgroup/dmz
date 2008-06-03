#ifndef DMZ_QT_CONFIG_WRITE_DOT_H
#define DMZ_QT_CONFIG_WRITE_DOT_H

#include <dmzQtUtilExport.h>

class QByteArray;
class QGraphicsView;
class QMatrix;
class QPointF;
class QRectF;
class QSizeF;


namespace dmz {
   
   class Config;
   class String;
   
   
   DMZ_QT_UTIL_LINK_SYMBOL Config qpointf_to_config (
      const String &Name,
      const QPointF &Value);
      
   DMZ_QT_UTIL_LINK_SYMBOL Config qsizef_to_config (
      const String &Name,
      const QSizeF &Value);
      
   DMZ_QT_UTIL_LINK_SYMBOL Config qrectf_to_config (
      const String &Name,
      const QRectF &Value);
   
   DMZ_QT_UTIL_LINK_SYMBOL Config qmatrix_to_config (
      const String &Name,
      const QMatrix &Value);
      
   DMZ_QT_UTIL_LINK_SYMBOL Config qbytearray_to_config (
      const String &Name,
      const QByteArray &Value);
      
   DMZ_QT_UTIL_LINK_SYMBOL Config qgraphicsview_to_config (
      const String &Name,
      const QGraphicsView &Value);
};


#endif // DMZ_QT_CONFIG_WRITE_DOT_H
