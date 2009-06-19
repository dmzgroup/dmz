#ifndef DMZ_QT_PLUGIN_MAP_OBJECT_POSITION_DOT_H
#define DMZ_QT_PLUGIN_MAP_OBJECT_POSITION_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesVector.h>
#include <qmapcontrol.h>
#include <QtGui/QWidget>


namespace dmz {

   class RenderModulePick;
   class QtModuleCanvas;
   class QtModuleMap;
   
   class QtPluginMapObjectPosition :
         public QWidget,
         public Plugin,
         public ObjectObserverUtil {
            
      Q_OBJECT

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

      protected slots:
         void slot_zoom_changed (int zoom);

      protected:
         Boolean _world_to_canvas (const Vector &WorldPos, Vector &canvasPos);
         Boolean _update_object (const Handle ObjectHandle, const Vector &WorldPos);
         void _init (Config &local);

         Log _log;
         QtModuleCanvas *_canvasModule;
         String _canvasModuleName;
         QtModuleMap *_mapModule;
         String _mapModuleName;
         RenderModulePick *_pickModule;
         String _pickModuleName;
         Handle _defaultAttrHandle;
         Handle _positionAttrHandle;
         Handle _sourceCanvas;
         ObjectTypeSet _typeSet;
         VectorComponentEnum _vectorOrder[3];

      private:
         QtPluginMapObjectPosition ();
         QtPluginMapObjectPosition (const QtPluginMapObjectPosition &);
         QtPluginMapObjectPosition &operator= (const QtPluginMapObjectPosition &);

   };
};

#endif // DMZ_QT_PLUGIN_MAP_OBJECT_POSITION_DOT_H
