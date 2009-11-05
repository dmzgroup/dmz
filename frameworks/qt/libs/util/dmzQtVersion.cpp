#include <dmzQtVersion.h>
#include <dmzRuntimeVersion.h>
#include <QtGui/QAction>
#include <QtGui/QPixmap>
#include <ui_dmzQtVersion.h>


struct dmz::QtVersion::State {

   QPixmap pix;
   Ui::QtVersionForm ui;
   Version version;

   State (const Config &Source, const String &Prefix) : version (Source, Prefix) {;}
};


dmz::QtVersion::QtVersion (QWidget *parent, Config &local, const String &Prefix) :
      QDialog (parent),
      _state (*(new State (local, Prefix))) {

   _state.ui.setupUi (this);

   const String Name (_state.version.get_name ());
   const String Build (_state.version.get_build ());
   const String Image (_state.version.get_image_name ());
   const String Version (_state.version.get_version ());

   if (Image) {

      _state.pix.load (Image.get_buffer ());
      if (!_state.pix.isNull ()) { _state.ui.imageLabel->setPixmap (_state.pix); }
   }

   if (Name) { _state.ui.nameLabel->setText (Name.get_buffer ()); }
   if (Version) { _state.ui.versionLabel->setText (Version.get_buffer ()); }
   if (Build) { _state.ui.buildLabel->setText (Build.get_buffer ()); }

   // hit Ctrl+V to display a aboutQt message box
   QAction *action = new QAction (this);
   action->setShortcut (Qt::CTRL + Qt::Key_V);
   addAction (action);
   connect (action, SIGNAL (triggered ()), qApp, SLOT (aboutQt ()));
}


dmz::QtVersion::~QtVersion () { delete &_state; }

