#ifndef DMZ_QT_PLUGIN_RENDER_SCREEN_CAPTURE_DOT_H
#define DMZ_QT_PLUGIN_RENDER_SCREEN_CAPTURE_DOT_H

#include <dmzRuntimeDataConverterTypesBase.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHashTableStringTemplate.h>

#include <QtGui/QFileDialog>
#include <QtGui/QWidget>
#include <ui_ScreenCapturePreview.h>

namespace dmz {

   class QtScreenCaptureSave : public QWidget {

      Q_OBJECT

      public:
         QtScreenCaptureSave (const QString &FileName, const QString &TmpFile, Log &log);
         ~QtScreenCaptureSave ();

         Boolean update ();

         QString get_file_name ();

         void closeEvent (QCloseEvent *);
 
      protected slots:
         void do_accepted ();
         void do_rejected ();
         void on_buttonBox_accepted ();
         void on_buttonBox_rejected ();

      protected:
         const QString _FileName;
         const QString _TmpName;
         Log &_log;
			Ui::ScreenCapturePreview _ui;
         QFileDialog *_qsave;
         Boolean _done;
         Boolean _watchForFile;
         QString _savedFileName;
  };

   class QtPluginRenderScreenCapture :
         public Plugin,
         public TimeSlice,
         public MessageObserver {

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

      protected:
         String _find_target_name ();
         void _init (Config &local);

         Log _log;
         DataConverterString _convert;

         String _savePath;
         String _fileRoot;
         String _fileExt;
         Message _startCaptureMsg;
         Message _screenCaptureMsg;

         HashTableStringTemplate<QtScreenCaptureSave> _saveTable;

      private:
         QtPluginRenderScreenCapture ();
         QtPluginRenderScreenCapture (const QtPluginRenderScreenCapture &);
         QtPluginRenderScreenCapture &operator= (const QtPluginRenderScreenCapture &);

   };
};

#endif // DMZ_QT_PLUGIN_RENDER_SCREEN_CAPTURE_DOT_H
