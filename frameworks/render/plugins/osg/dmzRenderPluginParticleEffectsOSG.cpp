#include <dmzObjectAttributeMasks.h>
#include <dmzRenderConfigToOSG.h>
#include <dmzRenderModuleCoreOSG.h>
#include "dmzRenderPluginParticleEffectsOSG.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <osgParticle/SmokeEffect>
#include <osgParticle/FireEffect>
#include <osgParticle/FluidProgram>

#include <qdb.h>
static dmz::qdb out;

namespace {

typedef dmz::RenderPluginParticleEffectsOSG::ParticleStateFactory ParticleStateFactory;

const osg::Vec3 Gravity (0.0, -9.18, 0.0);

class SmokeStateFactory : public ParticleStateFactory {

   public:
      SmokeStateFactory (
         const dmz::Mask &State,
         const dmz::String &TextureName,
         const osg::Vec3 Offset);

      virtual osgParticle::ParticleEffect *create_effect ();

   protected:
      const dmz::String _TextureName;
      const osg::Vec3 _Offset;
};

SmokeStateFactory::SmokeStateFactory (
      const dmz::Mask &State,
      const dmz::String &TextureName,
      const osg::Vec3 Offset) :
      ParticleStateFactory (State),
      _TextureName (TextureName),
      _Offset (Offset) {;}

osgParticle::ParticleEffect *
SmokeStateFactory::create_effect () {

   osgParticle::SmokeEffect *result (new osgParticle::SmokeEffect (_Offset, 5.0, 1.0));

   if (result) {

      if (_TextureName) { result->setTextureFileName (_TextureName.get_buffer ()); }
      result->setUseLocalParticleSystem (false);

      osgParticle::FluidProgram *program = dynamic_cast<osgParticle::FluidProgram *> (
         result->getProgram ());

      if (program) { program->setAcceleration (Gravity); }

      const osgParticle::Particle &OrigParticle = result->getDefaultParticleTemplate ();

      osgParticle::Particle particle (OrigParticle);

      particle.setLifeTime (20.0);

      osgParticle::rangev4 cr (
         osg::Vec4 (0.1, 0.1, 0.1, 1.0), osg::Vec4 (0.0, 0.0, 0.0, 0.1));
      particle.setColorRange (cr);
      result->setDefaultParticleTemplate (particle);
   }

   return result;
}


class FireStateFactory : public ParticleStateFactory {

   public:
      FireStateFactory (
         const dmz::Mask &State,
         const dmz::String &TextureName,
         const osg::Vec3 Offset);

      virtual osgParticle::ParticleEffect *create_effect ();

   protected:
      const dmz::String _TextureName;
      const osg::Vec3 _Offset;
};

FireStateFactory::FireStateFactory (
      const dmz::Mask &State,
      const dmz::String &TextureName,
      const osg::Vec3 Offset) :
      ParticleStateFactory (State),
      _TextureName (TextureName),
      _Offset (Offset) {;}


osgParticle::ParticleEffect *
FireStateFactory::create_effect () {

   osgParticle::FireEffect *result (new osgParticle::FireEffect (_Offset, 5.0, 1.0));

   if (result) {

      if (_TextureName) { result->setTextureFileName (_TextureName.get_buffer ()); }
      result->setUseLocalParticleSystem (false);

      osgParticle::FluidProgram *program = dynamic_cast<osgParticle::FluidProgram *> (
         result->getProgram ());

      if (program) { program->setAcceleration (Gravity); }
   }

   return result;
}

};


void
dmz::RenderPluginParticleEffectsOSG::ParticleStateStruct::enable (
      const Boolean Enable) {

   if (effect.valid ()) {

      osgParticle::Emitter *em = effect->getEmitter ();

      if (em) { em->setEnabled (Enable); }
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

      if (!_core) { _core = RenderModuleCoreOSG::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOSG::cast (PluginPtr))) { _core = 0; }
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

   if (_core && !_ignoreTypes.contains_exact_type (Type)) {

      ObjectDefStruct *ods = _lookup_object_def (Type);

      if (ods) {

         osg::Group *root = _core->get_dynamic_objects ();
         osg::Group *obj = _core->lookup_dynamic_object (ObjectHandle);

         ObjectStruct *os = new ObjectStruct;

         if (obj && root && os && _objTable.store (ObjectHandle, os)) {

            ParticleStateFactory *current (ods->list);

            while (current) {

               ParticleStateStruct *pss = new ParticleStateStruct (current->State);

               if (pss) {

                  pss->effect = current->create_effect ();

                  if (pss->effect.valid ()) {

                     pss->enable (False);

                     obj->addChild (pss->effect.get ());
                     os->geode->addDrawable (pss->effect->getParticleSystem ());

                     pss->next = os->list;
                     os->list = pss;
                  }
                  else { delete pss; pss = 0; }
               }

               current = current->next;
            }

            root->addChild (os->geode.get ());
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

      if (_core) {

         osg::Group *root = _core->get_dynamic_objects ();

         if (root) { root->removeChild (os->geode.get ()); }
      }

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
      const osg::Vec3 Offset (
         config_to_osg_vec3 ("offset", fx, osg::Vec3 (0.0, 2.5, 0.0)));

      result = new SmokeStateFactory (state, Texture, Offset);
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
      const osg::Vec3 Offset (
         config_to_osg_vec3 ("offset", fx, osg::Vec3 (0.0, 1.5, 0.0)));

      result = new FireStateFactory (state, Texture, Offset);
   }
   else {

      _log.error << "Unknown fire state name: " << StateName << endl;
   }

   return result;
}

void
dmz::RenderPluginParticleEffectsOSG::_init (Config &local) {

   activate_default_object_attribute (
      ObjectCreateMask |
      ObjectDestroyMask |
      ObjectStateMask);
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
