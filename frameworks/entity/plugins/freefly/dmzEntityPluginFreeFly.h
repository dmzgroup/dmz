#ifndef DMZ_ENTITY_PLUGIN_FREE_FLY_DOT_H
#define DMZ_ENTITY_PLUGIN_FREE_FLY_DOT_H

#include <dmzObjectModule.h>
#include <dmzObjectObserverUtil.h>
#include <dmzInputObserverUtil.h>
#include <dmzRenderIsect.h>
#include <dmzRenderModuleIsect.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

namespace dmz {

   class Vector;
   class Matrix;

   class EntityPluginFreeFly :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil,
         public InputObserverUtil {

      public:
         //! \cond
         EntityPluginFreeFly (const PluginInfo &Info, Config &local);

         ~EntityPluginFreeFly ();

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

         virtual void update_object_scalar (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue);

         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State);

         virtual void receive_axis_event (
            const Handle Channel,
            const InputEventAxis &Value);

         virtual void receive_button_event (
            const Handle Channel,
            const InputEventButton &Value);

         virtual void receive_key_event (
            const Handle Channel,
            const InputEventKey &Value);

         virtual void receive_mouse_event (
            const Handle Channel,
            const InputEventMouse &Value);

         virtual void receive_switch_event (
            const Handle Channel,
            const InputEventSwitch &Value);

      protected:
         struct MovementStruct {
            Float64 moveSpeed;
            Float64 turnRate;
            Float64 speedAxis;
            Float64 strafeAxis;
            Float64 ymoveAxis;
            Float64 turnAxis;
            Float64 pitchAxis;

            Float64 yawAngle;
            Float64 pitchAngle;

            MovementStruct () :
                  moveSpeed (1.0),
                  turnRate (1.0),
                  speedAxis (0.0),
                  strafeAxis (0.0),
                  ymoveAxis (0.0),
                  turnAxis (0.0),
                  pitchAxis (0.0),
                  yawAngle (0.0),
                  pitchAngle (0.0) {;}
         } _move;

         void _init (Config &local);

         RenderModuleIsect *_isect;
         Handle _handle;
         Handle _defaultHandle;
         Handle _hilHandle;

         IsectTestContainer _isectTestContainer;
         IsectParameters _isectParameters;

         Int32 _active;

         Log _log;
         //! \endcond

      private:
         EntityPluginFreeFly (const EntityPluginFreeFly &);
         EntityPluginFreeFly &operator= (const EntityPluginFreeFly &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_FREE_FLY_DOT_H
