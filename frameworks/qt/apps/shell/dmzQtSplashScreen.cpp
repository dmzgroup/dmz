#include <dmzApplication.h>
#include "dmzQtSplashScreen.h"
#include <QtGui/QtGui>
#include <dmzSystemFile.h>

const char LocalSplashPath[] = "./assets/DMZ-Splash.png";

dmz::QtSplashScreen *
dmz::create_splash_screen (const Application &App) {

   QtSplashScreen *result (0);

   String fileName ("./assets/");
   fileName << App.get_name () << "-splash.png";

   if (!is_valid_path (fileName)) {

      if (is_valid_path (LocalSplashPath)) { fileName = LocalSplashPath; }
      else { fileName.flush (); }
   }

   if (fileName) {

      QString file (fileName.get_buffer ());
      result = new QtSplashScreen (file, App.get_context ());
   }

   if (result) { result->show (); result->raise (); }

   return result;
}


dmz::QtSplashScreen::QtSplashScreen (QString &FileName, RuntimeContext *context) :
      QSplashScreen (),
      LogObserver (context) {

   QPixmap splashPixmap (FileName);

   setPixmap (splashPixmap);
   setMask (splashPixmap.mask ());
}


dmz::QtSplashScreen::~QtSplashScreen () {

}


// Log Observer Interface
void
dmz::QtSplashScreen::store_log_message (
      const String &LogName,
      const LogLevelEnum Level,
      const String &Message) {

   Int32 index (0);

   if (Message.find_sub ("Created plugin:", index)) {

      showMessage (Message.get_buffer (), Qt::AlignLeft | Qt::AlignBottom, Qt::white);
   }
}

