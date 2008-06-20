#ifndef DMZ_QT_PLUGIN_LUA_PROFILER_DOT_H
#define DMZ_QT_PLUGIN_LUA_PROFILER_DOT_H

#include <dmzLuaObserver.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <QtGui/QWidget>
#include <ui_dmzQtLuaProfiler.h>

namespace dmz {

   class LuaModule;

   class QtPluginLuaProfiler :
         public QWidget,
         public Plugin,
         public TimeSlice,
         public MessageObserver,
         public LuaObserver {

         Q_OBJECT

      public:
         QtPluginLuaProfiler (const PluginInfo &Info, Config &local);
         ~QtPluginLuaProfiler ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Message Observer Interface
         virtual void receive_message (
            const Message &Msg,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

         // LuaObserver Interface
         virtual void store_lua_module (const String &ModuleName, LuaModule &module);
         virtual void remove_lua_module (const String &ModuleName, LuaModule &module);

         virtual void add_lua_script (lua_State *L, const String &Path) {;}
         virtual void remove_lua_script (lua_State *L, const String &Path) {;}
         virtual void update_lua_hook_count (lua_State *L, const Int32 Count);
         virtual void lua_hook (lua_State *L, lua_Debug *ar);
         virtual void lua_parse_error (lua_State *L) {;}
         virtual void lua_runtime_error (lua_State *L) {;}

      protected slots:
         void on_startButton_clicked ();
         void on_timeSpinBox_valueChanged (int value);
         void on_stepsSpinBox_valueChanged (int value);

      protected:
         struct StatsStruct {

            Float64 count;
            String file;
            String func;

            StatsStruct () : count (0) {;}
         };

         void _create_profile_table ();
         void _init (Config &local);

         Log _log;
         UInt32 _tableCount;
         Message _showMsg;
         Float64 _hookCount;
         HashTableStringTemplate<StatsStruct> _statsTable;
         String _luaModuleName;
         Boolean _profiling;
         Boolean _inCountUpdate;
         LuaModule *_luaMod;
         Ui::QtLuaProfiler _ui;

      private:
         QtPluginLuaProfiler ();
         QtPluginLuaProfiler (const QtPluginLuaProfiler &);
         QtPluginLuaProfiler &operator= (const QtPluginLuaProfiler &);

   };
};

#endif // DMZ_QT_PLUGIN_LUA_PROFILER_DOT_H
