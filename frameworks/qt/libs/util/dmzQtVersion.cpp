#include <dmzQtVersion.h>
#include <dmzRuntimeVersion.h>
#include <QtGui/QAction>
#include <QtGui/QPixmap>
#include <ui_dmzQtVersion.h>

#include <qdb.h>
static dmz::qdb out;

struct dmz::QtVersion::State {

   QPixmap pix;
   Ui::QtVersionForm ui;
   Version version;

   State (const Config &Source, const String &Prefix) : version (Source, Prefix) {;}
};

dmz::QtVersion::QtVersion (Config &local, const String &Prefix) :
      QWidget (0, Qt::Dialog),
      _state (*(new State (local, Prefix))) {

   _state.ui.setupUi (this);

   const String Name (_state.version.get_name ());
   const String Major (_state.version.get_major ());
   const String Minor (_state.version.get_minor ());
   const String Bug (_state.version.get_bug ());
   const String Build (_state.version.get_build ());
   const String Release (_state.version.get_release ());
   const String Image (_state.version.get_image_name ());

   if (Image) {

      _state.pix.load (Image.get_buffer ());

      if (!_state.pix.isNull ()) {

         _state.ui.imageLabel->setPixmap (_state.pix);
      }
      else { out << "**** Unable to load about image: " << Image << endl; }
   }

   if (Name) { _state.ui.nameLabel->setText (Name.get_buffer ()); }

   if (Major) {

      String value (Major);
      if (Minor) {

         value << "." << Minor;

         if (Bug) { value << "." << Bug; }
      }

      if (Release) { value << " " << Release; }

      _state.ui.versionLabel->setText (value.get_buffer ());
   }

   if (Build) { _state.ui.buildLabel->setText (Build.get_buffer ()); }

   // hit Ctrl+V to display a aboutQt message box
   QAction *action = new QAction (this);
   action->setShortcut (Qt::CTRL + Qt::Key_V);
   addAction (action);
   connect (action, SIGNAL (triggered ()), qApp, SLOT (aboutQt ()));
}


dmz::QtVersion::~QtVersion () { delete &_state; }

