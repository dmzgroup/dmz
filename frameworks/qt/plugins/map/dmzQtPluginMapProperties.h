#ifndef DMZ_QT_PLUGIN_MAP_PROPERTIES_DOT_H
#define DMZ_QT_PLUGIN_MAP_PROPERTIES_DOT_H

#include <dmzApplicationState.h>
#include <dmzArchiveObserverUtil.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeDataConverters.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeUndo.h>


namespace dmz {

   class QtModuleMainWindow;

   class QtPluginMapProperties :
         public Plugin,
         public ArchiveObserverUtil,
         public MessageObserver {

      public:
         QtPluginMapProperties (const PluginInfo &Info, Config &local);
         ~QtPluginMapProperties ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // ArchiveObserver Interface.
         virtual void create_archive (
            const Handle ArchiveHandle,
            Config &local,
            Config &global);

         virtual void process_archive (
            const Handle ArchiveHandle,
            Config &local,
            Config &global);

         // Message Observer Interface
         virtual void receive_message (
            const Message &Type,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

      protected:
         QString _get_last_path ();
         void _init (Config &local);

         Log _log;
         Undo _undo;
         DataConverterString _dataConverter;
         ApplicationStateWrapper _appState;
         QtModuleMainWindow *_mainWindowModule;
         String _mainWindowModuleName;
         QtModuleMap *_mapModule;
         String _mapModuleName;
         Message _cleanupMessage;
         Message _backgroundEditMessage;
         Message _undoMessage;
         Config _data;
         
      private:
         QtPluginMapProperties ();
         QtPluginMapProperties (const QtPluginMapProperties &);
         QtPluginMapProperties &operator= (const QtPluginMapProperties &);
   };
};

#endif // DMZ_QT_PLUGIN_MAP_PROPERTIES_DOT_H
