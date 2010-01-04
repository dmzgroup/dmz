#include <dmzFoundationBase64.h>
#include <dmzLuaKernelValidate.h>
#include <dmzLuaModule.h>
#include "dmzQtExtLuaConsole.h"
#include <dmzQtUtil.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigWrite.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <luacpp.h>
#include <QtGui/QAction>

// This header needs to go here in order to compile -rb
#include "dmzLuaFunctions.h"

using namespace dmz;

namespace {

static const char ConsoleKey = 'c';

static void
set_console (lua_State *L, QtExtLuaConsole *console) {

   LUA_START_VALIDATE (L);

   lua_pushlightuserdata (L, (void *)&ConsoleKey);
   lua_rawget (L, LUA_REGISTRYINDEX);
   QtExtLuaConsole **ptr = (QtExtLuaConsole **)lua_touserdata (L, -1);
   lua_pop (L, 1);

   if (!ptr) {

      lua_pushlightuserdata (L, (void *)&ConsoleKey);
      ptr = (QtExtLuaConsole **)lua_newuserdata (L, sizeof (QtExtLuaConsole *));
      if (ptr) { lua_rawset (L, LUA_REGISTRYINDEX); }
      *ptr = 0;
   }

   if (ptr) { *ptr = console; }

   LUA_END_VALIDATE (L, 0);
}


static QtExtLuaConsole *
get_console (lua_State *L) {

   QtExtLuaConsole *result (0);
   lua_pushlightuserdata (L, (void *)&ConsoleKey);
   lua_rawget (L, LUA_REGISTRYINDEX);
   QtExtLuaConsole **ptr = (QtExtLuaConsole **)lua_touserdata (L, -1);
   lua_pop (L, 1);
   if (ptr) { result = *ptr; }
   return result;
}


static int
cprint (lua_State *L) {

   String value;

   int Top = lua_gettop (L);

   for (int ix = 1; ix <= Top; ix++) {

      const char *ptr = lua_tostring (L, ix);

      if (ix != 1) { value << " "; }

      if (ptr) { value << ptr; }
      else {

         if (lua_getmetatable (L, ix)) {

            lua_getfield (L, -1, "__tostring");

            if (lua_isfunction (L, -1)) {

               lua_pushvalue (L, ix);

               lua_call (L, 1, 1);

               if (lua_isstring (L, -1)) {

                  value << lua_tostring (L, -1);
               }

              lua_pop (L, 2);
            }
            else { value << "<UNPRINTABLE>"; }
         }
      }
   }

   QtExtLuaConsole *c (get_console (L));

   if (c) {

      c->append (value);
   }

   return 0;
}

}

dmz::Highlighter::Highlighter (QTextDocument *parent) : QSyntaxHighlighter (parent) {

   HighlightingRule rule;


   keywordFormat.setForeground (Qt::blue);
   keywordFormat.setFontWeight (QFont::Bold);
   QStringList keywordPatterns;

   keywordPatterns
      << "\\band\\b"
      << "\\bbreak\\b"
      << "\\bdo\\b"
      << "\\belse\\b"
      << "\\belseif\\b"
      << "\\bend\\b"
      << "\\bfalse\\b"
      << "\\bfor\\b"
      << "\\bfunction\\b"
      << "\\bif\\b"
      << "\\bin\\b"
      << "\\blocal\\b"
      << "\\bnil\\b"
      << "\\bnot\\b"
      << "\\bor\\b"
      << "\\brepeat\\b"
      << "\\breturn\\b"
      << "\\bthen\\b"
      << "\\btrue\\b"
      << "\\buntil\\b"
      << "\\bwhile\\b";

   foreach (QString pattern, keywordPatterns) {

      rule.pattern = QRegExp (pattern);
      rule.format = keywordFormat;
      highlightingRules.append (rule);
   }

   funcFormat.setForeground (Qt::darkGreen);
   funcFormat.setFontWeight (QFont::Bold);
   QStringList funcPatterns;

   set_lua_functions (funcPatterns);

   foreach (QString pattern, funcPatterns) {

      rule.pattern = QRegExp (pattern);
      rule.format = funcFormat;
      highlightingRules.append (rule);
   }

   singleLineCommentFormat.setForeground (Qt::darkGray);
   rule.pattern = QRegExp ("--[^\\[\\]][^\n]*");
   rule.format = singleLineCommentFormat;
   highlightingRules.append (rule);

   quotationFormat.setForeground (Qt::red);
   rule.pattern = QRegExp ("\"[^\"]*\"");
   rule.format = quotationFormat;
   highlightingRules.append (rule);

   squotationFormat.setForeground (Qt::red);
   rule.pattern = QRegExp ("'[^']*'");
   rule.format = squotationFormat;
   highlightingRules.append (rule);

   numberFormat.setForeground (Qt::red);
   numberFormat.setFontWeight (QFont::Bold);

   rule.pattern = QRegExp ("[-]?\\d{1,}[\\.]?\\d*[eE]{1}[-]?\\d{1,}");
   rule.format = numberFormat;
   highlightingRules.append (rule);

   rule.pattern = QRegExp ("[-]?\\d{1,}[\\.]?\\d*");
   rule.format = numberFormat;
   highlightingRules.append (rule);

   commentStartExpression = QRegExp ("--\\[([=]*)\\[");
   multiLineCommentFormat.setForeground (Qt::darkGray);
}


void
dmz::Highlighter::highlightBlock (const QString &text) {

   foreach (HighlightingRule rule, highlightingRules) {

      QRegExp expression (rule.pattern);
      int index = text.indexOf (expression);

      while (index >= 0) {

         int length = expression.matchedLength ();
         setFormat (index, length, rule.format);
         index = text.indexOf (expression, index + length);
      }
   }

   setCurrentBlockState (0);

   int startIndex = 0;

   if (previousBlockState () != 1) {

       startIndex = text.indexOf (commentStartExpression);
       QString comment ("--\\]");
       comment += commentStartExpression.cap (1) + "\\]";
       commentEndExpression = QRegExp (comment);
   }

   while (startIndex >= 0) {

      int endIndex = text.indexOf (commentEndExpression, startIndex);
      int commentLength;

      if (endIndex == -1) {

         setCurrentBlockState (1);
         commentLength = text.length () - startIndex;
      }
      else {

         commentLength = endIndex - startIndex
            + commentEndExpression.matchedLength ();
      }

      setFormat (startIndex, commentLength, multiLineCommentFormat);
      startIndex = text.indexOf (commentStartExpression, startIndex + commentLength);
   }
}


dmz::QtExtLuaConsole::QtExtLuaConsole (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      LogObserver (Info.get_context ()),
      MessageObserver (Info),
      LuaExt (Info),
      L (0),
      _luaMod (0),
      _hl (0),
      _historyCount (0),
      _historyCurrent (0),
      _historyHead (0),
      _historyTail (0),
      _log (Info) {

   detach_log_observer ();

   _console.setupUi (this);

   _hl.setDocument (_console.inputBox->document ());

   QAction *executeAct = new QAction(this);
   executeAct->setShortcut(Qt::Key_Return + Qt::CTRL);

   QAction *prevAct = new QAction(this);
   prevAct->setShortcut(Qt::Key_K + Qt::CTRL);

   QAction *nextAct = new QAction(this);
   nextAct->setShortcut(Qt::Key_J + Qt::CTRL);

   connect(executeAct, SIGNAL (triggered ()), this, SLOT (on_executeButton_clicked ()));
   connect(prevAct, SIGNAL (triggered ()), this, SLOT (trigger_prev_history ()));
   connect(nextAct, SIGNAL (triggered ()), this, SLOT (trigger_next_history ()));

   _console.inputBox->addAction (executeAct);
   _console.inputBox->addAction (prevAct);
   _console.inputBox->addAction (nextAct);

   _defaultColor = _console.displayBox->textColor ();
   _init (local);
   
   adjustSize ();
}


dmz::QtExtLuaConsole::~QtExtLuaConsole () {

   if (_historyHead) {

      delete _historyHead; _historyHead = _historyTail = _historyCurrent = 0;
   }
}


// Plugin Interface
void
dmz::QtExtLuaConsole::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateShutdown) {

      if (L && (this == get_console (L))) { set_console (L, 0); }

      RuntimeContext *context (get_plugin_runtime_context ());

      if (context) {

         String data;

         Config session (get_plugin_name ());

         session.add_config (qbytearray_to_config ("geometry", saveGeometry ()));

         session.add_config (
            qbytearray_to_config ("splitter", _console.splitter->saveState ()));

         if (isVisible ()) {

            session.add_config (boolean_to_config ("window", "visible", True));
         }

         HistoryStruct *current = _historyHead;

         Int32 count = 0;

         while (current && (count < 100)) {

            Config history ("history");

            history.store_attribute (
               "value",
                encode_base64 (qPrintable (current->Value)));

            session.add_config (history);
            current = current->next;
            count++;
         }

         set_session_config (context, session);
      }
   }
}


// MessageObserver Interface
void
dmz::QtExtLuaConsole::receive_message (
      const Message &Type,
      const Handle MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   if (Type == _showMsg) { show (); activateWindow (); }
}


// LogObserver Interface
void
dmz::QtExtLuaConsole::store_log_message (
      const String &LogName,
      const LogLevelEnum Level,
      const String &Message) {

   String out;
   if (LogName) { out << LogName << ":"; }
   out << Message;

   if (Level == LogLevelError) {

      _console.displayBox->setTextColor (QColor ("red"));
   }
   else if (Level == LogLevelWarn) {

      _console.displayBox->setTextColor (QColor ("orange"));
   }

   append (out);

   _console.displayBox->setTextColor (_defaultColor);
}


// LuaExt Interface
void
dmz::QtExtLuaConsole::store_lua_module (LuaModule &module) { _luaMod = &module; }


void
dmz::QtExtLuaConsole::open_lua_extension (lua_State *luaState) {

   L = luaState;

   if (L) {

      set_console (L, this);
      lua_pushcfunction (L, cprint);
      lua_setglobal (L, "cprint");

      lua_newtable (L); // Create function env table
      lua_newtable (L); // Create meta table
      lua_getglobal (L, "_G"); // Get global table
      lua_setfield (L, -2, "__index"); // set __index == _G in meta table
      lua_setmetatable (L, -2); // set meta table in function table
      // store function table in global table
      lua_setglobal (L, get_plugin_name ().get_buffer ());
   }
}


void
dmz::QtExtLuaConsole::close_lua_extension (lua_State *luaState) { L = 0; }


void
dmz::QtExtLuaConsole::remove_lua_module (LuaModule &module) { _luaMod = 0; }


// QtExtLuaConsole Inteface
void
dmz::QtExtLuaConsole::append (const String &Str) {

   if (!Str.is_null ()) { _console.displayBox->append (Str.get_buffer ()); }
}

void
dmz::QtExtLuaConsole::on_resetButton_clicked () {

   if (_luaMod) { _luaMod->reset_lua (); }
}


void
dmz::QtExtLuaConsole::on_executeButton_clicked () {

   QString str (_console.inputBox->toPlainText ());

   if (str.trimmed ().isEmpty ()) { _console.inputBox->clear (); }
   else if (L) {

      const int Top = lua_gettop (L);

      Boolean error (False);
      set_console (L, this);

      if (!luaL_loadstring (L, qPrintable (str))) {

         _console.displayBox->setTextColor (QColor ("blue"));
         _console.displayBox->append (str);

         _console.displayBox->setTextColor (_defaultColor);

         lua_getglobal (L, get_plugin_name ().get_buffer ());
         lua_setfenv (L, -2); // set function env table

         attach_log_observer ();

         if (!lua_pcall (L, 0, LUA_MULTRET, 0)) {

            _console.inputBox->clear ();
            const int NewTop = lua_gettop (L);

            _currentBuffer.clear ();
            _add_history (str);

            if (NewTop > Top) { lua_pop (L, NewTop - Top); }
         }
         else { error = True; }

         detach_log_observer ();
      }
      else { error = True; }

      if (error)  {

         QString error = lua_tostring (L, -1);

         lua_pop (L, 1);

         _console.displayBox->setTextColor (QColor ("red"));
         _console.displayBox->append (error);
         _console.displayBox->setTextColor (_defaultColor);
      }
   }
}


void
dmz::QtExtLuaConsole::trigger_prev_history () {

   Boolean set (False);

   if (!_historyCurrent) {

      _currentBuffer = _console.inputBox->toPlainText ();
      _historyCurrent = _historyHead; set = True;
   }
   else if (_historyCurrent->next) {

      _historyCurrent = _historyCurrent->next;
      set = True;
   }

   if (set && _historyCurrent) {

      _console.inputBox->setPlainText (_historyCurrent->Value);
   }
}


void
dmz::QtExtLuaConsole::trigger_next_history () {

   Boolean wasCurrent = False;

   if (_historyCurrent) { _historyCurrent = _historyCurrent->prev; wasCurrent = True; }

   if (_historyCurrent) {

      _console.inputBox->setPlainText (_historyCurrent->Value);
   }
   else if (!_currentBuffer.isEmpty () || wasCurrent) {

      _console.inputBox->setPlainText (_currentBuffer);
      _currentBuffer.clear ();
   }
}


void
dmz::QtExtLuaConsole::_add_history (const QString &Value) {

   const QString Trimmed = Value.trimmed ();

   if (!Trimmed.isEmpty ()) {

      _historyCurrent = 0;

      if (!_historyHead) {

         _historyHead = _historyTail = new HistoryStruct (Value);
         if (_historyHead) { _historyCount = 1; }
      }
      else if (_historyHead->Value != Value) {

         HistoryStruct *hs (new HistoryStruct (Value));

         if (hs) {

            hs->next = _historyHead;
            _historyHead->prev = hs;
            _historyHead = hs;
            _historyCount++;
         }
      }
   }
}


void
dmz::QtExtLuaConsole::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   _showMsg = config_create_message (
      "show.name",
      local,
      "DMZ_Show_Lua_Console",
      context);

   subscribe_to_message (_showMsg);

   if (context) {

      Config session (get_session_config (get_plugin_name (), context));

      QByteArray geometry (config_to_qbytearray ("geometry", session, saveGeometry ()));
      restoreGeometry (geometry);

      geometry = config_to_qbytearray ("splitter", session);

      if (!geometry.isEmpty ()) { _console.splitter->restoreState (geometry); }

      if (config_to_boolean ("window.visible", session, False)) { show (); }

      Config historyList;

      if (session.lookup_all_config ("history", historyList)) {

         ConfigIterator it;
         Config history;

         Boolean found (historyList.get_last_config (it, history));

         while (found) {

            const String Value = decode_base64 (config_to_string ("value", history));
            _add_history (Value.get_buffer ());
            found = historyList.get_prev_config (it, history);
         }
      }
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtExtLuaConsole (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtExtLuaConsole (Info, local);
}

};
