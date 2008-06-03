#ifndef DMZ_QT_LUA_PROFILER_TABLE_DOT_H
#define DMZ_QT_LUA_PROFILER_TABLE_DOT_H

#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QStandardItemModel>
#include <QtGui/QWidget>
#include <ui_dmzQtLuaProfilerTable.h>

namespace dmz {

   class QtLuaProfilerTable : public QWidget {

         Q_OBJECT

      public:
         QtLuaProfilerTable ();
         ~QtLuaProfilerTable ();

         Ui::QtLuaProfilerTable ui;
         QStandardItemModel model;
         QSortFilterProxyModel proxyModel;

      protected slots:
         void on_saveButton_clicked ();

      private:
         QtLuaProfilerTable (const QtLuaProfilerTable &);
         QtLuaProfilerTable &operator= (const QtLuaProfilerTable &);

   };
};

#endif // DMZ_QT_LUA_PROFILER_TABLE_DOT_H
