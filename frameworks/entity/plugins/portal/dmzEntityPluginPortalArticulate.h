#ifndef DMZ_ENTITY_PLUGIN_PORTAL_VIEWER_DOT_H
#define DMZ_ENTITY_PLUGIN_PORTAL_VIEWER_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

namespace dmz {

   class EntityModulePortal;

   class EntityPluginPortalArticulate :
         public Plugin,
         public TimeSlice,
         public InputObserverUtil,
         public ObjectObserverUtil {

      public:
         //! \cond
         EntityPluginPortalArticulate (const PluginInfo &Info, Config &local);
         ~EntityPluginPortalArticulate ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State);

         // Object Observer Interface
         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

         virtual void update_object_scalar (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue);

      protected:
         struct ComponentStruct {

            const Handle AttrHandle;
            const Vector Axis;
            const Vector Offset;
            Float64 angle;
            ComponentStruct *next;

            ComponentStruct (
                  const Handle TheAttrHandle,
                  const Vector &TheAxis,
                  const Vector &TheOffset) :
                  AttrHandle (TheAttrHandle),
                  Axis (TheAxis),
                  Offset (TheOffset),
                  angle (0.0),
                  next (0) {;}

            ~ComponentStruct () { if (next) { delete next; next = 0; } }
         };

         void _init (Config &local);

         Log _log;

         EntityModulePortal *_portal;

         HashTableHandleTemplate<ComponentStruct> _componentTable;
         ComponentStruct *_transform;

         Int32 _active;
         Handle _defaultAttrHandle;
         Handle _hilAttrHandle;
         Handle _hil;
         //! \endcond

      private:
         EntityPluginPortalArticulate ();
         EntityPluginPortalArticulate (const EntityPluginPortalArticulate &);
         EntityPluginPortalArticulate &operator= (const EntityPluginPortalArticulate &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_PORTAL_VIEWER_DOT_H
