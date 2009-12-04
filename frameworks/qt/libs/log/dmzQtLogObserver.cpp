#include "dmzQtLogObserverWidget.h"
#include <dmzQtLogObserver.h>
#include <dmzQtUtil.h>
#include <dmzRuntimeSession.h>
#include <dmzRuntimeConfigWrite.h>
#include <QtGui/QtGui>


namespace {

   const char DefaultName[] = "QtLogObserver";
};


struct dmz::QtLogObserver::State {

   Boolean processUpdates;
   RuntimeContext *context;
   QtLogObserverWidget *logWidget;
};


dmz::QtLogObserver::QtLogObserver (
      RuntimeContext *context,
      QWidget *parent) :
      QWidget (parent),
      LogObserver (context),
      MessageObserver (0, DefaultName, context),
      _state (*(new State ())) {

   _state.context = context;
   _state.processUpdates = False;
   _state.logWidget = new QtLogObserverWidget (this);

   QVBoxLayout *layout (new QVBoxLayout (this));
   layout->setMargin (8);
   layout->setSpacing (0);
   layout->addWidget (_state.logWidget, 0);

   setWindowTitle (tr ("DMZ Log Output"));
   qApp->installEventFilter (this);
}


dmz::QtLogObserver::~QtLogObserver () {

   delete &_state;
}


void
dmz::QtLogObserver::set_process_updates (const Boolean Value) {

   _state.processUpdates = Value;
}


void
dmz::QtLogObserver::save_session () {

   if (_state.context) {

      String data;

      Config session (DefaultName);

      session.add_config (qbytearray_to_config ("geometry", saveGeometry ()));

      if (isVisible ()) {

         session.add_config (boolean_to_config ("window", "visible", True));
      }

      set_session_config (_state.context, session);
   }
}


void
dmz::QtLogObserver::load_session () {

   if (_state.context) {

      Config session (get_session_config (DefaultName, _state.context));

      QByteArray geometry (config_to_qbytearray ("geometry", session, saveGeometry ()));
      restoreGeometry (geometry);

      if (config_to_boolean ("window.visible", session, False)) { show (); }
   }
}


void
dmz::QtLogObserver::toggle_visibility () {

   if (isVisible ()) { close (); }
   else { show (); raise (); }
}

bool
dmz::QtLogObserver::eventFilter (QObject *object, QEvent *event) {

   bool retVal = false;

   if (event) {

      if (event->type () == QEvent::KeyPress) {

         QKeyEvent *keyEvent = (QKeyEvent *) event;

         if ((keyEvent->key () == Qt::Key_L) &&
             ((keyEvent->modifiers () & (Qt::SHIFT | Qt::CTRL)) ==
                (Qt::SHIFT | Qt::CTRL))) {

            toggle_visibility ();
            retVal = true;
         }
      }
   }

   return retVal;
}


// Message Observer Interface
void
dmz::QtLogObserver::receive_message (
      const Message &Msg,
      const UInt32 MessageSendHandle,
      const UInt32 TargetObserverHandle,
      const Data *InData,
      Data *outData) {

}


// Log Observer Interface
void
dmz::QtLogObserver::store_log_message (
      const String &LogName,
      const LogLevelEnum Level,
      const String &Message) {

   if (_state.logWidget) {

      _state.logWidget->store_log_message (LogName, Level, Message);

      if (Level == LogLevelError) {

         //QMessageBox::critical (this, tr ("Error"), Message.get_buffer ());
      }

      if (_state.processUpdates) { qApp->processEvents (); }
   }
}

