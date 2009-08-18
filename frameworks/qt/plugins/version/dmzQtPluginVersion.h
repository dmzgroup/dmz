#ifndef DMZ_QT_PLUGIN_VERSION_DOT_H
#define DMZ_QT_PLUGIN_VERSION_DOT_H

#include <dmzRuntimeConfig.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzQtVersion.h>
#include <QtCore/QObject>

namespace dmz {

   class QtPluginVersion :
         public QObject,
         public Plugin {

      Q_OBJECT

      public:
         QtPluginVersion (const PluginInfo &Info, Config &local, Config &global);
         ~QtPluginVersion ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

      protected slots:
         void _slot_about ();

      protected:
         void _init (Config &local);

         Config _global;
         Log _log;

         QtVersion *_version;
         QAction *_aboutAction;

      private:
         QtPluginVersion ();
         QtPluginVersion (const QtPluginVersion &);
         QtPluginVersion &operator= (const QtPluginVersion &);

   };
};

#endif // DMZ_QT_PLUGIN_VERSION_DOT_H
