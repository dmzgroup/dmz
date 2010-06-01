#ifndef DMZ_ENTITY_PLUGIN_PORTAL_FOLLOW_3D_DOT_H
#define DMZ_ENTITY_PLUGIN_PORTAL_FOLLOW_3D_DOT_H

#include <dmzEntityModulePortal.h>
#include <dmzInputObserverUtil.h>
#include <dmzObjectModule.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

namespace dmz {

   class EntityModulePortal;

   class EntityPluginPortalFollow3D :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil,
         public InputObserverUtil {

      public:
         //! \cond
         EntityPluginPortalFollow3D (const PluginInfo &Info, Config &local);
         ~EntityPluginPortalFollow3D ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Object Observer Interface
         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State);

      protected:
         void _init (Config &local);

         const Int32 _HistoryCount;
         EntityModulePortal *_portal;
         Handle _hil;
         Handle _defaultHandle;
         Handle _hilAttrHandle;

         Matrix *_history;
         Int32 _which;

         Vector _offset;

         Int32 _active;

         Log _log;
         //! \endcond

      private:
         EntityPluginPortalFollow3D ();
         EntityPluginPortalFollow3D (const EntityPluginPortalFollow3D &);
         EntityPluginPortalFollow3D &operator= (const EntityPluginPortalFollow3D &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_PORTAL_FOLLOW_3D_DOT_H
