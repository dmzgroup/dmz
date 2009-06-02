#ifndef DMZ_RENDER_PLUGIN_PARTICLE_EFFECTS_OSG_DOT_H
#define DMZ_RENDER_PLUGIN_PARTICLE_EFFECTS_OSG_DOT_H

#include <dmzEventObserverUtil.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeResources.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesMask.h>

#include <osg/Geode>
#include <osgParticle/ParticleEffect>

namespace dmz {

   class RenderModuleCoreOSG;

   class RenderPluginParticleEffectsOSG :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil,
         public EventObserverUtil {

      public:
         class ParticleStateFactory {

            public:
               const Mask State;
               ParticleStateFactory *next;
               virtual ~ParticleStateFactory () { if (next) { delete next; next = 0; } }
               virtual osgParticle::ParticleEffect *create_effect () = 0;

            protected:
               ParticleStateFactory (const Mask &TheState) :
                     State (TheState),
                     next (0) {;}
         };

         RenderPluginParticleEffectsOSG (const PluginInfo &Info, Config &local);
         ~RenderPluginParticleEffectsOSG ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

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

         virtual void update_object_position (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_orientation (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Matrix &Value,
            const Matrix *PreviousValue);

         virtual void update_object_velocity (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         // Event Observer Interface
         virtual void create_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality);

         virtual void close_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality);

      protected:
         struct ObjectDefStruct {

            ParticleStateFactory *list;
            ObjectDefStruct () : list (0) {;}
            ~ObjectDefStruct () { if (list) { delete list; list = 0; } }
         };

         struct ParticleStateStruct {

            const Mask State;
            ParticleStateStruct *next;
            osg::ref_ptr<osgParticle::ParticleEffect> effect;

            ParticleStateStruct (const Mask &TheState) : State (TheState), next (0) {;}
            ~ParticleStateStruct () { if (next) { delete next; next = 0; } }

            void enable (const Boolean Enable);
         };

         struct ObjectStruct {

            ParticleStateStruct *list;
            osg::ref_ptr<osg::Geode> geode;

            ObjectStruct () : list (0) { geode = new osg::Geode; }
            ~ObjectStruct () { if (list) { delete list; list = 0; } }
         };

         ObjectDefStruct *_lookup_object_def (const ObjectType &Type);
         ObjectDefStruct *_create_object_def (const ObjectType &Type);
         ParticleStateFactory *_create_smoke_state_factory (Config &fx);
         ParticleStateFactory *_create_fire_state_factory (Config &fx);
         ParticleStateFactory *_create_dust_state_factory (Config &fx);

         void _init (Config &local);

         const Mask _EmptyState;

         Log _log;
         Definitions _defs;
         Resources _rc;

         RenderModuleCoreOSG *_core;

         ObjectTypeSet _ignoreTypes;
         HashTableHandleTemplate<ObjectDefStruct> _objDefTable;
         HashTableHandleTemplate<ObjectDefStruct> _objMasterDefTable;
         HashTableHandleTemplate<ObjectStruct> _objTable;

      private:
         RenderPluginParticleEffectsOSG ();
         RenderPluginParticleEffectsOSG (const RenderPluginParticleEffectsOSG &);
         RenderPluginParticleEffectsOSG &operator= (
            const RenderPluginParticleEffectsOSG &);

   };
};

#endif // DMZ_RENDER_PLUGIN_PARTICLE_EFFECTS_OSG_DOT_H
