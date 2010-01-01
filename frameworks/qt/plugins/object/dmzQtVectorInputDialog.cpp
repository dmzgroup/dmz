#include "dmzQtVectorInputDialog.h"
#include <QtGui/QtGui>


dmz::QtVectorInputDialog::QtVectorInputDialog (QWidget *parent) : QDialog (parent) {
         
   _ui.setupUi (this);
}


dmz::QtVectorInputDialog::~QtVectorInputDialog () {;}


void
dmz::QtVectorInputDialog::setLabelText (const QString &Text) {

   _ui.label->setText (Text);
}



void
dmz::QtVectorInputDialog::setVector (const Vector &Value) {

   _ui.xSpinBox->setValue (Value.get_x ());
   _ui.ySpinBox->setValue (Value.get_y ());
   _ui.zSpinBox->setValue (Value.get_z ());
}


void
dmz::QtVectorInputDialog::done (int result) {
   
   if (result) {
      
      Vector vec (
         _ui.xSpinBox->value (),
         _ui.ySpinBox->value (),
         _ui.zSpinBox->value ());
         
      Q_EMIT vectorChanged (vec);
   }
   
   QDialog::done (result);
}
