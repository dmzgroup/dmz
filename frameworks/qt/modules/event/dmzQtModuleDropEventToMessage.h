#ifndef DMZ_QT_MODULE_DROP_EVENT_TO_MESSAGE_DOT_H
#define DMZ_QT_MODULE_DROP_EVENT_TO_MESSAGE_DOT_H

#include <dmzQtModuleDropEvent.h>
#include <dmzRenderModulePick.h>
#include <dmzRuntimeDataConverterTypesBase.h>
#include <dmzRuntimeDataConverterVector.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHandleContainer.h>

namespace dmz {

   class QtModuleDropEventToMessage :
         public Plugin,
         public QtModuleDropEvent {

      public:
         QtModuleDropEventToMessage (const PluginInfo &Info, Config &local);
         ~QtModuleDropEventToMessage ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // QtModuleDropEvent Interface
         virtual void receive_drop_event (
            const Handle Source,
            const QDropEvent &Event);

      protected:
         // QtModuleDropEventToMessage Interface
         void _init (Config &local);

         Log _log;

         RenderModulePick *_pick;

         DataConverterString _sConvert;
         DataConverterVector _vConvert;

         HandleContainer _targets;
         Message _msg;

      private:
         QtModuleDropEventToMessage ();
         QtModuleDropEventToMessage (const QtModuleDropEventToMessage &);
         QtModuleDropEventToMessage &operator= (const QtModuleDropEventToMessage &);

   };
};

#endif // DMZ_QT_MODULE_DROP_EVENT_TO_MESSAGE_DOT_H
