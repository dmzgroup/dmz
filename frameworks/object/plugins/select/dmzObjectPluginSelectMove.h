#ifndef DMZ_OBJECT_PLUGIN_SELECT_MOVE_DOT_H
#define DMZ_OBJECT_PLUGIN_SELECT_MOVE_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class ObjectModuleSelect;

   class ObjectPluginSelectMove :
         public Plugin,
         public ObjectObserverUtil {

      public:
         //! \cond
         ObjectPluginSelectMove (const PluginInfo &Info, Config &local);
         ~ObjectPluginSelectMove ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

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

         virtual void update_object_orientation (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Matrix &Value,
            const Matrix *PreviousValue);

      protected:
         void _init (Config &local);

         Handle _defaultHandle;
         Boolean _inUpdate;
         ObjectModuleSelect *_selectMod;

         Log _log;
         //! \endcond

      private:
         ObjectPluginSelectMove ();
         ObjectPluginSelectMove (const ObjectPluginSelectMove &);
         ObjectPluginSelectMove &operator= (const ObjectPluginSelectMove &);

   };
};

#endif // DMZ_OBJECT_PLUGIN_SELECT_MOVE_DOT_H
