#include <dmzQtUtil.h>
#include <dmzRuntimeObjectType.h>
#include <dmzTypesStringContainer.h>
#include <dmzTypesUUID.h>
#include <math.h>
#include <QtGui/QtGui>


QString
dmz::to_qstring (const Handle &ObjectHandle) {

   String result; result << ObjectHandle;
   return to_qstring (result);
}


QString
dmz::to_qstring (const String &Text) {

   QString result (Text.get_buffer ());
   return result;
}


QString
dmz::to_qstring (const UUID &Identity) {

   QString result (Identity.to_string ().get_buffer ());
   return result;
}


QString
dmz::to_qstring (const ObjectType &Type) {

   QString result (Type.get_name ().get_buffer ());
   return result;
}


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

            QString styleSheet (file.readAll ());
            widget->setStyleSheet (styleSheet);

            qDebug () << widget->objectName () << "Style Sheet:" << qss.get_buffer ();
            result = True;
         }
      }
   }

   return result;
}


void
dmz::set_qwidget_contents_margins (const String &Name, const Config &Source, QWidget *widget) {

   if (widget) {

      Config cd;

      if (Name) { Source.lookup_config (Name, cd); }
      else { cd = Source; }

      Int32 left (config_to_int32 ("left", cd, 0));
      Int32 top (config_to_int32 ("top", cd, 0));
      Int32 right (config_to_int32 ("right", cd, 0));
      Int32 bottom (config_to_int32 ("bottom", cd, 0));

      widget->setContentsMargins (left, top, right, bottom);
   }
}


// Prompt for a file and make sure an extension is added
// unless the user explicitly specifies another one.

QString
dmz::get_save_file_name_with_extension (
      QWidget *parent,
      const QString &title,
      QString dir,
      const QString &filter,
      const QString &extension) {

    const QChar dot = QLatin1Char('.');

    QString saveFile;
    while (true) {

        saveFile = QFileDialog::getSaveFileName(
           parent, title, dir, filter, 0, QFileDialog::DontConfirmOverwrite);

        if (saveFile.isEmpty())
            return saveFile;

        const QFileInfo fInfo(saveFile);
        if (fInfo.suffix().isEmpty() && !fInfo.fileName().endsWith(dot)) {

            saveFile += dot;
            saveFile += extension;
        }

        const QFileInfo fi(saveFile);
        if (!fi.exists())
            break;

        const QString prompt =
           QObject::tr("%1 already exists.\nDo you want to replace it?").arg(fi.fileName());

        if (QMessageBox::warning(parent, title, prompt, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            break;

        dir = saveFile;
    }
    return saveFile;
}



dmz::Boolean
dmz::rename_file (const QString &OldName, const QString &NewName) {

   Boolean retVal (False);

   if (OldName == NewName) { retVal = True; }
   else {

      if (QFile::exists (NewName)) { QFile::remove (NewName); }

      if (QFile::rename (OldName, NewName)) { retVal = True; }
   }

   return retVal;
}


void
dmz::to_qstringlist (const StringContainer &Source, QStringList &target) {

   target.clear ();

   String data;
   StringContainerIterator it;

   while (Source.get_next (it, data)) {

      target.append (data.get_buffer ());
   }
}


void
dmz::to_dmz_string_container (const QStringList &Source, StringContainer &target) {

   target.clear ();

   foreach (QString data, Source) {

      target.append (qPrintable (data));
   }
}

