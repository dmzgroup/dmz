#ifndef DMZ_QT_PLUGIN_CANVAS_AUTO_TRACK_DOT_H
#define DMZ_QT_PLUGIN_CANVAS_AUTO_TRACK_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeSync.h>


namespace dmz {

   class QtModuleCanvas;
   

   class QtPluginCanvasAutoTrack : 
      public Plugin,
      public Sync,
      public ObjectObserverUtil {

      public:
         QtPluginCanvasAutoTrack (const PluginInfo &Info, Config &local);
         ~QtPluginCanvasAutoTrack ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // Sync Interface
         virtual void update_sync (const Float64 TimeDelta);

         // Object Observer Interface
         virtual void update_object_position (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_orientation (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Matrix &Value,
            const Matrix *PreviousValue);

         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

      protected:
         void _init (Config &local);

         Log _log;
         QtModuleCanvas *_canvasModule;
         String _canvasModuleName;
         Handle _defaultAttributeHandle;
         Handle _hilAttributeHandle;
         Handle _hilHandle;
         Boolean _updateView;
         
      private:
         QtPluginCanvasAutoTrack ();
         QtPluginCanvasAutoTrack (const QtPluginCanvasAutoTrack &);
         QtPluginCanvasAutoTrack &operator= (const QtPluginCanvasAutoTrack &);

   };
};

#endif // DMZ_QT_PLUGIN_CANVAS_AUTO_TRACK_DOT_H
