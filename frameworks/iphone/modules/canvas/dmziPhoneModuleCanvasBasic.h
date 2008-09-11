#ifndef DMZ_IPHONE_MODULE_CANVAS_BASIC_DOT_H
#define DMZ_IPHONE_MODULE_CANVAS_BASIC_DOT_H

#include <dmziPhoneModuleCanvas.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>

@class dmziPhoneCanvasView;


namespace dmz {

   class iPhoneModuleUI;

   class iPhoneModuleCanvasBasic :
         public Plugin,
         public iPhoneModuleCanvas {

      public:
         iPhoneModuleCanvasBasic (const PluginInfo &Info, Config &local);
         ~iPhoneModuleCanvasBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // iPhoneModuleCanvas Interface
         virtual Boolean add_item (const Handle ObjectHandle, UIView *item);
         virtual UIView *lookup_item (const Handle ObjectHandle);
         virtual UIView *remove_item (const Handle ObjectHandle);

      protected:
         void _save_session ();
         void _load_session ();
         void _init (Config &local);

         Log _log;
         String _canvasName;
         iPhoneModuleUI *_uiModule;
         String _mainWindowModuleName;
         Handle _channel;
         dmziPhoneCanvasView *_canvas;
         HashTableHandleTemplate<UIView> _itemTable;

      private:
         iPhoneModuleCanvasBasic ();
         iPhoneModuleCanvasBasic (const iPhoneModuleCanvasBasic &);
         iPhoneModuleCanvasBasic &operator= (const iPhoneModuleCanvasBasic &);
   };
};


#endif // DMZ_IPHONE_MODULE_CANVAS_BASIC_DOT_H
