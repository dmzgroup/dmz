#include "dmzDynamicsPluginODESimple.h"
#include <dmzInputEventController.h>
#include <dmzInputEventMasks.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMask.h>



dmz::DynamicsPluginODESimple::DynamicsPluginODESimple (
      const PluginInfo &Info,
      const String &InputModuleName,
      Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, ""),
      InputObserverUtil (Info, InputModuleName),
      _handle (0),
      _defaultHandle (0),
      _hilHandle (0),
      _throttleHandle (0),
      _objMod (0),
      _isect (0),
      _renderCoreOSG (0), 
      _heading (0),
      _pitch (0),
      _log (Info.get_name (), Info.get_context ()),
      _terrainMeshVertices (0),
      _numTerrainMeshVertices (0),
      _terrainMeshTriangleIndices (0),
      _numTerrainMeshTriangleIndices (0),
      _ERP (0.9f),
      _CFM (0.1f),
      _vehicleTotalMass (0.0),
      _vehicleLength (0.0), 
      _vehicleForceScale (0.0),
      _gravity (0.0),
      _maxTimeStep (0.0), 
      _world (0), 
      _space (0), 
      _vehicleGeom (0), 
      _vehicleBody (0), 
      _groundGeom (0),
      _contactGroup (0) {

   const Mask EmptyMask;

   activate_object_attribute (
      ObjectAttributeDefaultName,
      EmptyMask,
      _defaultHandle);

   activate_object_attribute (
      ObjectAttributeHumanInTheLoop,
      ObjectFlagMask,
      _hilHandle);

   activate_object_attribute (
      "throttle",
      ObjectScalarMask,
      _throttleHandle);

   _init (local);
}


dmz::DynamicsPluginODESimple::~DynamicsPluginODESimple () {


   if (_vehicleGeom) { dGeomDestroy(_vehicleGeom); }
   if (_groundGeom) { dGeomDestroy(_groundGeom); }

   if (_contactGroup) {
      dJointGroupEmpty (_contactGroup);
      dJointGroupDestroy (_contactGroup);
   }

   if (_space) {
      dSpaceDestroy (_space);
   }

   if (_world) {
      dWorldDestroy (_world);
   }

   dCloseODE();

}


// Plugin Interface
void
dmz::DynamicsPluginODESimple::discover_plugin (const Plugin *PluginPtr) {

   if (!_isect) { _isect = RenderModuleIsect::cast (PluginPtr); }
   if (!_objMod) { _objMod = ObjectModule::cast (PluginPtr); }
   if (!_renderCoreOSG) { _renderCoreOSG = RenderModuleCoreOSG::cast (PluginPtr); }
}


void
dmz::DynamicsPluginODESimple::start_plugin () {

   if (_renderCoreOSG) {
  
      // Get and then copy the terrain data from OSG render core
      UInt32 numVerts (0);
      UInt32 numIndices (0);

      Vector *vertices (0);
      UInt32 *indicies (0);

      // Get array of vertices and triangle vertex indices from the OSG render core
      // NOTE: ODE docs say that it can automatically calculate normals, but
      // it may be calculating them incorrectly, resulting in odd bouncing behavior,
      // so this render core geometry extraction function may need to pull the normals
      // from OSG too
      _renderCoreOSG->get_static_triangles (&vertices, numVerts, &indicies, numIndices);


      _numTerrainMeshVertices = numVerts;
      _numTerrainMeshTriangleIndices =  numIndices;

      _terrainMeshVertices = new dVector3 [_numTerrainMeshVertices];
      _terrainMeshTriangleIndices = new int [_numTerrainMeshTriangleIndices];

      if (_terrainMeshVertices) {
         for (
               UInt32 vertexCount = 0; 
               vertexCount < _numTerrainMeshVertices; 
               vertexCount++) {

            (_terrainMeshVertices [vertexCount])[0] = 
               dReal (vertices [vertexCount].get_x ());

            (_terrainMeshVertices [vertexCount])[1] = 
               dReal (vertices [vertexCount].get_y ());

            (_terrainMeshVertices [vertexCount])[2] = 
               dReal (vertices [vertexCount].get_z ());
         }
      }

      if (_terrainMeshTriangleIndices) {
         for (
               UInt32 indicesCount = 0; 
               indicesCount < _numTerrainMeshTriangleIndices; 
               indicesCount++) {

            _terrainMeshTriangleIndices [indicesCount] = (int) (indicies [indicesCount]);

         }

      }

      // ODE Setup stuff

      dInitODE();
      _world = dWorldCreate();
      dWorldSetQuickStepNumIterations (_world, 64);


      _space = dHashSpaceCreate(0);
      _contactGroup = dJointGroupCreate (0);

      // NOTE: Gravity SHOULD be negative y, but it appears to be flipped, so
      // the config file currently specifies a negative magnitude (to make the
      // gravity in the world positive) which makes it appear ok - this obviously
      // needs to be sorted out
      dWorldSetGravity (_world, 0, -_gravity, 0);
      dWorldSetCFM (_world, _CFM);

      // Load terrain geometry

      if (_terrainMeshTriangleIndices && _terrainMeshVertices) {

         dTriMeshDataID Data = dGeomTriMeshDataCreate();
          dGeomTriMeshDataBuildSingle (
            Data, 
            (dReal *)_terrainMeshVertices,
            3 * sizeof (dReal),
            _numTerrainMeshVertices, 
            _terrainMeshTriangleIndices,     
            _numTerrainMeshTriangleIndices,
            3 * sizeof (int));

          // Place geometry in the single space we have defined
         _groundGeom = dCreateTriMesh(_space, Data, 0, 0, 0);

         dMatrix3 groundRot;
         dRSetIdentity(groundRot);
         dGeomSetRotation (_groundGeom, groundRot);

         // Eventually need to look up position and orientation of terrain
         // from object module to match up with ODE's internal representation
         dGeomSetPosition(_groundGeom, 0, 0, 0);
         dGeomTriMeshEnableTC(_groundGeom, dSphereClass, False);
         dGeomTriMeshEnableTC(_groundGeom, dBoxClass, False);

         // The terrain geometry has no associated body (it has a body id of zero), 
         // and is placed in the world as just a lone geometry. This results in ODE
         // performing collision detection with this geometry, but not keeping track of 
         // dynamics on the object. With a body id of zero, the geometry will be 
         // considered part of the static environment, which it should be.

      }
      

      // Create Vehicle Body

 
      _vehicleBody = dBodyCreate (_world);
       dMass mass;

      // Sphere mass matrix generation
      //dMassSetSphereTotal (
      //   &mass, 
      //   _vehicleTotalMass,
      //   _vehicleLength / 2.0f);

      //dBodySetMass (_vehicleBody, &mass);

      //Box mass matrix generation
      
      dMassSetBoxTotal (
         &mass, 
         _vehicleTotalMass, 
         _vehicleLength, 
         _vehicleLength / 2.0f, 
         _vehicleLength * 2.0f);

      dBodySetMass (_vehicleBody, &mass);

      // Create Vehicle Geometry

      // Box geometry - box has a height of 1/2 of the requested length, and is
      // twice as long as the requested length in one dimension
      _vehicleGeom = dCreateBox (
         0, 
         _vehicleLength, 
         _vehicleLength / 2.0f, 
         _vehicleLength * 2.0f);

      //Sphere Geometry - sphere has radius of 1/2 of the requested length
      //_vehicleGeom = dCreateSphere(0, _vehicleLength / 2.0f);
     
      // Link geometry to the body (geometry is used for collision, body is used
      // for keeping track of dynamics)
      dGeomSetBody (_vehicleGeom, _vehicleBody);


      // Set position and rotation of vehicle
      dMatrix3 vehicleRot;
      dRSetIdentity (vehicleRot);

      dBodySetPosition (
         _vehicleBody, 
         Float32 (_vehicleInitialPos.get_x ()), 
         Float32 (_vehicleInitialPos.get_y ()), 
         Float32 (_vehicleInitialPos.get_z ()));
      
      if (_objMod) {
         
         _objMod->store_position (_handle, _defaultHandle, _vehicleInitialPos);
      }

      dBodySetRotation (_vehicleBody, vehicleRot);

      // Set velocities of vehicle
      dBodySetLinearVel (_vehicleBody, 0,0,0);
      dBodySetAngularVel (_vehicleBody, 0,0,0);

      // Add vehicle (geometry with associated body) into our single space
      dSpaceAdd (_space, _vehicleGeom);
   }

}


void
dmz::DynamicsPluginODESimple::sync_plugin (const Float64 TimeDelta) {

   if (_objMod && _handle && _defaultHandle && _renderCoreOSG) {

      Vector currentPos;
      Matrix currentOri;
      currentOri.set_identity ();

      _objMod->lookup_position (_handle, _defaultHandle, currentPos);
      _objMod->lookup_orientation (_handle, _defaultHandle, currentOri);


  
      Vector right (1.0, 0.0, 0.0);
      Vector up (0.0, 1.0, 0.0);
      Vector forward (0.0, 0.0, -1.0);

     


      currentOri.transform_vector (right);
      currentOri.transform_vector (forward);
      currentOri.transform_vector (up);

      Vector headingTorque = up * _move.turnAxis;
      Vector pitchTorque = right * _move.pitchAxis;

      Vector totalTorque (headingTorque + pitchTorque);
      totalTorque.normalize_in_place ();

      // Torque force is much more sensitive, so we reduce the torque force
      // relative to the linear force
      totalTorque = totalTorque * (_vehicleForceScale / 100.0);



      Vector forwardForce (forward * _move.speedAxis);
      Vector rightForce (right * _move.strafeAxis);

      Vector directionForce (forwardForce + rightForce);
      directionForce.normalize_in_place ();

      directionForce = directionForce * _vehicleForceScale;

      Float64 deltaSecs = TimeDelta;

      // Main simulation loop
      while (deltaSecs > 0.0) {

         // Apply forces to the bodies as necessary.
         // (forces are valid for one time step only, so this must be done inside
         // this inner loop that keeps the time steps short)

         // NOTE: for some reason, the forward force causes backwards movement
         // on the screen
         dBodyAddForce (
            _vehicleBody, 
            dReal (directionForce.get_x ()), 
            dReal (directionForce.get_y ()), 
            dReal (directionForce.get_z ()));

         dBodyAddTorque (
            _vehicleBody, 
            dReal (totalTorque.get_x ()), 
            dReal (totalTorque.get_y ()), 
            dReal (totalTorque.get_z ()));
            


         // Call collision detection.
         dSpaceCollide (_space, (void *) (this), &_ode_collision_callback);

         // Create a contact joint for every collision point, and put it in
         //      the contact joint group.
         // See '_ode_collision_callback()'

         // Take a simulation step
         Float64 step (0.0);
         
         if (_maxTimeStep < deltaSecs) { step = _maxTimeStep; }
         else { step = deltaSecs; }

         deltaSecs -= _maxTimeStep;

         dWorldStep (_world, dReal (step));
         // Alternatively, for a faster step:
         //dWorldQuickStep (_world, dReal (step));

         // Remove all joints in the contact joint group.
         dJointGroupEmpty (_contactGroup);
      }

      const dReal* vehicleRotation (dBodyGetRotation (_vehicleBody));

      const dReal* vehiclePosition (dBodyGetPosition (_vehicleBody));

      const dReal* vehicleLinearVel (dBodyGetLinearVel  (_vehicleBody));

      Vector newPos (
         (Float64) (vehiclePosition[0]), 
         (Float64) (vehiclePosition[1]), 
         (Float64) (vehiclePosition[2]));

      
      // ODE's returned rotation matrix is an array arranged
      // as 4 columns and 3 rows, row-major. The final element
      // in each row is an unused zero
      Matrix newRot;
      newRot.set_element (0, 0, Float64 (vehicleRotation[0]));
      newRot.set_element (0, 1, Float64 (vehicleRotation[1]));
      newRot.set_element (0, 2, Float64 (vehicleRotation[2]));
      newRot.set_element (1, 0, Float64 (vehicleRotation[4]));
      newRot.set_element (1, 1, Float64 (vehicleRotation[5]));
      newRot.set_element (1, 2, Float64 (vehicleRotation[6]));
      newRot.set_element (2, 0, Float64 (vehicleRotation[8]));
      newRot.set_element (2, 1, Float64 (vehicleRotation[9]));
      newRot.set_element (2, 2, Float64 (vehicleRotation[10]));

      Vector newVel (
         (Float64) (vehicleLinearVel[0]), 
         (Float64) (vehicleLinearVel[1]), 
         (Float64) (vehicleLinearVel[2]));
      
      _objMod->store_position (_handle, _defaultHandle, newPos);
      _objMod->store_velocity (_handle, _defaultHandle, newVel);
      _objMod->store_orientation (_handle, _defaultHandle, newRot);

      if (newVel.magnitude () > 0.1) {
         _objMod->store_scalar (_handle, _throttleHandle, 2.0);
      }
      else {
         _objMod->store_scalar (_handle, _throttleHandle, 0.8);
      }
   }
}


void
dmz::DynamicsPluginODESimple::stop_plugin () {

}


void
dmz::DynamicsPluginODESimple::shutdown_plugin () {

}


void
dmz::DynamicsPluginODESimple::remove_plugin (const Plugin *PluginPtr) {

   if (_isect && (_isect == RenderModuleIsect::cast (PluginPtr))) { _isect = 0; }
   if (_objMod && (_objMod == ObjectModule::cast (PluginPtr))) { _objMod = 0; }
}


// Object Observer Interface
void
dmz::DynamicsPluginODESimple::update_object_flag (
      const UUID &Identity,
      const ObjectHandle Handle,
      const UInt32 AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if ((AttributeHandle == _hilHandle) && Value) { 
      _handle = Handle; 
      
   }
}


void
dmz::DynamicsPluginODESimple::update_object_scalar (
      const UUID &Identity,
      const ObjectHandle Handle,
      const UInt32 AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {;}


// Input Observer Interface
void
dmz::DynamicsPluginODESimple::input_channels_initialized () {

}


void
dmz::DynamicsPluginODESimple::input_channels_uninitialized () {

}


void
dmz::DynamicsPluginODESimple::update_channel_state (
      const UInt32 Channel,
      const Boolean State) {

}


void
dmz::DynamicsPluginODESimple::receive_axis_event (
      const UInt32 Channel,
      const InputEventAxis &Value) {


   switch (Value.get_axis_id ())
   {
      case 1: { _move.speedAxis = Value.get_axis_value () * _move.moveSpeed; break; }
      case 2: { _move.turnAxis = Value.get_axis_value () * _move.turnRate; break; }
      case 3: { _move.strafeAxis = Value.get_axis_value () * _move.moveSpeed; break; }
      case 4: { _move.pitchAxis = Value.get_axis_value () * _move.turnRate; break; }
   }
}


void
dmz::DynamicsPluginODESimple::receive_button_event (
      const UInt32 Channel,
      const InputEventButton &Value) {

   _log.out << "B" << endl;
}


void
dmz::DynamicsPluginODESimple::receive_key_event (
      const UInt32 Channel,
      const InputEventKey &Value) {

}


void
dmz::DynamicsPluginODESimple::receive_mouse_event (
      const UInt32 Channel,
      const InputEventMouse &Value) {

}


void
dmz::DynamicsPluginODESimple::receive_switch_event (
      const UInt32 Channel,
      const InputEventSwitch &Value) {

}


dWorldID
dmz::DynamicsPluginODESimple::get_world_id () { return _world; }


dJointGroupID
dmz::DynamicsPluginODESimple::get_joint_group_id () { return _contactGroup; }


dmz::Float32
dmz::DynamicsPluginODESimple::get_erp () { return _ERP; }


dmz::Float32
dmz::DynamicsPluginODESimple::get_cfm () { return _CFM; }


void
dmz::DynamicsPluginODESimple::_ode_collision_callback (
      void* data,
      dGeomID o1,
      dGeomID o2) {

   const int MAX_CONTACTS = 50; // maximum number of contact points per body

   DynamicsPluginODESimple *odePlugin = (DynamicsPluginODESimple *) (data);

   if (odePlugin) {


      dBodyID b1 = dGeomGetBody(o1);
      dBodyID b2 = dGeomGetBody(o2);

      if (b1 && b2 && dAreConnected (b1, b2)) {

         // exit without doing anything if the two bodies are connected by a joint
      }
      else if (dGeomIsSpace(o1) || dGeomIsSpace(o2)) {


         dSpaceCollide2(o1, o2, data, &_ode_collision_callback);
      }
      else {

         dContact contact[MAX_CONTACTS];
         for (int i = 0; i < MAX_CONTACTS; i++) {

            // See ODE docs for info about collision modes, constants, and
            // settings

            // taken from ode_basket demo code
            contact[i].surface.slip1 = dReal (0.7);
            contact[i].surface.slip2 = dReal (0.7);

            // Alternative 1:
            // contact[i].surface.mode = dContactBounce| dContactSoftCFM;

            // Alternative 2:

            //contact[i].surface.mode = dContactBounce | dContactSoftCFM |
            //  dContactSoftERP;

            // Alternative 3 (taken from ode_basket demo code)
            contact[i].surface.mode = 
               dContactSoftERP | 
               dContactSoftCFM | 
               dContactApprox1 | 
               dContactSlip1 | 
               dContactSlip2;

            // taken from ode_basket demo code
            contact[i].surface.mu = dReal (50.0); // was dInfinity;
            contact[i].surface.soft_erp = dReal (odePlugin->get_erp ());
            contact[i].surface.soft_cfm = dReal (odePlugin->get_cfm ());

         }

         int numc = dCollide (o1, o2, MAX_CONTACTS, &contact[0].geom, sizeof (dContact));

         if (numc != 0) {

            for (int i = 0; i < numc; i++) {
               
               dJointID c = dJointCreateContact (odePlugin->get_world_id (),
                                                 odePlugin->get_joint_group_id (),
                                                 contact+i);
               dJointAttach (c, b1, b2);
            }
         }
      }
   }
}

void
dmz::DynamicsPluginODESimple::_init (Config &local) {

   Config movement;

   if (local.lookup_all_data ("movement", movement)) {

      ConfigIterator it;
      Config cd;

      Boolean found (movement.get_first_data (it, cd));

      if (found)  {
         _move.moveSpeed = config_to_uint32 ("speed", cd);
         _move.turnRate = config_to_uint32 ("turnRate", cd);
      }
      _log.info << "Loaded movement info" << endl;
   }

   Config channels;

   if (local.lookup_all_data ("channel", channels)) {

      ConfigIterator it;

      Config cd;

      Boolean found (channels.get_first_data (it, cd));

      while (found) {

         const dmz::String ChannelName (dmz::config_to_string ("name", cd));

         if (ChannelName) {

            activate_input_channel (ChannelName, InputEventButtonMask);
            activate_input_channel (ChannelName, InputEventAxisMask);

            _log.info << "Activating input channel: " << ChannelName << endl;
         }

         found = channels.get_next_data (it, cd);
      }
   }
   else {

      activate_default_input_channel (InputEventButtonMask);
      activate_default_input_channel (InputEventAxisMask);

      _log.info << "Activating default input channel" << endl;
   }

   _isectParameters.set_test_result_type (IsectClosestPoint);
   _isectParameters.set_calculate_normal (False);
   _isectParameters.set_calculate_object_handle (False);
   _isectParameters.set_calculate_distance (True);
   _isectParameters.set_calculate_cull_mode (False);


   Config data;
   if (local.lookup_data ("gravity", data)) {
   
      _gravity = config_to_float32 ("value", data, -0.5);
   }

   if (local.lookup_data ("simulation", data)) {

      _CFM = config_to_float32 ("constraintForceMixing", data, 0.1f);
      _ERP = config_to_float32 ("errorReductionParameter", data, 0.9f);
      _maxTimeStep = config_to_float32 ("maxTimeStep", data, 0.01f);
   }


   if (local.lookup_data ("vehicle", data)) {

      _vehicleInitialPos = config_to_vector ("", data,  Vector (0.0, 0.0, 0.0));
      _vehicleTotalMass = config_to_float32 ("mass", data, 100.0f);
      _vehicleLength = config_to_float32 ("length", data, 0.5f);
      _vehicleForceScale = config_to_float32 ("force", data, 8000.0f);
      
   }

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzDynamicsPluginODESimple (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   const dmz::String InputModuleName (
      dmz::config_to_string ("input_module.name", local));

   return new dmz::DynamicsPluginODESimple (Info, InputModuleName, local);
}

};
