#ifndef DMZ_QT_PLUGIN_BUTTON_TO_CHANNEL_DOT_H
#define DMZ_QT_PLUGIN_BUTTON_TO_CHANNEL_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzQtWidget.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <QtGui/QFrame>

class QAction;
class QActionGroup;
class QDockWidget;


namespace dmz {

   class InputModule;

   class QtPluginButtonToChannel :
         public QFrame,
         public Plugin,
         public QtWidget,
         public InputObserverUtil {

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

         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State);

         virtual void receive_axis_event (
            const Handle Channel,
            const InputEventAxis &Value) {;}

         virtual void receive_button_event (
            const Handle Channel,
            const InputEventButton &Value) {;}

         virtual void receive_key_event (
            const Handle Channel,
            const InputEventKey &Value) {;}

         virtual void receive_mouse_event (
            const Handle Channel,
            const InputEventMouse &Value) {;}

         virtual void receive_switch_event (
            const Handle Channel,
            const InputEventSwitch &Value) {;}

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

      private:
         QtPluginButtonToChannel ();
         QtPluginButtonToChannel (const QtPluginButtonToChannel &);
         QtPluginButtonToChannel &operator= (const QtPluginButtonToChannel &);

   };
};

#endif // DMZ_QT_PLUGIN_BUTTON_TO_CHANNEL_DOT_H
