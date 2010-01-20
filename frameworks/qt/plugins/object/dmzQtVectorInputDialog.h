#ifndef DMZ_QT_VECTOR_INPUT_DIALOG_DOT_H
#define DMZ_QT_VECTOR_INPUT_DIALOG_DOT_H

#include "dmzQtObjectConsts.h"
#include <dmzTypesVector.h>
#include <QtGui/QDialog>
#include "ui_VectorInputDialog.h"


namespace dmz {
   
   class QtVectorInputDialog : public QDialog {

      Q_OBJECT

      public:
         QtVectorInputDialog (QWidget *parent = 0);
         ~QtVectorInputDialog ();

         void setLabelText (const QString &Text);
         void setVector (const Vector &Value);

      public Q_SLOTS:
         void done (int result);
      
      Q_SIGNALS:
         void vectorChanged (const Vector &Value);
      
      protected:
         Ui::VectorInputDialog _ui;

      private:
         // QtVectorInputDialog ();
         QtVectorInputDialog (const QtVectorInputDialog &);
         QtVectorInputDialog &operator= (const QtVectorInputDialog &);
   };
};

#endif // DMZ_QT_VECTOR_INPUT_DIALOG_DOT_H
