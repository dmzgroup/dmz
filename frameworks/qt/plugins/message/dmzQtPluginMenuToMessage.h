#ifndef DMZ_MBRA_PLUGIN_FAULT_TREE_BUILDER_DOT_H
#define DMZ_MBRA_PLUGIN_FAULT_TREE_BUILDER_DOT_H

#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHandleContainer.h>
#include <QtCore/QList>
#include <QtCore/QObject>

class QAction;
class QActionGroup;
class QMenu;


namespace dmz {

   class ObjectModule;
   class QtModuleCanvas;
   class RenderModulePick;
   
   
   class QtPluginMenuToMessage :
         public QObject,
         public Plugin,
         public MessageObserver {

      Q_OBJECT

      public:
         QtPluginMenuToMessage (const PluginInfo &Info, Config &local);
         ~QtPluginMenuToMessage ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin () {;}
         virtual void stop_plugin () {;}
         virtual void shutdown_plugin () {;}
         virtual void remove_plugin (const Plugin *PluginPtr);

         // Message Observer Interface
         void receive_message (
            const Message &Msg,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

      protected slots:
         void _slot_send (QAction *action);
      
      protected:
         struct MessageStruct {

            QAction *action;
            Message message;
            HandleContainer targets;
         
            MessageStruct () :
               action (0),
               message (),
               targets () {;}
         };
         
         void _get_targets (
            const String &Name,
            Config &config,
            HandleContainer &targets);
               
         void _get_type_set (
            const String &Name,
            Config &config,
            ObjectTypeSet &set);

         void _build_menu (const String &Name, Config &config);
         
         void _init (Config &local);
          
         Log _log;
         Definitions _defs;
         ObjectModule *_objectModule;
         String _objectModuleName;
         RenderModulePick *_pickModule;
         String _pickModuleName;
         Message _showMessage;
         Handle _defaultAttrHandle;
         Handle _source;
         Handle _objectAttrHandle;
         Handle _positionAttrHandle;
         Handle _object;
         ObjectTypeSet _objectSet;
         QActionGroup *_actionGroup;
         QList<MessageStruct *> _messageList;   
         QMenu *_menu;
         
      private:
         QtPluginMenuToMessage ();
         QtPluginMenuToMessage (const QtPluginMenuToMessage &);
         QtPluginMenuToMessage &operator= (const QtPluginMenuToMessage &);

   };
};

#endif // DMZ_MBRA_PLUGIN_FAULT_TREE_BUILDER_DOT_H
