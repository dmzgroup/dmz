#include "dmzNetModuleLocalDRBasic.h"
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMask.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

#include <math.h>

namespace {

   enum TestTypeEnum { TestPosition, TestVelocity, TestAcceleration, TestVector };

   class valueTest : public dmz::NetModuleLocalDRBasic::ObjectUpdate {

      public:
         valueTest (
            const dmz::Handle PreviousHandle,
            const dmz::Handle CurrentHandle,
            const dmz::Float64 Diff);

      protected:
         const dmz::Handle _PreviousHandle;
         const dmz::Handle _CurrentHandle;
         const dmz::Float64 _Diff;
   };

   class zeroVelocityTest : public valueTest {

      public:
         zeroVelocityTest (
            const dmz::Handle PreviousHandle,
            const dmz::Handle CurrentHandle);

         dmz::Boolean update_object (
            const dmz::Handle ObjectHandle,
            dmz::ObjectModule &module,
            dmz::Boolean &limitRate);
   };

   class stateTest : public valueTest {

      public:
         stateTest (
            const dmz::Handle PreviousHandle,
            const dmz::Handle CurrentHandle,
            const dmz::Mask &TheState);

         dmz::Boolean update_object (
            const dmz::Handle ObjectHandle,
            dmz::ObjectModule &module,
            dmz::Boolean &limitRate);

      protected:
         const dmz::Mask _StateMask;
   };

   class posSkewTest : public valueTest {

      public:
         posSkewTest (
            const dmz::Handle PreviousHandle,
            const dmz::Handle CurrentHandle,
            const dmz::Time &TheTime,
            const dmz::Float64 Diff);

         dmz::Boolean update_object (
            const dmz::Handle ObjectHandle,
            dmz::ObjectModule &module,
            dmz::Boolean &limitRate);

      protected:
         const dmz::Time &_Time;
   };

   class vectorTest : public valueTest {

      public:
         vectorTest (
            const TestTypeEnum Type,
            const dmz::Handle PreviousHandle,
            const dmz::Handle CurrentHandle,
            const dmz::Float64 Diff);

         dmz::Boolean update_object (
            const dmz::Handle ObjectHandle,
            dmz::ObjectModule &module,
            dmz::Boolean &limitRate);

      protected:
         const TestTypeEnum _Type;
   };

   class oriTest : public valueTest {

      public:
         oriTest (
            const dmz::Handle PreviousHandle,
            const dmz::Handle CurrentHandle,
            const dmz::Float64 Diff);

         dmz::Boolean update_object (
            const dmz::Handle ObjectHandle,
            dmz::ObjectModule &module,
            dmz::Boolean &limitRate);
   };

   class scalarTest : public valueTest {

      public:
         scalarTest (
            const dmz::Handle PreviousHandle,
            const dmz::Handle CurrentHandle,
            const dmz::Float64 Diff);

         dmz::Boolean update_object (
            const dmz::Handle ObjectHandle,
            dmz::ObjectModule &module,
            dmz::Boolean &limitRate);
   };

   class timeTest : public dmz::NetModuleLocalDRBasic::ObjectUpdate {

      public:
         timeTest (
            const dmz::Handle PreviousHandle,
            const dmz::Time &TheTime,
            const dmz::Float64 Diff);

      protected:
         const dmz::Handle _PreviousHandle;
         const dmz::Time &_Time;
         const dmz::Float64 _Diff;
         dmz::Boolean _init;
   };

   class heartbeatTest : public timeTest {

      public:
         heartbeatTest (
            const dmz::Handle PreviousHandle,
            const dmz::Time &TheTime,
            const dmz::Float64 Diff);

         dmz::Boolean update_object (
            const dmz::Handle ObjectHandle,
            dmz::ObjectModule &module,
            dmz::Boolean &limitRate);
   };

   class limitRateTest : public timeTest {

      public:
         limitRateTest (
            const dmz::Handle PreviousHandle,
            const dmz::Time &TheTime,
            const dmz::Float64 Diff);

         dmz::Boolean update_object (
            const dmz::Handle ObjectHandle,
            dmz::ObjectModule &module,
            dmz::Boolean &limitRate);
   };
};


valueTest::valueTest (
      const dmz::Handle PreviousHandle,
      const dmz::Handle CurrentHandle,
      const dmz::Float64 Diff) :
      _PreviousHandle (PreviousHandle),
      _CurrentHandle (CurrentHandle),
      _Diff (Diff) {;}



zeroVelocityTest::zeroVelocityTest (
      const dmz::Handle PreviousHandle,
      const dmz::Handle CurrentHandle) :
      valueTest (PreviousHandle, CurrentHandle, 0.0) {;}


dmz::Boolean
zeroVelocityTest::update_object (
      const dmz::Handle ObjectHandle,
      dmz::ObjectModule &module,
      dmz::Boolean &limitRate) {

   dmz::Boolean result (dmz::False);

   dmz::Vector previous, current;

   const dmz::Boolean FoundPrevious (
      module.lookup_velocity (ObjectHandle, _PreviousHandle, previous));

   const dmz::Boolean FoundCurrent (
      module.lookup_velocity (ObjectHandle, _CurrentHandle, current));

   if (FoundPrevious && FoundCurrent) {

      const dmz::Float64 PMag (previous.magnitude ());
      const dmz::Float64 CMag (current.magnitude ());

      if (dmz::is_zero64 (PMag) && !dmz::is_zero64 (CMag)) { result = dmz::True; }
      else if (!dmz::is_zero64 (PMag) && dmz::is_zero64 (CMag)) { result = dmz::True; }
   }
   else if (FoundCurrent && !FoundPrevious) { result = dmz::True; }

   return result;
}


stateTest::stateTest (
      const dmz::Handle PreviousHandle,
      const dmz::Handle CurrentHandle,
      const dmz::Mask &TheState) :
      valueTest (PreviousHandle, CurrentHandle, 0.0), _StateMask (TheState) {;}


dmz::Boolean
stateTest::update_object (
      const dmz::Handle ObjectHandle,
      dmz::ObjectModule &module,
      dmz::Boolean &limitRate) {

   dmz::Boolean result (dmz::False);

   dmz::Mask previous, current;

   if (module.lookup_state (ObjectHandle, _PreviousHandle, previous) &&
         module.lookup_state (ObjectHandle, _CurrentHandle, current)) {

      if (_StateMask) {

         previous &= _StateMask;
         current &= _StateMask;
      }

      if (previous != current) { result = dmz::True; }
   }

   return result;
}


posSkewTest::posSkewTest (
      const dmz::Handle PreviousHandle,
      const dmz::Handle CurrentHandle,
      const dmz::Time &TheTime,
      const dmz::Float64 Diff) :
      valueTest (PreviousHandle, CurrentHandle, Diff), _Time (TheTime) {;}


dmz::Boolean
posSkewTest::update_object (
      const dmz::Handle ObjectHandle,
      dmz::ObjectModule &module,
      dmz::Boolean &limitRate) {

   dmz::Boolean result (dmz::False);

   dmz::Vector pos, lnvPos, lnvVel;
   
   if (module.lookup_position (ObjectHandle, _CurrentHandle, pos) &&
         module.lookup_position (ObjectHandle, _PreviousHandle, lnvPos) &&
         module.lookup_velocity (ObjectHandle, _PreviousHandle, lnvVel)) {

      const dmz::Float64 FrameTime (_Time.get_frame_time ());

      lnvPos += lnvVel * FrameTime;

      const dmz::Float64 CalcDiff ((pos - lnvPos).magnitude ());

      if (CalcDiff > _Diff) { result = dmz::True; }
   }

   return result;
}


vectorTest::vectorTest (
      const TestTypeEnum Type,
      const dmz::Handle PreviousHandle,
      const dmz::Handle CurrentHandle,
      const dmz::Float64 Diff) :
      valueTest (PreviousHandle, CurrentHandle, Diff),
      _Type (Type) {;}


dmz::Boolean
vectorTest::update_object (
      const dmz::Handle ObjectHandle,
      dmz::ObjectModule &module,
      dmz::Boolean &limitRate) {

   dmz::Boolean result (dmz::False);

   dmz::Vector current, previous;
   dmz::Boolean found (dmz::False);

   if (_Type == TestPosition) {

      found = module.lookup_position (ObjectHandle, _CurrentHandle, current) &&
         module.lookup_position (ObjectHandle, _PreviousHandle, previous);
   }
   else if (_Type == TestVelocity) {

      found = module.lookup_velocity (ObjectHandle, _CurrentHandle, current) &&
         module.lookup_velocity (ObjectHandle, _PreviousHandle, previous);
   }
   else if (_Type == TestAcceleration) {

      found = module.lookup_acceleration (ObjectHandle, _CurrentHandle, current) &&
         module.lookup_acceleration (ObjectHandle, _PreviousHandle, previous);
   }
   else if (_Type == TestVector) {

      found = module.lookup_vector (ObjectHandle, _CurrentHandle, current) &&
         module.lookup_vector (ObjectHandle, _PreviousHandle, previous);
   }

   if (found) {

      if (_Diff < ((current - previous).magnitude ())) { result = dmz::True; }
   }

   return result;
}


oriTest::oriTest (
      const dmz::Handle PreviousHandle,
      const dmz::Handle CurrentHandle,
      const dmz::Float64 Diff) : valueTest (PreviousHandle, CurrentHandle, Diff) {;}


dmz::Boolean
oriTest::update_object (
      const dmz::Handle ObjectHandle,
      dmz::ObjectModule &module,
      dmz::Boolean &limitRate) {

   dmz::Boolean result (dmz::False);

   dmz::Matrix ori, lnvOri;

   if (module.lookup_orientation (ObjectHandle, _PreviousHandle, lnvOri) &&
         module.lookup_orientation (ObjectHandle, _CurrentHandle, ori)) {

      dmz::Vector vec1 (0.0, 0.0, -1.0);
      dmz::Vector vec2 (0.0, 0.0, -1.0);

      ori.transform_vector (vec1);
      lnvOri.transform_vector (vec2);

      if (vec1.get_angle (vec2) > _Diff) { result = dmz::True; }
      else {

         dmz::Vector vec3 (0.0, 1.0, 0.0);
         dmz::Vector vec4 (0.0, 1.0, 0.0);

         ori.transform_vector (vec3);
         lnvOri.transform_vector (vec4);

         if (vec3.get_angle (vec4) > _Diff) { result = dmz::True; }
      }
   }

   return result;
}


scalarTest::scalarTest (
      const dmz::Handle PreviousHandle,
      const dmz::Handle CurrentHandle,
      const dmz::Float64 Diff) : valueTest (PreviousHandle, CurrentHandle, Diff) {;}


dmz::Boolean
scalarTest::update_object (
      const dmz::Handle ObjectHandle,
      dmz::ObjectModule &module,
      dmz::Boolean &limitRate) {

   dmz::Boolean result (dmz::False);

   dmz::Float64 value (0.0), lnv (0.0);

   if (module.lookup_scalar (ObjectHandle, _PreviousHandle, lnv) &&
         module.lookup_scalar (ObjectHandle, _CurrentHandle, value)) {

      if (fabs (value - lnv)  > _Diff) { result = dmz::True; }
      else {

        const dmz::Boolean IsZero1 (dmz::is_zero64 (lnv));
        const dmz::Boolean IsZero2 (dmz::is_zero64 (value));

        if (IsZero1 && !IsZero2) { result = dmz::True; }
        else if (!IsZero1 && IsZero2) { result = dmz::True; }
      }
   }

   return result;
}


timeTest::timeTest (
      const dmz::Handle PreviousHandle,
      const dmz::Time &TheTime,
      const dmz::Float64 Diff) :
      _PreviousHandle (PreviousHandle),
      _Time (TheTime),
      _Diff (Diff),
      _init (dmz::False) {;}


heartbeatTest::heartbeatTest (
      const dmz::Handle PreviousHandle,
      const dmz::Time &TheTime,
      const dmz::Float64 Diff) : timeTest (PreviousHandle, TheTime, Diff) {;}


dmz::Boolean
heartbeatTest::update_object (
      const dmz::Handle ObjectHandle,
      dmz::ObjectModule &module,
      dmz::Boolean &limitRate) {

   dmz::Boolean result (dmz::False);

      dmz::Float64 lnvStamp (0.0);

      if (module.lookup_time_stamp (ObjectHandle, _PreviousHandle, lnvStamp)) {

         if (_Time.get_frame_time () >= (lnvStamp + _Diff)) { result = dmz::True; }
      }

   return result;
}


limitRateTest::limitRateTest (
      const dmz::Handle PreviousHandle,
      const dmz::Time &TheTime,
      const dmz::Float64 Diff) : timeTest (PreviousHandle, TheTime, Diff) {;}


dmz::Boolean
limitRateTest::update_object (
      const dmz::Handle ObjectHandle,
      dmz::ObjectModule &module,
      dmz::Boolean &limitRate) {

   dmz::Float64 lnvStamp (0.0);

   if (module.lookup_time_stamp (ObjectHandle, _PreviousHandle, lnvStamp)) {

      if (_Time.get_frame_time () < (lnvStamp + _Diff)) { limitRate = dmz::True; }
   }

   return dmz::False;
}


dmz::NetModuleLocalDRBasic::NetModuleLocalDRBasic (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      NetModuleLocalDR (Info),
      _log (Info),
      _time (Info.get_context ()),
      _objMod (0),
      _defaultHandle (0) {

   _init (local);
}


dmz::NetModuleLocalDRBasic::~NetModuleLocalDRBasic () {

   if (_defaultTest) { delete _defaultTest; _defaultTest = 0; }

   _baseTable.empty ();
   _typeTable.clear ();
}


// Plugin Interface
void
dmz::NetModuleLocalDRBasic::discover_plugin (const Plugin *PluginPtr) {

   if (!_objMod) {

      _objMod = ObjectModule::cast (PluginPtr);
   }
}


void
dmz::NetModuleLocalDRBasic::start_plugin () {

}


void
dmz::NetModuleLocalDRBasic::stop_plugin () {

}


void
dmz::NetModuleLocalDRBasic::shutdown_plugin () {

}


void
dmz::NetModuleLocalDRBasic::remove_plugin (const Plugin *PluginPtr) {

   if (_objMod && (_objMod == ObjectModule::cast (PluginPtr))) { _objMod = 0; }
}


dmz::Boolean
dmz::NetModuleLocalDRBasic::update_object (const Handle ObjectHandle) {

   Boolean result (False);

   if (_objMod) {

      ObjectType type;

      if (_objMod->lookup_object_type (ObjectHandle, _defaultHandle, type)) {

         ObjectUpdate *test (_typeTable.lookup (type.get_handle ()));

         if (!test) { test = _create_test_from_type (type); }

         Boolean limitRate (False);

         while (test && !result && !limitRate) {

            result = test->update_object (ObjectHandle, *_objMod, limitRate);
            test = test->next;
         }
      }
   }

   return result;
}


void
dmz::NetModuleLocalDRBasic::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _defaultHandle = defs.create_named_handle (ObjectAttributeDefaultName);

   Config defaultList;

   if (local.lookup_all_config_merged ("default", defaultList)) {

      _defaultTest = _create_update_list (defaultList);
   }
   else {

      _log.info << "Using default tests" << endl;

      const Handle PreviousHandle (defs.create_named_handle (
            ObjectAttributeLastNetworkValueName));

      const Handle CurrentHandle (defs.create_named_handle (
            ObjectAttributeDefaultName));

      ObjectUpdate *current = _defaultTest = new heartbeatTest (
         PreviousHandle,
         _time,
         5.0);

      if (current) {

         const Mask EmptyState;

         current->next = new stateTest (
            PreviousHandle,
            CurrentHandle,
            EmptyState);

         if (current->next) { current = current->next; }

         current->next = new zeroVelocityTest (
            PreviousHandle,
            CurrentHandle);

         if (current->next) { current = current->next; }

         current->next = new limitRateTest (
            PreviousHandle,
            _time,
            0.066666666667); // 1/15 of a second max update rate

         if (current->next) { current = current->next; }

         current->next = new posSkewTest (
            PreviousHandle,
            CurrentHandle,
            _time,
            0.25);

         if (current->next) { current = current->next; }

         current->next = new oriTest (
            PreviousHandle,
            CurrentHandle,
            Pi64 / 60.0); // 0.05235987756 == 3 degrees

         if (current->next) { current = current->next; }

         current->next = new scalarTest (
            defs.create_named_handle (
               create_last_network_value_name (ObjectAttributeScalarThrottleName)),
            defs.create_named_handle (ObjectAttributeScalarThrottleName),
            0.01);
      }
   }
}


dmz::NetModuleLocalDRBasic::ObjectUpdate *
dmz::NetModuleLocalDRBasic::_create_update_list (Config &listData) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   ObjectUpdate *head (0), *current (0);

   Config cd;

   ConfigIterator it;

   Boolean found (listData.get_first_config (it, cd));

   while (found) {

      ObjectUpdate *next (0);

      const Handle CurrentHandle (defs.create_named_handle (
         config_to_string ("current", cd, ObjectAttributeDefaultName)));

      const String PreviousName (
         config_to_string ("previous", cd, ObjectAttributeLastNetworkValueName));

      const Handle PreviousHandle (defs.create_named_handle (PreviousName));

      const String Name (cd.get_name ().to_lower ());

      if (Name == "position") {

         next = new vectorTest (
            TestPosition,
            PreviousHandle,
            CurrentHandle,
            config_to_float64 ("value", cd, 0.25));
      }
      else if (Name == "velocity") {

         next = new vectorTest (
            TestVelocity,
            PreviousHandle,
            CurrentHandle,
            config_to_float64 ("value", cd, 0.25));
      }
      else if (Name == "acceleration") {

         next = new vectorTest (
            TestAcceleration,
            PreviousHandle,
            CurrentHandle,
            config_to_float64 ("value", cd, 0.25));
      }
      else if (Name == "vector") {

         next = new vectorTest (
            TestAcceleration,
            PreviousHandle,
            CurrentHandle,
            config_to_float64 ("value", cd, 0.25));
      }
      else if (Name == "orientation") {

         next = new oriTest (
            PreviousHandle,
            CurrentHandle,
            config_to_float64 ("value", cd, 0.25));
      }
      else if (Name == "scalar") {

         String lnvName (PreviousName);

         if (lnvName == ObjectAttributeLastNetworkValueName) {

            lnvName = create_last_network_value_name (lnvName);
         }

         next = new scalarTest (
            defs.create_named_handle (lnvName),
            CurrentHandle,
            config_to_float64 ("value", cd, 0.25));
      }
      else if (Name == "state") {

         Definitions defs (get_plugin_runtime_context (), &_log);

         Mask state;

         defs.lookup_state (config_to_string ("value", cd), state);

         next = new stateTest (PreviousHandle, CurrentHandle, state);
      }
      else if (Name == "skew") {

         next = new posSkewTest (
            PreviousHandle,
            CurrentHandle,
            _time,
            config_to_float64 ("value", cd, 0.25));
      }
      else if (Name == "heartbeat") {

         next = new heartbeatTest (
            PreviousHandle,
            _time,
            config_to_float64 ("value", cd, 5.0));
      }
      else if (Name == "ratelimit") {

         next = new limitRateTest (
            PreviousHandle,
            _time,
            config_to_float64 ("diff", cd, Pi64 / 60.0));
      }

      if (next) {

         if (current) { current->next = next; current = next; }
         else { head = current = next; }
      }

      found = listData.get_next_config (it, cd);
   }

   return head;
}


dmz::NetModuleLocalDRBasic::ObjectUpdate *
dmz::NetModuleLocalDRBasic::_create_test_from_type (const ObjectType &Type) {

   ObjectUpdate *result (0);

   ObjectType current (Type);

   while (current && !result) {

      result = _typeTable.lookup (current.get_handle ());

      if (!result) {

         result = _baseTable.lookup (current.get_handle ());

         if (result) { _typeTable.store (Type.get_handle (), result); }
      }

      if (!result) {

         Config listData;

         if (current.get_config ().lookup_all_config_merged (
               "net.local.rules",
               listData)) {

            result = _create_update_list (listData);

            if (result) {

               if (_baseTable.store (current.get_handle (), result)) {

                  _typeTable.store (Type.get_handle (), result);
               }
            }
         }
         else { current.become_parent (); }
      }
   }

   if (!result) {

      result = _defaultTest;

      if (result) { _typeTable.store (Type.get_handle (), result); }
   }

   return result;
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzNetModuleLocalDRBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::NetModuleLocalDRBasic (Info, local);
}

};
