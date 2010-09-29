#include "dmzQtActionListView.h"

#include <QtGui/QDrag>
#include <QtGui/QStandardItemModel>

namespace {

static void
startActionDrag (
      QWidget *dragParent,
      QAbstractItemModel *model,
      const QModelIndexList &indexes,
      Qt::DropActions supportedActions,
      const int Extent) {

   if (indexes.empty())
       return;

   const int HalfExtent = Extent / 2;

   QDrag *drag = new QDrag (dragParent);
   QMimeData *data = model->mimeData (indexes);
   drag->setMimeData (data);

   QStandardItemModel *smodel = dynamic_cast<QStandardItemModel *>(model);

   if (smodel) {

      QStandardItem *item = smodel->itemFromIndex (indexes.front ());

      drag->setPixmap (item->icon ().pixmap (Extent));
   }

   drag->setHotSpot (QPoint (HalfExtent, HalfExtent));
   drag->start (supportedActions);
}

};

dmz::QtActionListView::QtActionListView (QWidget *parent) :
      QListView (parent),
      _iconExtent (72) {;}


void
dmz::QtActionListView::startDrag (Qt::DropActions supportedActions) {

   startActionDrag (this, model (), selectedIndexes(), supportedActions, _iconExtent);
}


void
dmz::QtActionListView::set_icon_extent (const int Extent) {

   if (Extent > 0) { _iconExtent = Extent; }
}

