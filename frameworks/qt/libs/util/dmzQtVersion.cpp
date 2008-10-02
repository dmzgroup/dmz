#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzQtVersion.h>
#include <QtGui/QAction>
#include <QtGui/QPixmap>
#include <ui_dmzQtVersion.h>

#include <qdb.h>
static dmz::qdb out;

struct dmz::QtVersion::State {

   QPixmap pix;
   Ui::QtVersionForm ui;
};

dmz::QtVersion::QtVersion (Config &local, const String &Prefix) :
      QWidget (0, Qt::Dialog),
      _state (*(new State)) {

   _state.ui.setupUi (this);

   const String Name (config_to_string (Prefix + ".name.value", local));
   const String Major (config_to_string (Prefix + ".major.value", local));
   const String Minor (config_to_string (Prefix + ".minor.value", local));
   const String Bug (config_to_string (Prefix + ".bug.value", local));
   const String Build (config_to_string (Prefix + ".build.value", local));
   const String Release (config_to_string (Prefix + ".release.value", local));
   const String Image (config_to_string (Prefix + ".image.value", local));

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

