#ifndef DMZ_QT_LOG_OBSERVER_WIDGET_DOT_H
#define DMZ_QT_LOG_OBSERVER_WIDGET_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzTypesString.h>
#include <QtGui/QWidget>
#include "ui_dmzQtLogObserverWidget.h"


namespace dmz {

   class RuntimeContext;

   class QtLogObserverWidget : public QWidget {

      Q_OBJECT

      public:
         QtLogObserverWidget (QWidget *parent = 0);
         ~QtLogObserverWidget ();

         void store_log_message (
            const String &LogName,
            const LogLevelEnum Level,
            const String &Message);

      public slots:
         void find ();
         void findNext ();
         void findPrevious ();

      protected:
         void keyPressEvent (QKeyEvent *event);

      protected slots:
         void on_loadButton_clicked ();
         void on_saveButton_clicked ();
         void on_logComboBox_activated (int index);

         void _find (
            const QString &Text,
            const Boolean Forward = False,
            const Boolean Backward = False);

      protected:
         Ui::QtLogObserverWidget _ui;
         UInt32 _messageNumber;
         QStringList _logList;
         QString _lastSavedLog;

      private:
         QtLogObserverWidget (const QtLogObserverWidget &);
         QtLogObserverWidget &operator= (const QtLogObserverWidget &);
   };
};

#endif // DMZ_QT_LOG_OBSERVER_WIDGET_DOT_H

