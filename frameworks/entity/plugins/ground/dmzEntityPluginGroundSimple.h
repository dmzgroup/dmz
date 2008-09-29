#ifndef DMZ_ENTITY_PLUGIN_GROUND_SIMPLE_DOT_H
#define DMZ_ENTITY_PLUGIN_GROUND_SIMPLE_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzObjectModule.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRenderIsect.h>
#include <dmzRenderModuleIsect.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzRuntimeTime.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class EventModuleCommon;
   class Vector;

   class EntityPluginGroundSimple :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil,
         public InputObserverUtil {

      public:
         //! \cond
         EntityPluginGroundSimple (const PluginInfo &Info, Config &local);
         ~EntityPluginGroundSimple ();

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
         struct MoveStruct {

            Float64 maxSpeed;
            Float64 turboBoost;
            Float64 turboModifier;
            Float64 turboEndTime;
            Float64 turboDuration;
            Float64 turboRefillDuration;
            Float64 turboRefillTime;
            Float64 maxAccel;
            Float64 maxDecel;
            Float64 turnRate;
            Float64 throttle;
            Float64 turn;
            Float64 brakeRate;
            Float64 brake;
            Float64 whiskerLength;
            Float64 whiskerHeight;

            MoveStruct () :
                  maxSpeed (45.0),
                  turboBoost (15.0),
                  turboModifier (15.0),
                  turboEndTime (0.0),
                  turboDuration (3.0),
                  turboRefillDuration (10.0),
                  turboRefillTime (0.0),
                  maxAccel (15.0),
                  maxDecel (7.0),
                  turnRate (HalfPi64),
                  throttle (0.0),
                  turn (0.0),
                  brakeRate (30.0),
                  brake (0.0),
                  whiskerLength (0.75),
                  whiskerHeight (1.0) {;}
         };

         Boolean _find_point (
            const Vector &Pos,
            const Matrix &Ori,
            Vector &point,
            Vector &normal);

         Boolean _validate_isect_result (
            const Vector &Start,
            const Vector &Dir,
            const IsectResultContainer &IsectResults,
            Vector &point,
            Vector &normal);

         void _move_entity (
            const Float64 TimeDelta,
            const Boolean Airborn,
            Vector &pos,
            Matrix &ori,
            Vector &vel,
            Float64 &heading);

         // Note: The const values are NOT passed in by reference. This way, the
         // new values can be the same variables passed in as the consts.
         void _validate_move (
            const Float64 TimeDelta,
            const Boolean Airborn,
            const Vector StartPos,
            Vector &pos,
            Matrix &ori,
            Vector &vel,
            Float64 &heading);

         void _validate_move_isect_point (
            const Handle CullMode,
            const Float64 LastDistance,
            const Float64 Distance,
            const Vector &Point,
            const Vector &Dir,
            const Vector &Normal,
            Boolean &found,
            Float64 &outDistance,
            Vector &outPoint,
            Vector &outNorm);

         void _init (Config &local);

         RenderModuleIsect *_isect;
         EventModuleCommon *_eventMod;
         Boolean _wasAirborn;
         Handle _hil;
         Handle _defaultHandle;
         Handle _hilHandle;
         Handle _throttleHandle;

         MoveStruct _move;

         IsectTestContainer _isectTestContainer;
         IsectParameters _isectParameters;

         Mask _deadState;
         Boolean _isDead;

         Int32 _active;

         Time _time;

         Log _log;
         //! \endcond

      private:
         EntityPluginGroundSimple (const EntityPluginGroundSimple &);
         EntityPluginGroundSimple &operator= (const EntityPluginGroundSimple &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_GROUND_SIMPLE_DOT_H
