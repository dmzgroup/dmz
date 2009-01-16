#ifndef DMZ_QT_PLUGIN_QUIT_DOT_H
#define DMZ_QT_PLUGIN_QUIT_DOT_H

#include <dmzRuntimeExit.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

#include <QtCore/QObject>

namespace dmz {

   class QtPluginQuit :
         public QObject,
         public Plugin {

      Q_OBJECT

      public:
         QtPluginQuit (const PluginInfo &Info, Config &local);
         ~QtPluginQuit ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

      protected slots:
         void _slot_quit ();

      protected:
         void _init (Config &local);

         Log _log;
         Exit _exit;

      private:
         QtPluginQuit ();
         QtPluginQuit (const QtPluginQuit &);
         QtPluginQuit &operator= (const QtPluginQuit &);
   };
};

#endif // DMZ_QT_PLUGIN_QUIT_DOT_H
