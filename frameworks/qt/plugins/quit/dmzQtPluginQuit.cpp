#include "dmzQtPluginQuit.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <QtGui/QApplication.h>
#include <QtGui/QMenuBar.h>
#include <QtGui/QAction.h>

dmz::QtPluginQuit::QtPluginQuit (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      _log (Info),
      _exit (Info) {

   setObjectName (Info.get_name ().get_buffer ());

   QMenuBar *bar = new QMenuBar (0);
   QAction *ac = new QAction (tr ("E&xit"), this);
   //ac->setShortcut (tr ("Ctrl+Q"));
   connect (ac, SIGNAL (triggered ()), this, SLOT (_slot_quit ()));
   
   bar->addMenu (tr ("&File"))->addAction (ac);

   _init (local);
}


dmz::QtPluginQuit::~QtPluginQuit () {

}


void
dmz::QtPluginQuit::_slot_quit () {

   _log.error << "About To Quit!" << endl;
   _exit.request_exit (ExitStatusNormal, "Application Quit.");
}


void
dmz::QtPluginQuit::_init (Config &local) {

   connect (QCoreApplication::instance(), SIGNAL(aboutToQuit ()), this, SLOT(_slot_quit ()));
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginQuit (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginQuit (Info, local);
}

};
