#include "dmzQtMaskInputDialog.h"
#include <QtGui/QtGui>


dmz::QtMaskInputDialog::QtMaskInputDialog (QWidget *parent) : QDialog (parent) {
         
   _ui.setupUi (this);
}


dmz::QtMaskInputDialog::~QtMaskInputDialog () {;}


void
dmz::QtMaskInputDialog::setLabelText (const QString &Text) {

   _ui.label->setText (Text);
}


void
dmz::QtMaskInputDialog::setMaskItems (const QStringList &Items) {

   _ui.listWidget->clear ();
   
   foreach (QString text, Items) {
   
      QListWidgetItem *item = new QListWidgetItem (text, _ui.listWidget);
      item->setFlags (Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);      
      item->setCheckState (Qt::Unchecked);
   }
}


void
dmz::QtMaskInputDialog::setMask (const QString &Mask) {

   const QStringList MaskList = Mask.split (" | ");
   
   for (int row = 0; row < _ui.listWidget->count (); row++) {
      
      QListWidgetItem *item = _ui.listWidget->item (row);
      
      if (MaskList.contains (item->text ())) {
         
         item->setCheckState (Qt::Checked);
      }
   }
}


void
dmz::QtMaskInputDialog::done (int result) {
   
   if (result) {
      
      QStringList maskList;
      
      for (int row = 0; row < _ui.listWidget->count (); row++) {

         QListWidgetItem *item = _ui.listWidget->item (row);

         if (item->checkState () == Qt::Checked) {
            
            maskList.append (item->text ());
         }
      }
      
      Q_EMIT maskChanged (maskList.join (" | "));
   }
   
   QDialog::done (result);
}
