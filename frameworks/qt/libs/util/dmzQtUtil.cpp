#include <dmzQtUtil.h>
#include <math.h>
#include <QtGui/QtGui>


dmz::Float64
dmz::get_qmatrix_scale (const QMatrix &Source) {
   
   qreal scaleX = Source.m11 ();
   qreal scaleY = Source.m12 ();
   qreal scale = sqrt (scaleX * scaleX + scaleY + scaleY);
   return scale;
}


void
dmz::set_qobject_properties (const String &Name, const Config &Source, QObject *object) {
   
   if (object) {
      
      Config properties;

      if (Name) { Source.lookup_all_config_merged (Name, properties); }
      else { properties = Source; }

      ConfigIterator it;
      Config cd;

      while (properties.get_next_config (it, cd)) {

         const QVariant Value (config_to_string ("value", cd).get_buffer ());
         
         object->setProperty (cd.get_name ().get_buffer (), Value);
      }
   }
}


dmz::Boolean
dmz::set_qwidget_stylesheet (const String &Name, const Config &Source, QWidget *widget) {
   
   Boolean result (False);
   
   if (widget) {
      
      Config cd;

      if (Name) { Source.lookup_config (Name, cd); }
      else { cd = Source; }

      String qss (config_to_string ("file", cd));
      
      if (qss) {
      
         QFile file (qss.get_buffer ());
         if (file.open (QFile::ReadOnly)) {
            
            QString styleSheet (QLatin1String (file.readAll ()));
            widget->setStyleSheet (styleSheet);

            qDebug () << widget->objectName () << "Style Sheet:" << qss.get_buffer ();
            result = True;
         }
      }
   }
   
   return result;
}

