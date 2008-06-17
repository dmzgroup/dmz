#include <dmzRuntimeData.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeMessaging.h>
#include <dmzTypesString.h>
#include <dmzQtSingletonApplication.h>
#include <windows.h>

namespace {

static const QString NameAppendix ("_dmzQtSingletonApplicationWindow");

class winEventClass : public QWidget {

   public:
      const QString Name;
      dmz::Message msg;
      dmz::Handle fileHandle;

      winEventClass (dmz::RuntimeContext *context) :
            QWidget (),
            fileHandle (0) {

         dmz::Definitions defs (context);
         fileHandle = defs.create_named_handle ("file");
         defs.create_message_type ("DMZ_Open_File_Message", msg);
      }

      bool winEvent (MSG *winMsg, long *value) {

         bool result (false);

         if (winMsg->message == WM_COPYDATA) {

            if (winMsg->hwnd == winId()) {

               COPYDATASTRUCT *data = (COPYDATASTRUCT*)winMsg->lParam;
               const dmz::String FileName ((char*)data->lpData);

               if (FileName) {

                  dmz::Data data;
                  data.store_string (fileHandle, 0, FileName);
                  msg.send (&data);
               }
            }

            result = true;
         }

         return result;
      }
}; 

static HANDLE
local_lock (const QString &Name) {

   HANDLE result (CreateMutex (0, FALSE, qPrintable (Name)));

   DWORD value (WaitForSingleObject (result, INFINITE));

   if ((value == WAIT_ABANDONED) || (value == WAIT_TIMEOUT)) {

      CloseHandle (result); result = 0;
   }

   return result;
}

static void
local_unlock (HANDLE handle) {

   if (handle) {

      ReleaseMutex (handle);
      CloseHandle (handle);
   }
}


static HWND
local_find_app (const QString &Name) {

   HWND result (0);

   HANDLE mutex (local_lock (Name));

   if (mutex) {

      const QString WinName (Name + NameAppendix);

      result = ::FindWindow ("QWidget", qPrintable (WinName));

      local_unlock (mutex);
   }

   return result;
}

};


struct dmz::QtSingletonApplication::State {

   QString name;
   RuntimeContext *context;
   winEventClass *wec;

   State (const QString &TheID, RuntimeContext *theContext) :
         name (TheID),
         context (theContext),
         wec (0) {;}

   ~State () { if (wec) { delete wec; wec = 0; } }
};


dmz::QtSingletonApplication::QtSingletonApplication (
      const QString &name,
      int argc,
      char **argv,
      RuntimeContext *context) :
      QApplication (argc, argv),
      _state (*(new State (name, context))) {;}


dmz::QtSingletonApplication::~QtSingletonApplication () { delete &_state; }


QString
dmz::QtSingletonApplication::get_requested_file () { return ""; }


bool
dmz::QtSingletonApplication::start_application () {

   bool result (false);

   if (_state.wec) { result = true; }
   else if (local_find_app (_state.name) == 0) {

      HANDLE mutex = local_lock (_state.name);
      _state.wec = new winEventClass (_state.context);
      if (_state.wec) { _state.wec->setWindowTitle (_state.name + NameAppendix); }
      local_unlock (mutex);
      result = true;
   }

   return result;
}


bool
dmz::QtSingletonApplication::send_to_running_application (const QString &Message) {

   bool result (false);

   HWND hwnd = local_find_app (_state.name);

   if (hwnd) {

      COPYDATASTRUCT data;
      data.dwData = 0;
      data.cbData = (Message.length()+1) * sizeof(QChar);
      data.lpData = (void*)qPrintable (Message);
      DWORD value (0);

      if (SendMessageTimeout (
            hwnd,
            WM_COPYDATA,
            0,
            (LPARAM)&data,
            SMTO_ABORTIFHUNG,
            15000,
            &value) != 0) {

         result = true;
      }
   }

   return result;
}


bool
dmz::QtSingletonApplication::event (QEvent *event) { return QApplication::event (event); }
