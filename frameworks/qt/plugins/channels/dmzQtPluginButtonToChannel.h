#ifndef DMZ_QT_PLUGIN_BUTTON_TO_CHANNEL_DOT_H
#define DMZ_QT_PLUGIN_BUTTON_TO_CHANNEL_DOT_H

#include <dmzQtWidget.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <QtGui/QWidget>

class QAction;
class QActionGroup;
class QDockWidget;


namespace dmz {

   class InputModule;

   class QtPluginButtonToChannel : public QWidget, public Plugin, public QtWidget {

      Q_OBJECT

      public:
         QtPluginButtonToChannel (const PluginInfo &Info, Config &local);
         ~QtPluginButtonToChannel ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // QtWidget Interface
         virtual QWidget *get_qt_widget ();

      protected slots:
         void _slot_change_channel (QAction *action);

      protected:
         struct ChannelStruct {

            const Handle Channel;
            QAction *action;
            ChannelStruct *next;

            ChannelStruct (const Handle TheHandle) :
                  Channel (TheHandle),
                  action (0),
                  next (0) {;}

            ~ChannelStruct () { if (next) { delete next; next = 0; } }
         };

         void _init (Config &local);

         Log _log;
         Definitions _defs;
         InputModule *_inputModule;
         String _inputModuleName;
         QActionGroup *_actionGroup;
         ChannelStruct *_channelList;
         Handle _defaultChannel;

      private:
         QtPluginButtonToChannel ();
         QtPluginButtonToChannel (const QtPluginButtonToChannel &);
         QtPluginButtonToChannel &operator= (const QtPluginButtonToChannel &);

   };
};

#endif // DMZ_QT_PLUGIN_BUTTON_TO_CHANNEL_DOT_H
