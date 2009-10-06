#ifndef DMZ_QT_LOG_OBSERVER_DOT_H
#define DMZ_QT_LOG_OBSERVER_DOT_H

#ifdef _WIN32

#ifdef DMZ_QT_LOG_OBSERVER_EXPORT
#define DMZ_QT_LOG_OBSERVER_LINK_SYMBOL __declspec (dllexport)
#else
#define DMZ_QT_LOG_OBSERVER_LINK_SYMBOL __declspec (dllimport)
#endif // DMZ_QT_LOG_OBSERVER_EXPORT

#else // !_WIN32

#define DMZ_QT_LOG_OBSERVER_LINK_SYMBOL

#endif // _WIN32

#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <QtGui/QWidget>


namespace dmz {

   class QtLogObserverWidget;
   class RuntimeContext;

   class DMZ_QT_LOG_OBSERVER_LINK_SYMBOL QtLogObserver :
         public QWidget,
         protected MessageObserver,
         protected LogObserver {

      Q_OBJECT

      public:
         QtLogObserver (RuntimeContext *context, QWidget *parent = 0);
         ~QtLogObserver ();

          void set_process_updates (const Boolean Value);

          void save_session ();
          void load_session ();

      public slots:
         void toggle_visibility ();

      protected:
         virtual bool eventFilter (QObject *object, QEvent *event);

         virtual void receive_message (
            const Message &Msg,
            const UInt32 MessageSendHandle,
            const UInt32 TargetObserverHandle,
            const Data *InData,
            Data *outData);

         virtual void store_log_message (
            const String &LogName,
            const LogLevelEnum Level,
            const String &Message);

      protected:
         Boolean _processUpdates;
         RuntimeContext *_context;
         QtLogObserverWidget *_logWidget;

      private:
         QtLogObserver ();
         QtLogObserver (const QtLogObserver &);
         QtLogObserver &operator= (const QtLogObserver &);
   };
};

#endif // DMZ_QT_LOG_OBSERVER_DOT_H
