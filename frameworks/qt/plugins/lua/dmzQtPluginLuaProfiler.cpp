#include <dmzLuaModule.h>
#include "dmzQtLuaProfilerTable.h"
#include "dmzQtPluginLuaProfiler.h"
#include <dmzQtConfigRead.h>
#include <dmzQtConfigWrite.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigWrite.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <luacpp.h>
#include <QtGui/QTableWidget>
#include <QtGui/QTableWidgetItem>

namespace {

static const QString LocalStart ("Start");
static const QString LocalStop ("Stop");

};

dmz::QtPluginLuaProfiler::QtPluginLuaProfiler (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      TimeSlice (Info, TimeSliceTypeSystemTime, TimeSliceModeSingle, 30.0),
      MessageObserver (Info),
      LuaObserver (Info),
      _log (Info),
      _tableCount (0),
      _hookCount (0.0),
      _profiling (False),
      _inCountUpdate (False),
      _luaMod (0) {

   _ui.setupUi (this);
   _ui.startButton->setEnabled (false);
   _ui.startButton->setText (LocalStart);
   _ui.timeSpinBox->setEnabled (false);
   _ui.timeSpinBox->setValue ((int)get_time_slice_interval ());
   _ui.stepsSpinBox->setEnabled (false);

   _init (local);
}


dmz::QtPluginLuaProfiler::~QtPluginLuaProfiler () {

   _statsTable.empty ();
}


// Plugin Interface
void
dmz::QtPluginLuaProfiler::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateShutdown) {

      RuntimeContext *context (get_plugin_runtime_context ());

      if (context) {

         String data;

         Config session (get_plugin_name ());

         session.add_config (qbytearray_to_config ("geometry", saveGeometry ()));

         if (isVisible ()) {

            session.add_config (boolean_to_config ("window", "visible", True));
         }

         set_session_config (context, session);
      }
   }
}


// TimeSlice Interface
void
dmz::QtPluginLuaProfiler::update_time_slice (const Float64 TimeDelta) {

   if (_profiling) { on_startButton_clicked (); }
}


// Message Observer Interface
void
dmz::QtPluginLuaProfiler::receive_message (
      const Message &Msg,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   if (Msg == _showMsg) { show (); activateWindow (); }
}


// LuaObserver Interface
void
dmz::QtPluginLuaProfiler::store_lua_module (const String &ModuleName, LuaModule &module) {

   if (!_luaModuleName || (ModuleName == _luaModuleName)) {

      _ui.startButton->setEnabled (true);
      _ui.timeSpinBox->setEnabled (true);
      _ui.stepsSpinBox->setEnabled (true);
      _luaMod = &module;
      module.register_lua_observer (LuaHookCountUpdateMask, *this);
   }
}


void
dmz::QtPluginLuaProfiler::remove_lua_module (
      const String &ModuleName,
      LuaModule &module) {

   if (_luaMod && (_luaMod == &module)) {

      _ui.startButton->setEnabled (false);
      _ui.timeSpinBox->setEnabled (false);
      _ui.stepsSpinBox->setEnabled (false);
      stop_time_slice ();
      _luaMod = 0;
   }
}


void
dmz::QtPluginLuaProfiler::update_lua_hook_count (lua_State *L, const Int32 Count) {

   if (!_inCountUpdate) {

      _inCountUpdate = True;
      _ui.stepsSpinBox->setValue ((int)Count);
      _inCountUpdate = False;
   }
}


void
dmz::QtPluginLuaProfiler::lua_hook (lua_State *L, lua_Debug *ar) {

   _hookCount += 1.0;

   if (lua_getinfo (L, "Sn", ar)) {

      if (ar->name && ar->source) {

         String Name (ar->name);
         String File (ar->source[0] == '@' ? &(ar->source[1]) : ar->source);
         StatsStruct *ss (_statsTable.lookup (Name + File));

         if (!ss) {

            ss = new StatsStruct;
            if (ss) { ss->func = Name; ss->file = File; }
            if (!_statsTable.store (Name + File, ss)) { delete ss; ss = 0; }
         }

         if (ss) { ss->count += 1.0; }
      }
   }
}


void
dmz::QtPluginLuaProfiler::on_startButton_clicked () {

   if (_luaMod) {

      if (_profiling) {

         _ui.startButton->setText (LocalStart);
         _ui.timeSpinBox->setEnabled (true);
         _ui.stepsSpinBox->setEnabled (true);
         _profiling = False;
         _luaMod->release_lua_observer (LuaHookCountMask, *this);
         stop_time_slice ();
         _create_profile_table ();
      }
      else {

         _ui.startButton->setText (LocalStop);
         _ui.timeSpinBox->setEnabled (false);
         _ui.stepsSpinBox->setEnabled (false);
         _profiling = True;
         _luaMod->register_lua_observer (LuaHookCountMask, *this);
         _hookCount = 0.0;
         _statsTable.empty ();
         start_time_slice ();
      }
   }
}


void
dmz::QtPluginLuaProfiler::on_timeSpinBox_valueChanged (int value) {

   set_time_slice_interval ((Float64)value);
}


void
dmz::QtPluginLuaProfiler::on_stepsSpinBox_valueChanged (int value) {

   if (_luaMod) { _luaMod->set_hook_count ((Int32)value); }
}


void
dmz::QtPluginLuaProfiler::_create_profile_table () {

   if (is_zero64 (_hookCount)) { _hookCount = 1.0; }

   HashTableStringIterator it;
   StatsStruct *ss (_statsTable.get_first (it));

   while (ss) {

      _log.info << ss->count / _hookCount * 100 << "% " << ss->func << " : "
         << ss->file << endl;
      ss = _statsTable.get_next (it);
   }

   QtLuaProfilerTable *table = new QtLuaProfilerTable;
   QString title ("Lua Profiler Results #");
   _tableCount++;
   title += QString::number (_tableCount);
   table->setWindowTitle (title);
   table->ui.samplesLabel->setText (QString::number (_hookCount));
   table->show ();
   table->activateWindow ();

   if (table) {

      int count (0);
      HashTableStringIterator it;
      StatsStruct *ss (_statsTable.get_first (it));

      if (is_zero64 (_hookCount)) { _hookCount = 1.0; }

      while (ss) {

         QStandardItem *c0 (new QStandardItem);

         if (c0) {

            c0->setData (count, Qt::DisplayRole);
            c0->setEditable (False);
         }

         QStandardItem *c1 (new QStandardItem);

         if (c1) {

            c1->setData ((ss->count / _hookCount) * 100.0, Qt::DisplayRole);
            c1->setEditable (False);
         }

         QStandardItem *c2 (new QStandardItem (ss->func.get_buffer ()));
         if (c2) { c2->setEditable (False); }
         QStandardItem *c3 (new QStandardItem (ss->file.get_buffer ()));
         if (c3) { c3->setEditable (False); }

         table->model.setItem (count, 0, c0);
         table->model.setItem (count, 1, c1);
         table->model.setItem (count, 2, c2);
         table->model.setItem (count, 3, c3);
         count++;

         ss = _statsTable.get_next (it);
      }

      table->show ();
      table->activateWindow ();
   }
}



void
dmz::QtPluginLuaProfiler::_init (Config &local) {

   _luaModuleName = config_to_string ("module.lua.name", local);

   RuntimeContext *context (get_plugin_runtime_context ());

   _showMsg = config_create_message (
      "show.name",
      local,
      "DMZ_Show_Lua_Profiler",
      context);

   subscribe_to_message (_showMsg);

   if (context) {

      Config session (get_session_config (get_plugin_name (), context));

      QByteArray geometry (config_to_qbytearray ("geometry", session, saveGeometry ()));
      restoreGeometry (geometry);

      if (config_to_boolean ("window.visible", session, False)) { show (); }
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginLuaProfiler (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginLuaProfiler (Info, local);
}

};
