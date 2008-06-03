#include <dmzRuntimeData.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeMessaging.h>
#include <dmzTypesString.h>
#include <dmzQtSingletonApplication.h>

struct dmz::QtSingletonApplication::State {

   QString id;
   QString fileName;
   Message msg;
   Handle fileHandle;

   State (const QString &TheID, RuntimeContext *context) : id (TheID), fileHandle (0) {

      Definitions defs (context);

      fileHandle = defs.create_named_handle ("file");
      defs.create_message_type ("DMZ_Open_File_Message", msg);
   }
};


dmz::QtSingletonApplication::QtSingletonApplication (
      const QString &id,
      int argc,
      char **argv,
      RuntimeContext *context) :
      QApplication (argc, argv),
      _state (*(new State (id, context))) {;}


dmz::QtSingletonApplication::~QtSingletonApplication () { delete &_state; }


QString
dmz::QtSingletonApplication::get_requested_file () { return _state.fileName; }


bool
dmz::QtSingletonApplication::start_application () { return true; }


bool
dmz::QtSingletonApplication::send_to_running_application (const QString &Message) { return false; }


bool
dmz::QtSingletonApplication::event (QEvent *event) {

   bool eaten (false);

   switch (event->type ()) {

      case QEvent::FileOpen: {
         _state.fileName = ((QFileOpenEvent *)event)->file ();
         Data value;
         value.store_string (_state.fileHandle, 0, qPrintable (_state.fileName));
         _state.msg.send (&value);
         eaten = true;
         break;
      }
      default:
         eaten = QApplication::event (event);
         break;
   }

   return eaten;
}
