#ifndef DMZ_QT_PLUGIN_RENDER_SCREEN_CAPTURE_DOT_H
#define DMZ_QT_PLUGIN_RENDER_SCREEN_CAPTURE_DOT_H

#include <dmzRuntimeDataConverterTypesBase.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>

#include <QtGui/QWidget>
#include <ui_ScreenCapturePreview.h>

namespace dmz {

   class QtPluginRenderScreenCapture :
         public QWidget,
         public Plugin,
         public TimeSlice,
         public MessageObserver {

      Q_OBJECT

      public:
         QtPluginRenderScreenCapture (const PluginInfo &Info, Config &local);
         ~QtPluginRenderScreenCapture ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Message Observer Interface
         virtual void receive_message (
            const Message &Type,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

         void closeEvent (QCloseEvent *);

      protected slots:
         void on_buttonBox_accepted ();
         void on_buttonBox_rejected ();

      protected:
         void _init (Config &local);

         Log _log;
         DataConverterString _convert;

         Boolean _watchForFile;
         Boolean _saveFile;
         String _fileExt;
         String _fileName;
         Message _startCaptureMsg;
         Message _screenCaptureMsg;

         Ui::ScreenCapturePreview _ui;

      private:
         QtPluginRenderScreenCapture ();
         QtPluginRenderScreenCapture (const QtPluginRenderScreenCapture &);
         QtPluginRenderScreenCapture &operator= (const QtPluginRenderScreenCapture &);

   };
};

#endif // DMZ_QT_PLUGIN_RENDER_SCREEN_CAPTURE_DOT_H
