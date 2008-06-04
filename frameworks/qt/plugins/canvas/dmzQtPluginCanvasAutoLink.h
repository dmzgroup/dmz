#ifndef DMZ_QT_PLUGIN_CANVAS_AUTO_LINK_DOT_H
#define DMZ_QT_PLUGIN_CANVAS_AUTO_LINK_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>


namespace dmz {
   
   class QtPluginCanvasAutoLink :
         public Plugin,
         public ObjectObserverUtil {

      public:
         QtPluginCanvasAutoLink (const PluginInfo &Info, Config &local);
         ~QtPluginCanvasAutoLink ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

      protected:
         void _init (Config &local);
         void _get_type_set (const String &Name, Config &config, ObjectTypeSet &set);

         Log _log;
         Handle _defaultAttrHandle;
         Handle _activeAttrHandle;
         Handle _linkAttrHandle;
         Handle _activeLayer;
         ObjectTypeSet _ignoreSet;
         ObjectTypeSet _linkSet;

      private:
         QtPluginCanvasAutoLink ();
         QtPluginCanvasAutoLink (const QtPluginCanvasAutoLink &);
         QtPluginCanvasAutoLink &operator= (const QtPluginCanvasAutoLink &);

   };
};

#endif // DMZ_QT_PLUGIN_CANVAS_AUTO_LINK_DOT_H
