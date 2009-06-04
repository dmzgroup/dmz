#ifndef DMZ_QT_PLUGIN_MAP_OBJECT_POSITION_DOT_H
#define DMZ_QT_PLUGIN_MAP_OBJECT_POSITION_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>


namespace dmz {

   class RenderModulePick;

   class QtPluginMapObjectPosition :
         public Plugin,
         public ObjectObserverUtil {

      public:
         QtPluginMapObjectPosition (const PluginInfo &Info, Config &local);
         ~QtPluginMapObjectPosition ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Object Observer Interface
         virtual void update_object_position (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

      protected:
         Boolean _map_to_canvas (const Vector &Source, Vector &dest);
         void _init (Config &local);

         Log _log;
         RenderModulePick *_pickModule;
         String _pickModuleName;
         Handle _defaultAttrHandle;
         Handle _positionAttrHandle;
         Handle _sourceCanvas;
         Handle _sourceMap;
         ObjectTypeSet _typeSet;

      private:
         QtPluginMapObjectPosition ();
         QtPluginMapObjectPosition (const QtPluginMapObjectPosition &);
         QtPluginMapObjectPosition &operator= (const QtPluginMapObjectPosition &);

   };
};

#endif // DMZ_QT_PLUGIN_MAP_OBJECT_POSITION_DOT_H
