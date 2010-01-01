#ifndef DMZ_QT_MASK_INPUT_DIALOG_DOT_H
#define DMZ_QT_MASK_INPUT_DIALOG_DOT_H

#include "dmzQtObjectConsts.h"
#include <QtGui/QDialog>
#include "ui_MaskInputDialog.h"


namespace dmz {
   
   class QtMaskInputDialog : public QDialog {

      Q_OBJECT

      public:
         QtMaskInputDialog (QWidget *parent = 0);
         ~QtMaskInputDialog ();

         void setLabelText (const QString &Text);
         void setMaskItems (const QStringList &Items);
         void setMask (const QString &Mask);

      public Q_SLOTS:
         void done (int result);
      
      Q_SIGNALS:
         void maskChanged (const QString &);
      
      protected:
         Ui::MaskInputDialog _ui;

      private:
         QtMaskInputDialog ();
         QtMaskInputDialog (const QtMaskInputDialog &);
         QtMaskInputDialog &operator= (const QtMaskInputDialog &);
   };
};

#endif // DMZ_QT_MASK_INPUT_DIALOG_DOT_H
