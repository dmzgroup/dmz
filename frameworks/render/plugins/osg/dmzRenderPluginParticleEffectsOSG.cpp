#include <dmzObjectAttributeMasks.h>
//#include <dmzRenderConfigToOSG.h>
#include <dmzRenderModuleCoreOSG.h>
#include "dmzRenderPluginParticleEffectsOSG.h"
#include <dmzRenderConfigToOSG.h>
#include <dmzRenderUtilOSG.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <osgParticle/SmokeEffect>
#include <osgParticle/FireEffect>
#include <osgParticle/FluidProgram>

#include <qdb.h>
static dmz::qdb out;

namespace {

typedef dmz::RenderPluginParticleEffectsOSG::ParticleStateFactory ParticleStateFactory;

const dmz::Vector Gravity (0.0, -9.18, 0.0);

class SmokeStateFactory : public ParticleStateFactory {

   public:
      SmokeStateFactory (
         const dmz::Mask &State,
         const dmz::String &TextureName,
         const dmz::Vector &Offset,
         const dmz::Config &Init);

      virtual osgParticle::ParticleEffect *create_effect ();

   protected:
      const dmz::String _TextureName;
      const dmz::Vector _Offset;
      dmz::Float64 _scale;
      dmz::Float64 _intensity;
      dmz::Float64 _timeOut;
      dmz::Boolean _setSize;
      osgParticle::rangev4 _colorRange;
      osgParticle::rangef _alphaRange;
      osgParticle::rangef _sizeRange;
};

SmokeStateFactory::SmokeStateFactory (
      const dmz::Mask &State,
      const dmz::String &TextureName,
      const dmz::Vector &Offset,
      const dmz::Config &Init) :
      ParticleStateFactory (State),
      _TextureName (TextureName),
      _Offset (Offset),
      _scale (5.0),
      _intensity (1.0),
      _timeOut (20.0),
      _setSize (dmz::False),
      _colorRange (
         osg::Vec4 (0.1, 0.1, 0.1, 1.0), osg::Vec4 (0.0, 0.0, 0.0, 1.0)),
      _alphaRange (1.0f, 0.1f) {

   _scale = dmz::config_to_float64 ("scale", Init, _scale);
   _intensity = dmz::config_to_float64 ("intensity", Init, _intensity);

   osg::Vec4 min = dmz::config_to_osg_vec4_color (
      "start-color",
      Init,
      _colorRange.minimum);

   osg::Vec4 max = dmz::config_to_osg_vec4_color (
      "end-color",
      Init,
      _colorRange.maximum);

   _colorRange.set (min, max);

   dmz::Config sizeRange;

   if (Init.lookup_config ("size-range", sizeRange)) {

      dmz::Float32 Min = config_to_float32 ("min", sizeRange, 1.0);
      dmz::Float32 Max = config_to_float32 ("max", sizeRange, 2.0);

      _sizeRange.set (Min, Max);
      _setSize = dmz::True;
   }

   _alphaRange.set (
      dmz::config_to_float32 ("alpha-range.min", Init, 0.5),
      dmz::config_to_float32 ("alpha-range.max", Init, 0.1));

   _timeOut = dmz::config_to_float64 ("time-out", Init, _timeOut);
}


osgParticle::ParticleEffect *
SmokeStateFactory::create_effect () {

   osgParticle::SmokeEffect *result (
      new osgParticle::SmokeEffect (dmz::to_osg_vector (_Offset), _scale, _intensity));

   if (result) {

      if (_TextureName) { result->setTextureFileName (_TextureName.get_buffer ()); }
      result->setUseLocalParticleSystem (false);

      osgParticle::FluidProgram *program = dynamic_cast<osgParticle::FluidProgram *> (
         result->getProgram ());

      if (program) { program->setAcceleration (dmz::to_osg_vector (Gravity)); }

      const osgParticle::Particle &OrigParticle = result->getDefaultParticleTemplate ();

      osgParticle::Particle particle (OrigParticle);

      particle.setLifeTime (_timeOut);

      particle.setColorRange (_colorRange);
      particle.setAlphaRange (_alphaRange);

      if (_setSize) { particle.setSizeRange (_sizeRange); }

      result->setDefaultParticleTemplate (particle);
   }

   return result;
}


class FireStateFactory : public ParticleStateFactory {

   public:
      FireStateFactory (
         const dmz::Mask &State,
         const dmz::String &TextureName,
         const dmz::Vector &Offset,
         const dmz::Config &Init);

      virtual osgParticle::ParticleEffect *create_effect ();

   protected:
      const dmz::String _TextureName;
      const dmz::Vector _Offset;
      dmz::Float64 _scale;
      dmz::Float64 _intensity;
};


FireStateFactory::FireStateFactory (
      const dmz::Mask &State,
      const dmz::String &TextureName,
      const dmz::Vector &Offset,
      const dmz::Config &Init) :
      ParticleStateFactory (State),
      _TextureName (TextureName),
      _Offset (Offset),
      _scale (5.0),
      _intensity (1.0) {

   _scale = dmz::config_to_float64 ("scale", Init, _scale);
   _intensity = dmz::config_to_float64 ("intensity", Init, _intensity);
}


osgParticle::ParticleEffect *
FireStateFactory::create_effect () {

   osgParticle::FireEffect *result (
      new osgParticle::FireEffect (dmz::to_osg_vector (_Offset), _scale, _intensity));

   if (result) {

      if (_TextureName) { result->setTextureFileName (_TextureName.get_buffer ()); }
      result->setUseLocalParticleSystem (false);

      osgParticle::FluidProgram *program = dynamic_cast<osgParticle::FluidProgram *> (
         result->getProgram ());

      if (program) { program->setAcceleration (dmz::to_osg_vector (Gravity)); }
   }

   return result;
}


class DustStateFactory : public ParticleStateFactory {

   public:
      DustStateFactory (
         const dmz::Mask &State,
         const dmz::String &TextureName,
         const dmz::Vector &Offset);

      virtual osgParticle::ParticleEffect *create_effect ();

   protected:
      const dmz::String _TextureName;
      const dmz::Vector _Offset;
};

DustStateFactory::DustStateFactory (
      const dmz::Mask &State,
      const dmz::String &TextureName,
      const dmz::Vector &Offset) :
      ParticleStateFactory (State),
      _TextureName (TextureName),
      _Offset (Offset) {;}


osgParticle::ParticleEffect *
DustStateFactory::create_effect () {

   osgParticle::SmokeEffect *result (
      new osgParticle::SmokeEffect (dmz::to_osg_vector (_Offset), 10.0, 1.0));

   if (result) {

      if (_TextureName) { result->setTextureFileName (_TextureName.get_buffer ()); }
      result->setUseLocalParticleSystem (false);

      osgParticle::FluidProgram *program = dynamic_cast<osgParticle::FluidProgram *> (
         result->getProgram ());

      if (program) { program->setAcceleration (osg::Vec3 (0.0, 0.0, 0.0)); }

      const osgParticle::Particle &OrigParticle = result->getDefaultParticleTemplate ();

      osgParticle::Particle particle (OrigParticle);

      particle.setLifeTime (5.0);

      osgParticle::rangev4 cr (
         osg::Vec4 (0.54, 0.45, 0.38, 1.0), osg::Vec4 (0.56, 0.5, 0.42, 1.0));

      particle.setColorRange (cr);

      osgParticle::rangef sr (0.3, 6.0);
      particle.setSizeRange (sr);

      osgParticle::rangef ar (0.4, 0.0);
      particle.setAlphaRange (ar);

      result->setDefaultParticleTemplate (particle);
   }

   return result;
}

};


void
dmz::RenderPluginParticleEffectsOSG::ParticleStateStruct::enable (
      const Boolean Enable) {

   if (effect.valid ()) {

      osgParticle::Emitter *em = effect->getEmitter ();

      if (em) {

         em->setEnabled (Enable);
         em->setEndless (true);
      }
   }
}


void
dmz::RenderPluginParticleEffectsOSG::ParticleStateStruct::endless () {

   if (effect.valid ()) {

      osgParticle::Emitter *em = effect->getEmitter ();

      if (em) { em->setEndless (true); }
   }
}


dmz::RenderPluginParticleEffectsOSG::RenderPluginParticleEffectsOSG (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      EventObserverUtil (Info, local),
      _log (Info),
      _defs (Info),
      _rc (Info),
      _core (0) {

   _init (local);
}


dmz::RenderPluginParticleEffectsOSG::~RenderPluginParticleEffectsOSG () {

   _objTable.empty ();
   _objDefTable.clear ();
   _objMasterDefTable.empty ();
}


// Plugin Interface
void
dmz::RenderPluginParticleEffectsOSG::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::RenderPluginParticleEffectsOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) {

         _core = RenderModuleCoreOSG::cast (PluginPtr);

         if (_core) { _add_effects (); }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOSG::cast (PluginPtr))) {

         _remove_effects ();

         _core = 0;
      }
   }
}


// TimeSlice Interface
void
dmz::RenderPluginParticleEffectsOSG::update_time_slice (const Float64 TimeDelta) {


}


// Object Observer Interface
void
dmz::RenderPluginParticleEffectsOSG::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (!_ignoreTypes.contains_exact_type (Type)) {

      ObjectDefStruct *ods = _lookup_object_def (Type);

      if (ods) {

         ObjectStruct *os = new ObjectStruct;

         if (os && _objTable.store (ObjectHandle, os)) {

            ParticleStateFactory *current (ods->list);

            while (current) {

               ParticleStateStruct *pss = new ParticleStateStruct (current->State);

               if (pss) {

                  pss->effect = current->create_effect ();

                  if (pss->effect.valid ()) {

                     pss->effect->setWind (to_osg_vector (_wind));

                     pss->enable (False);

                     os->geode->addDrawable (pss->effect->getParticleSystem ());

                     pss->next = os->list;
                     os->list = pss;
                  }
                  else { delete pss; pss = 0; }
               }

               current = current->next;
            }

            _add_effect (ObjectHandle, *os);
         }
         else if (os) { delete os; os = 0; }
      }
      else { _ignoreTypes.add_object_type (Type); }
   }
}


void
dmz::RenderPluginParticleEffectsOSG::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   ObjectStruct *os = _objTable.remove (ObjectHandle);

   if (os) {

      _remove_effect (ObjectHandle, *os);

      delete os; os = 0;
   }
}


void
dmz::RenderPluginParticleEffectsOSG::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   ObjectStruct *os (_objTable.lookup (ObjectHandle));

   if (os) {

      const Mask &PValue (PreviousValue ? *PreviousValue : _EmptyState);

      ParticleStateStruct *current (os->list);

      while (current) {

         const Boolean IsSet (Value.contains (current->State));
         const Boolean WasSet (PValue.contains (current->State));

         if (IsSet && !WasSet) { current->enable (True); }
         if (!IsSet && WasSet) { current->enable (False); }

         current = current->next;
      }
   }
}


void
dmz::RenderPluginParticleEffectsOSG::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

}


void
dmz::RenderPluginParticleEffectsOSG::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

}


void
dmz::RenderPluginParticleEffectsOSG::update_object_velocity (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

}


void
dmz::RenderPluginParticleEffectsOSG::update_object_vector (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   _wind = Value;

   HashTableHandleIterator it;
   ObjectStruct *os (0);

   const osg::Vec3 WindOSG = to_osg_vector (_wind);

   while (_objTable.get_next (it, os)) {

      ParticleStateStruct *pss = os->list;

      while (pss) {

         if (pss->effect.valid ()) {

            pss->effect->setWind (WindOSG);
            // note: setting the wind seems to turn endless smoke off so re-enable it.
            pss->endless ();
         }

         pss = pss->next;
      }
   }
}


// Event Observer Interface
void
dmz::RenderPluginParticleEffectsOSG::create_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

}


void
dmz::RenderPluginParticleEffectsOSG::close_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

}

dmz::RenderPluginParticleEffectsOSG::ObjectDefStruct *
dmz::RenderPluginParticleEffectsOSG::_lookup_object_def (const ObjectType &Type) {

   ObjectDefStruct *result (_objDefTable.lookup (Type.get_handle ()));
   
   if (!result) { result = _create_object_def (Type); }

   return result;
}


dmz::RenderPluginParticleEffectsOSG::ObjectDefStruct *
dmz::RenderPluginParticleEffectsOSG::_create_object_def (const ObjectType &Type) {

   ObjectDefStruct *result (0);

   ObjectType current (Type);

   while (!result && current) {

      result = _objMasterDefTable.lookup (current.get_handle ());

      if (!result) {

         Config fxList;

         if (current.get_config ().lookup_all_config ("render.particle-effect", fxList)) {

            result = new ObjectDefStruct;

            if (result && _objMasterDefTable.store (current.get_handle (), result)) {

               ConfigIterator it;
               Config fx;

               while (fxList.get_next_config (it, fx)) {

                  ParticleStateFactory *factory (0);

                  const String Name = config_to_string ("type", fx);

                  if (Name == "smoking") { factory = _create_smoke_state_factory (fx); }
                  else if (Name == "fire") { factory = _create_fire_state_factory (fx); }
                  else if (Name == "dust-trail") {

                     factory = _create_dust_state_factory (fx);
                  }
                  else {

                     _log.error << "Unknown object state particle effect type: "
                        << Name << " for object type: " << current.get_name () << endl;
                  }

                  if (factory) { factory->next = result->list; result->list = factory; }
               }
            }
         }
      }

      current.become_parent ();
   }

   if (result) { _objDefTable.store (Type.get_handle (), result); }
  
   return result;
}


dmz::RenderPluginParticleEffectsOSG::ParticleStateFactory *
dmz::RenderPluginParticleEffectsOSG::_create_smoke_state_factory (Config &fx) {

   ParticleStateFactory *result (0);

   const String StateName = config_to_string ("state.name", fx, DefaultStateNameSmoking);

   Mask state;

   if (_defs.lookup_state (StateName, state)) {

      const String Resource = config_to_string ("texture.resource", fx, "smoke");
      const String Texture = Resource ? _rc.find_file (Resource) : String ();
      const Vector Offset (config_to_vector ("offset", fx, Vector (0.0, 2.5, 0.0)));

      result = new SmokeStateFactory (state, Texture, Offset, fx);
   }
   else {

      _log.error << "Unknown smoking state name: " << StateName << endl;
   }

   return result;
}


dmz::RenderPluginParticleEffectsOSG::ParticleStateFactory *
dmz::RenderPluginParticleEffectsOSG::_create_fire_state_factory (Config &fx) {

   ParticleStateFactory *result (0);

   const String StateName = config_to_string ("state.name", fx, DefaultStateNameFire);

   Mask state;

   if (_defs.lookup_state (StateName, state)) {

      const String Resource = config_to_string ("texture.resource", fx, "fire");
      const String Texture = Resource ? _rc.find_file (Resource) : String ();
      const Vector Offset (config_to_vector ("offset", fx, Vector (0.0, 1.5, 0.0)));

      result = new FireStateFactory (state, Texture, Offset, fx);
   }
   else {

      _log.error << "Unknown fire state name: " << StateName << endl;
   }

   return result;
}


dmz::RenderPluginParticleEffectsOSG::ParticleStateFactory *
dmz::RenderPluginParticleEffectsOSG::_create_dust_state_factory (Config &fx) {

   ParticleStateFactory *result (0);

   const String StateName =
      config_to_string ("state.name", fx, DefaultStateNameDustTrail);

   Mask state;

   if (_defs.lookup_state (StateName, state)) {

      const String Resource = config_to_string ("texture.resource", fx, "dust-trail");
      const String Texture = Resource ? _rc.find_file (Resource) : String ();
      const Vector Offset (config_to_vector ("offset", fx, Vector (0.0, 0.0, 3.0)));

      result = new DustStateFactory (state, Texture, Offset);
   }
   else {

      _log.error << "Unknown dust state name: " << StateName << endl;
   }

   return result;
}


void
dmz::RenderPluginParticleEffectsOSG::_add_effect (
      const Handle ObjectHandle,
      ObjectStruct &os) {

   if (_core) {

      osg::Group *root = _core->get_dynamic_objects ();

      if (root && os.geode.valid ()) { root->addChild (os.geode.get ()); }

      osg::Group *obj = _core->create_dynamic_object (ObjectHandle);

      if (obj) {

         ParticleStateStruct *pss = os.list;

         while (pss) {

            if (pss->effect.valid ()) { obj->addChild (pss->effect.get ()); }
            pss = pss->next;
         }
      }
   }
}


void
dmz::RenderPluginParticleEffectsOSG::_add_effects () {

   HashTableHandleIterator it;
   ObjectStruct *os (0);

   while (_objTable.get_next (it, os)) { _add_effect (it.get_hash_key (), *os); }
}


void
dmz::RenderPluginParticleEffectsOSG::_remove_effect (
      const Handle ObjectHandle,
      ObjectStruct &os) {

   if (_core) {

      osg::Group *root = _core->get_dynamic_objects ();

      if (root && os.geode.valid ()) { root->removeChild (os.geode.get ()); }

      osg::Group *obj = _core->lookup_dynamic_object (ObjectHandle);

      if (obj) {

         ParticleStateStruct *pss = os.list;

         while (pss) {

            if (pss->effect.valid ()) { obj->removeChild (pss->effect.get ()); }
            pss = pss->next;
         }
      }
   }
}


void
dmz::RenderPluginParticleEffectsOSG::_remove_effects () {

   HashTableHandleIterator it;
   ObjectStruct *os (0);

   while (_objTable.get_next (it, os)) { _remove_effect (it.get_hash_key (), *os); }
}


void
dmz::RenderPluginParticleEffectsOSG::_init (Config &local) {

   activate_default_object_attribute (
      ObjectCreateMask |
      ObjectDestroyMask |
      ObjectStateMask);

   activate_object_attribute (
      config_to_string ("wind-attribute.name", local, "Env_Wind_Direction"),
      ObjectVectorMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginParticleEffectsOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginParticleEffectsOSG (Info, local);
}

};
