#ifndef DMZ_RENDER_PLUGIN_RADAR_OVERLAY_DOT_H
#define DMZ_RENDER_PLUGIN_RADAR_OVERLAY_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesVector.h>

namespace dmz {

   class RenderModuleOverlay;
   class RenderModulePortal;

   class RenderPluginRadarOverlay :
         public Plugin,
         public TimeSlice,
         public InputObserverUtil,
         public ObjectObserverUtil {

      public:
         //! \cond
         RenderPluginRadarOverlay (const PluginInfo &Info, Config &local);
         ~RenderPluginRadarOverlay ();

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

         virtual void receive_axis_event (
            const Handle Channel,
            const InputEventAxis &Value);

         virtual void receive_button_event (
            const Handle Channel,
            const InputEventButton &Value);

         virtual void receive_switch_event (
            const Handle Channel,
            const InputEventSwitch &Value);

         virtual void receive_key_event (
            const Handle Channel,
            const InputEventKey &Value);

         virtual void receive_mouse_event (
            const Handle Channel,
            const InputEventMouse &Value);

         virtual void receive_data_event (
            const Handle Channel,
            const Handle Source,
            const Data &Value);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_state (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &Value,
            const Mask *PreviousValue);

         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

         virtual void update_object_position (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

      protected:
         struct ObjectDefStruct {

            const String CloneName;
            const String SwitchName;
            const String XFormName;

            ObjectDefStruct (
                  const String &TheCloneName,
                  const String &TheSwitchName,
                  const String &TheXFormName) :
                  CloneName (TheCloneName),
                  SwitchName (TheSwitchName),
                  XFormName (TheXFormName) {;}
         };

         struct ObjectStruct {

            Handle model;
            Handle switchHandle;
            Handle xformHandle;

            Vector pos;
            Boolean visible;

            ObjectStruct () :
                  model (0),
                  switchHandle (0),
                  xformHandle (0),
                  visible (False) {;}
         };

         void _set_visiblity (const Boolean Value, ObjectStruct &os);
         ObjectDefStruct *_lookup_def (const ObjectType &Type);
         ObjectDefStruct *_create_def (const ObjectType &Type);
         void _init (Config &local);

         Log _log;

         RenderModuleOverlay *_overlay;
         RenderModulePortal *_portal;

         Message _rangeMsg;

         String _rootName;
         Handle _root;
         Handle _defaultAttrHandle;
         Handle _hilAttrHandle;
         Handle _hil;
         Handle _rangeHandle;
         Float64 _radius;
         Float64 _range;
         Float64 _startRange;
         Float64 _lastRange;
         Float64 _rangeMin;
         Float64 _rangeMax;
         Float64 _rangeRate;
         Int32 _rangeCount;

         HashTableHandleTemplate<ObjectDefStruct> _defTable;
         HashTableHandleTemplate<ObjectDefStruct> _defMasterTable;
         HashTableHandleTemplate<ObjectStruct> _objTable;
         ObjectTypeSet _ignoreTypes;
         //! \endcond

      private:
         RenderPluginRadarOverlay ();
         RenderPluginRadarOverlay (const RenderPluginRadarOverlay &);
         RenderPluginRadarOverlay &operator= (const RenderPluginRadarOverlay &);

   };
};

#endif // DMZ_RENDER_PLUGIN_RADAR_OVERLAY_DOT_H
