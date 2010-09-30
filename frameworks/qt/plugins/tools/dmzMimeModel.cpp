#include "dmzMimeModel.h"

#include <QtCore/QMimeData>

dmz::MimeModel::MimeModel () {;}


dmz::MimeModel::~MimeModel () {;}


QMimeData *
dmz::MimeModel::mimeData (const QModelIndexList &indexes) const {

   QMimeData *result = new QMimeData();

   foreach (QModelIndex index, indexes) {

      if (index.isValid()) {

         QString text = data(index, Qt::DisplayRole).toString();
         result->setText (text);
      }
   }

   //result->setData("application/vnd.text.list", encodedData);

   return result;
}


QStringList 
dmz::MimeModel::mimeTypes () const {

    QStringList result;
    result << "application/vnd.text.list";
    return result;
}

