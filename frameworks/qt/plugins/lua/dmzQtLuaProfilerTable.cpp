#include <dmzSystemFile.h>
#include <dmzSystemStreamFile.h>
#include "dmzQtLuaProfilerTable.h"
#include <QtGui/QtGui>

#include <qdb.h>
static dmz::qdb out;

dmz::QtLuaProfilerTable::QtLuaProfilerTable () : model (0, 4, this), proxyModel (this) {

   ui.setupUi (this);
   setAttribute (Qt::WA_DeleteOnClose);
   proxyModel.setSourceModel (&model);
   proxyModel.setDynamicSortFilter (true);
   ui.tableView->setModel (&proxyModel);
   QStringList labels;
   labels << "Order" << "Percent (%)" << "Function" << "Source";
   model.setHorizontalHeaderLabels (labels);
   QHeaderView *header (ui.tableView->horizontalHeader ());
   if (header) {

      header->setResizeMode (0, QHeaderView::ResizeToContents);
      header->setResizeMode (1, QHeaderView::ResizeToContents);
      header->setResizeMode (2, QHeaderView::ResizeToContents);
      header->setResizeMode (3, QHeaderView::ResizeToContents);
   }
   
   adjustSize ();
}


dmz::QtLuaProfilerTable::~QtLuaProfilerTable () {;}


void
dmz::QtLuaProfilerTable::on_saveButton_clicked () {

   QString fileName =
      QFileDialog::getSaveFileName (
         this,
         tr ("Save Profiler Results"),
         "", // _get_last_path (),
         QString ("*.txt"));

   if (!fileName.isEmpty ()) {

      FILE *file = open_file (qPrintable (fileName), "wb");

      if (file) {

         StreamFile out (file);

         const int Rows (model.rowCount ());

         for (int ix = 0; ix < Rows; ix++) {

            QStandardItem *item1 (model.item (ix, 1));
            QStandardItem *item2 (model.item (ix, 2));
            QStandardItem *item3 (model.item (ix, 3));

            out << qPrintable (item1->text ()) << endl
               << qPrintable (item2->text ()) << endl
               << qPrintable (item3->text ()) << endl;
         }

         close_file (file);
      }
   }
}
