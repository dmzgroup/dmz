#ifndef DMZ_QT_SINGLETON_APPLICATION_DOT_H
#define DMZ_QT_SINGLETON_APPLICATION_DOT_H

#include <dmzQtUtilExport.h>

#include <QtGui/QtGui>

class QEvent;

namespace dmz {

   class RuntimeContext;

   class DMZ_QT_UTIL_LINK_SYMBOL QtSingletonApplication : public QApplication {

      Q_OBJECT

      public:
         QtSingletonApplication (
            const QString &id,
            int argc,
            char **argv,
            RuntimeContext *context);

         ~QtSingletonApplication ();

         QString get_requested_file ();

         bool start_application ();
         bool send_to_running_application (const QString &Message);

      protected:
         struct State;

         bool event (QEvent *event);

         State &_state;
   };
};

#endif // DMZ_QT_SINGLETON_APPLICATION_DOT_H
