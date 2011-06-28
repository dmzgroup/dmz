#ifndef DMZ_QT_VIEW_LINK_DIALOG_DOT_H
#define DMZ_QT_VIEW_LINK_DIALOG_DOT_H

#include "dmzQtObjectConsts.h"
#include <dmzTypesString.h>
#include <QtGui/QDialog>
#include "ui_ViewLinkDialog"

namespace dmz {

   class QtViewLinkDialog : public QDialog {

   Q_OBJECT

   public:
      QtViewLinkDialog (QWidget *parent = 0);
      ~QtViewLinkDialog ();



   };
}

#endif // DMZ_QT_VIEW_LINK_DIALOG_DOT_H
