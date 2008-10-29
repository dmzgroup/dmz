#include "dmzQtSplashScreen.h"
#include <QtGui/QtGui>
#include <dmzSystemFile.h>

const char LocalSplashPath[] = "./assets/DMZ-Splash.png";

dmz::QtSplashScreen::QtSplashScreen (RuntimeContext *context) :
      QSplashScreen (),
      LogObserver (context) {

   QPixmap splashPixmap (
      dmz::is_valid_path (LocalSplashPath) ? LocalSplashPath : ":/AppQt/DMZ-Splash.png");

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

   if (Message.find_sub ("Loading plugin:", index)) {

      showMessage (Message.get_buffer (), Qt::AlignLeft | Qt::AlignBottom, Qt::white);
   }
}

