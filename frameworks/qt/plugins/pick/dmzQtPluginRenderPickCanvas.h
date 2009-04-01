#ifndef DMZ_QT_PLUGIN_RENDER_PICK_CANVAS_DOT_H
#define DMZ_QT_PLUGIN_RENDER_PICK_CANVAS_DOT_H

#include <dmzRenderPickUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesVector.h>

class QPoint;


namespace dmz {

   class QtModuleCanvas;
   class ObjectModule;


   class QtPluginRenderPickCanvas :
      public Plugin,
      private RenderPickUtil {

      public:
         QtPluginRenderPickCanvas (const PluginInfo &Info, Config &local);
         ~QtPluginRenderPickCanvas ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // RenderModulePick Interface
         virtual Boolean screen_to_world (
            const Int32 ScreenPosX,
            const Int32 ScreenPosY,
            Vector &worldPosition,
            Vector &normal,
            Handle &objectHandle);

         virtual Boolean world_to_screen (
            const Vector &WorldPosition,
            Int32 &screenPosX,
            Int32 &screenPosY);

         virtual Boolean source_to_world (
            const Int32 SourcePosX,
            const Int32 SourcePosY,
            Vector &worldPosition,
            Vector &normal,
            Handle &objectHandle);

         virtual Boolean world_to_source (
            const Vector &WorldPosition,
            Int32 &sourcePosX,
            Int32 &sourcePosY);

      protected:
         Handle _get_object_handle (const QPoint &Pos);
         void _init (Config &local);

         Log _log;
         QtModuleCanvas *_canvasModule;
         String _canvasModuleName;
         ObjectModule *_objectModule;
         String _objectModuleName;
         VectorComponentEnum _vectorOrder[3];

      private:
         QtPluginRenderPickCanvas ();
         QtPluginRenderPickCanvas (const QtPluginRenderPickCanvas &);
         QtPluginRenderPickCanvas &operator= (const QtPluginRenderPickCanvas &);
   };
};


#endif // DMZ_QT_PLUGIN_RENDER_PICK_CANVAS_DOT_H

