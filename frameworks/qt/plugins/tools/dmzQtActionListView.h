#ifndef DMZ_QT_ACTION_LIST_VIEW_DOT_H
#define DMZ_QT_ACTION_LIST_VIEW_DOT_H

#include <QtGui/QListView>

namespace dmz {

class QtActionListView : public QListView {

   public:
      QtActionListView (QWidget *parent = 0);
      virtual void startDrag (Qt::DropActions supportedActions);
};

};

#endif // DMZ_QT_ACTION_LIST_VIEW_DOT_H
