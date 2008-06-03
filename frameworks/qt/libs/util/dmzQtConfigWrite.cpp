#include <dmzQtConfigWrite.h>
#include <dmzQtUtil.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigWrite.h>
#include <dmzTypesString.h>
#include <dmzTypesVector.h>
#include <QtCore/QtCore>
#include <QtGui/QtGui>


dmz::Config
dmz::qpointf_to_config (const String &Name, const QPointF &Value) {
   
   Config result (Name);

   String val;

   val << Value.x ();
   result.store_attribute ("x", val);

   val.flush () << Value.y ();
   result.store_attribute ("y", val);

   return result;
}


dmz::Config
dmz::qsizef_to_config (const String &Name, const QSizeF &Value) {
   
   Config result (Name);

   String val;

   val << Value.width ();
   result.store_attribute ("width", val);

   val.flush () << Value.height ();
   result.store_attribute ("height", val);

   return result;
}


dmz::Config
dmz::qrectf_to_config (const String &Name, const QRectF &Value) {
   
   Config result (Name);

   result.add_config (qpointf_to_config ("point", Value.topLeft ()));
   result.add_config (qsizef_to_config ("size", Value.size ()));

   return result;
}


dmz::Config
dmz::qmatrix_to_config (const String &Name, const QMatrix &Value) {
   
   Config result (Name);

   String val;

   val << Value.m11 ();
   result.store_attribute ("m11", val);

   val.flush () << Value.m12 ();
   result.store_attribute ("m12", val);

   val.flush () << Value.m21 ();
   result.store_attribute ("m21", val);

   val.flush () << Value.m22 ();
   result.store_attribute ("m22", val);

   val.flush () << Value.dx ();
   result.store_attribute ("dx", val);

   val.flush () << Value.dy ();
   result.store_attribute ("dy", val);

   return result;
}


dmz::Config
dmz::qbytearray_to_config (const String &Name, const QByteArray &Value) {
   
   Config result (Name);

   QByteArray base64 (Value.toBase64 ());
   String val;

   val << base64.constData ();
   result.store_attribute ("data", val);

   return result;
}


dmz::Config
dmz::qgraphicsview_to_config (const String &Name, const QGraphicsView &Value) {
   
   Config result (Name);

   result.add_config (qmatrix_to_config ("matrix", Value.matrix ()));
   
   QSize vpSize (Value.viewport ()->size ());
   QPoint center (vpSize.width () / 2, vpSize.height () / 2);
   result.add_config (qpointf_to_config ("center", Value.mapToScene (center)));

   return result;
}

