#ifndef DMZ_MIME_MODEL_DOT_H
#define DMZ_MIME_MODEL_DOT_H

#include <QtGui/QStandardItemModel>

namespace dmz {

class MimeModel : public QStandardItemModel {

   public:
      MimeModel ();
      ~MimeModel ();

      virtual QMimeData *mimeData (const QModelIndexList &indexes) const;
      virtual QStringList mimeTypes () const;
};

};

#endif // DMZ_MIME_MODEL_DOT_H
