#ifndef DMZ_PLUGIN_ODE_SIMPLE_DOT_H
#define DMZ_PLUGIN_ODE_SIMPLE_DOT_H

#include <dmzObjectModule.h>
#include <dmzObjectObserverUtil.h>
#include <dmzInputObserverUtil.h>
#include <dmzRenderIsect.h>
//#include <dmzRenderModulePortal.h>
#include <dmzRenderModuleCoreOSG.h>
#include <dmzRenderModuleIsect.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableUInt32Template.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

#include <ode/ode.h>


namespace dmz {

   class Vector;
   class Matrix;

   class DynamicsPluginODESimple :
         public Plugin,
         public ObjectObserverUtil,
         public InputObserverUtil {

      public:
         DynamicsPluginODESimple (const PluginInfo &Info,
            const String &InputModuleName,
            Config &local);
         ~DynamicsPluginODESimple ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void sync_plugin (const Float64 TimeDelta);
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // Object Observer Interface
         virtual void update_object_flag (
            const UUID &Identity,
            const ObjectHandle Handle,
            const UInt32 AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

         virtual void update_object_scalar (
            const UUID &Identity,
            const ObjectHandle Handle,
            const UInt32 AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue);

         // Input Observer Interface
         virtual void input_channels_initialized ();
         virtual void input_channels_uninitialized ();

         virtual void update_channel_state (const UInt32 Channel, const Boolean State);

         virtual void receive_axis_event (
            const UInt32 Channel,
            const InputEventAxis &Value);

         virtual void receive_button_event (
            const UInt32 Channel,
            const InputEventButton &Value);

         virtual void receive_key_event (
            const UInt32 Channel,
            const InputEventKey &Value);

         virtual void receive_mouse_event (
            const UInt32 Channel,
            const InputEventMouse &Value);

         virtual void receive_switch_event (
            const UInt32 Channel,
            const InputEventSwitch &Value);

         dWorldID get_world_id ();
         dJointGroupID get_joint_group_id ();
         Float32 get_erp ();
         Float32 get_cfm ();

      protected:
         struct MovementStruct {
            Float64 moveSpeed;
            Float64 turnRate;
            Float64 speedAxis;
            Float64 strafeAxis;
            Float64 turnAxis;
            Float64 pitchAxis;

            Float64 yawAngle;
            Float64 pitchAngle;

            MovementStruct () :
                  moveSpeed (1.0),
                  turnRate (1.0),
                  speedAxis (0.0),
                  strafeAxis (0.0),
                  turnAxis (0.0),
                  pitchAxis (0.0),
                  yawAngle (0.0),
                  pitchAngle (0.0) {;}
         } _move;



         void _init (Config &local);

         RenderModuleIsect *_isect;
         RenderModuleCoreOSG *_renderCoreOSG;
         ObjectModule *_objMod;
         ObjectHandle _handle;
         UInt32 _defaultHandle;
         UInt32 _hilHandle;
         UInt32 _throttleHandle;

         Float64 _heading;
         Float64 _pitch;

         IsectTestContainer _isectTestContainer;
         IsectParameters _isectParameters;

         Log _log;

         // ODE data

         dVector3 *_terrainMeshVertices;
         UInt32 _numTerrainMeshVertices;

         int *_terrainMeshTriangleIndices;
         UInt32 _numTerrainMeshTriangleIndices;

         // Configuration Data

         // ODE's "Error Reduction Parameter". This is empirically defined.
         Float32 _ERP;

         // ODE's "Constraint Force Mixing". This is empirically defined.
         Float32 _CFM;

         // Magnitude of gravity vector
         Float32 _gravity;

         // Initial position of vehicle in global coordinates
         Vector _vehicleInitialPos;

         // Vehicle - total mass
         Float32 _vehicleTotalMass;

         // Vehicle - length of longest side of box
         Float32 _vehicleLength;

         // Vehicle - movement force scale
         Float32 _vehicleForceScale;

         // Max simulation timestep (seconds)
         Float32 _maxTimeStep;

         dWorldID _world;
         dSpaceID _space;
         dGeomID _vehicleGeom;
         dBodyID _vehicleBody;
         dGeomID _groundGeom;
         dJointGroupID _contactGroup;


         static void _ode_collision_callback (void* data, dGeomID o1, dGeomID o2);


      private:
         DynamicsPluginODESimple (const DynamicsPluginODESimple &);
         DynamicsPluginODESimple &operator= (const DynamicsPluginODESimple &);

   };
};

#endif // DMZ_PLUGIN_ODE_SIMPLE_DOT_H
