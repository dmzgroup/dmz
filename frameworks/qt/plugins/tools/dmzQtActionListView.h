#ifndef DMZ_QT_ACTION_LIST_VIEW_DOT_H
#define DMZ_QT_ACTION_LIST_VIEW_DOT_H

#include <QtGui/QListView>

namespace dmz {

class QtActionListView : public QListView {

   public:
      QtActionListView (QWidget *parent = 0);
      virtual void startDrag (Qt::DropActions supportedActions);

      void set_icon_extent (const int Extent);

   protected:
      int _iconExtent;
};

};

#endif // DMZ_QT_ACTION_LIST_VIEW_DOT_H
