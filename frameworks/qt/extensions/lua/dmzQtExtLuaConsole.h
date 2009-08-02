#ifndef DMZ_QT_EXT_LUA_CONSOLE_DOT_H
#define DMZ_QT_EXT_LUA_CONSOLE_DOT_H

#include <dmzLuaExt.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <QtCore/QHash>
#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextCharFormat>
#include <QtGui/QWidget>

#include <ui_dmzQtLuaConsole.h>

class QTextDocument;

namespace dmz {

   class Highlighter : public QSyntaxHighlighter {

      Q_OBJECT

   public:
      Highlighter(QTextDocument *parent = 0);

   protected:
      void highlightBlock(const QString &text);

   private:
      struct HighlightingRule {

         QRegExp pattern;
         QTextCharFormat format;
      };

      QVector<HighlightingRule> highlightingRules;

      QRegExp commentStartExpression;
      QRegExp commentEndExpression;

      QTextCharFormat keywordFormat;
      QTextCharFormat funcFormat;
      QTextCharFormat singleLineCommentFormat;
      QTextCharFormat multiLineCommentFormat;
      QTextCharFormat quotationFormat;
      QTextCharFormat squotationFormat;
      QTextCharFormat numberFormat;
   };

   class QtExtLuaConsole :
         public QWidget,
         public Plugin,
         public MessageObserver,
         public LogObserver,
         public LuaExt {

      Q_OBJECT

      public:
         QtExtLuaConsole (const PluginInfo &Info, Config &local);
         ~QtExtLuaConsole ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // MessageObserver Interface
         virtual void receive_message (
            const Message &Type,
            const Handle MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

         // LogObserver Interface
         virtual void store_log_message (
            const String &LogName,
            const LogLevelEnum Level,
            const String &Message);

         // LuaExt Interface
         virtual void store_lua_module (LuaModule &module);
         virtual void open_lua_extension (lua_State *luaState);
         virtual void close_lua_extension (lua_State *luaState);
         virtual void remove_lua_module (LuaModule &module);

         // QtExtLuaConsole Inteface
         void append (const String &Str);

      protected slots:
         void on_resetButton_clicked ();
         void on_executeButton_clicked ();
         void trigger_prev_history ();
         void trigger_next_history ();

      protected:
         struct HistoryStruct {

            HistoryStruct *prev;
            HistoryStruct *next;
            const QString Value;

            HistoryStruct (const QString &TheValue) :
                  Value (TheValue),
                  prev (0),
                  next (0) {;}

            ~HistoryStruct () { if (next) { delete next; next = 0; } }
         };

         void _add_history (const QString &Value);
         void _init (Config &local);

         QColor _defaultColor;
         QString _currentBuffer;
         LuaModule *_luaMod;
         lua_State *L;
         Message _showMsg;
         Ui::LuaConsole _console;
         Highlighter _hl;
         Int32 _historyCount;
         HistoryStruct *_historyCurrent;
         HistoryStruct *_historyHead;
         HistoryStruct *_historyTail;
         Log _log;

      private:
         QtExtLuaConsole ();
         QtExtLuaConsole (const QtExtLuaConsole &);
         QtExtLuaConsole &operator= (const QtExtLuaConsole &);

   };
};

#endif // DMZ_QT_EXT_LUA_CONSOLE_DOT_H
