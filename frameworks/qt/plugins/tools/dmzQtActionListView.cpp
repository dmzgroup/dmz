#include "dmzQtActionListView.h"

#include <QtGui/QDrag>
#include <QtGui/QStandardItemModel>

namespace {
// Basically mimic  QAbstractItemView's startDrag routine, except that
// another pixmap is used, we don't want the whole row.

void
startActionDrag (
      QWidget *dragParent,
      QAbstractItemModel *model,
      const QModelIndexList &indexes,
      Qt::DropActions supportedActions) {

   if (indexes.empty())
       return;

   QDrag *drag = new QDrag(dragParent);
   QMimeData *data = model->mimeData(indexes);
   drag->setMimeData (data);

   QStandardItemModel *smodel = dynamic_cast<QStandardItemModel *>(model);

   if (smodel) {

      QStandardItem *item = smodel->itemFromIndex (indexes.front ());

      drag->setPixmap (item->icon ().pixmap (72));
   }

   drag->setHotSpot (QPoint (36, 36));
   drag->start (supportedActions);
}

};

dmz::QtActionListView::QtActionListView (QWidget *parent) : QListView (parent) {;}


void
dmz::QtActionListView::startDrag (Qt::DropActions supportedActions) {

   startActionDrag(this, model (), selectedIndexes(), supportedActions);
}

