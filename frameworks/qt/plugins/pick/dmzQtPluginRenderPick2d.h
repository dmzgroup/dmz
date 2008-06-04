#ifndef DMZ_QT_PLUGIN_RENDER_PICK_2D_DOT_H
#define DMZ_QT_PLUGIN_RENDER_PICK_2D_DOT_H

#include <dmzRenderPick2dUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

class QPoint;


namespace dmz {

   class QtModuleCanvas;
   class ObjectModule;


   class QtPluginRenderPick2d :
      public Plugin,
      private RenderPick2dUtil {

      public:
         QtPluginRenderPick2d (const PluginInfo &Info, Config &local);
         ~QtPluginRenderPick2d ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // RenderModule2dPick Interface
         virtual Boolean screen_to_world (
            const Int32 ScreenPosX,
            const Int32 ScreenPosY,
            Vector &worldPosition,
            Handle &objectHandle);

         virtual Boolean world_to_screen (
            const Vector &WorldPosition,
            Int32 &screenPosX,
            Int32 &screenPosY);

         virtual Boolean source_to_world (
            const Int32 SourcePosX,
            const Int32 SourcePosY,
            Vector &worldPosition,
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

      private:
         QtPluginRenderPick2d ();
         QtPluginRenderPick2d (const QtPluginRenderPick2d &);
         QtPluginRenderPick2d &operator= (const QtPluginRenderPick2d &);
   };
};


#endif // DMZ_QT_PLUGIN_RENDER_PICK_2D_DOT_H

