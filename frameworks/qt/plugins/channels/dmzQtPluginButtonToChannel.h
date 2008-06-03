#ifndef DMZ_QT_PLUGIN_BUTTON_TO_CHANNEL_DOT_H
#define DMZ_QT_PLUGIN_BUTTON_TO_CHANNEL_DOT_H

#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <QtGui/QWidget>

class QAction;
class QActionGroup;
class QDockWidget;


namespace dmz {

   class InputModule;
   class QtModuleMainWindow;


   class QtPluginButtonToChannel : public QWidget, public Plugin {

      Q_OBJECT
      
      public:
         QtPluginButtonToChannel (const PluginInfo &Info, Config &local);
         ~QtPluginButtonToChannel ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

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
         
         void _save_session ();
         void _load_session ();
         void _init (Config &local);
         
         Log _log;
         Definitions _defs;
         InputModule *_inputModule;
         String _inputModuleName;
         QtModuleMainWindow *_mainWindowModule;
         String _mainWindowModuleName;
         Handle _channel;
         QString _dockWidgetTitle;
         QDockWidget *_dock;
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
