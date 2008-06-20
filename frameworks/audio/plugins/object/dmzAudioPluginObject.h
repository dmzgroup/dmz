#ifndef DMZ_AUDIO_PLUGIN_OBJECT_DOT_H
#define DMZ_AUDIO_PLUGIN_OBJECT_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesMask.h>
#include <dmzTypesVector.h>

namespace dmz {
   class AudioModule;
   class ObjectModule;

   class AudioPluginObject :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil {

      public:
         //! \cond
         AudioPluginObject (
            const PluginInfo &Info,
            Config &local);

         ~AudioPluginObject ();

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
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_type (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const ObjectType &Value,
            const ObjectType *PreviousValue);

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

         virtual void update_object_velocity (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_scalar (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue);

      protected:
         struct SoundDefStruct {

            const Mask State;

            String activateName;
            String loopName;
            String deactivateName;

            Handle activateHandle;
            Handle loopHandle;
            Handle deactivateHandle;

            Handle scalarAttributeHandle;

            Float64 offset;
            Float64 scale;

            SoundDefStruct *next;

            SoundDefStruct (const Mask &TheState) :
               State (TheState),
               activateHandle (0),
               loopHandle (0),
               deactivateHandle (0),
               scalarAttributeHandle (0),
               offset (0.0),
               scale (1.0),
               next (0) {;}

            ~SoundDefStruct () { if (next) { delete next; next = 0; } }
         };

         struct SoundStruct {

            const SoundDefStruct &Data;
            Handle handle;
            Float64 scale;

            SoundStruct *next;

            SoundStruct (const SoundDefStruct &TheData) :
                  Data (TheData),
                  handle (0),
                  scale (1.0),
                  next (0) {

               if (Data.next) { next = new SoundStruct (*(Data.next)); }
            }

            ~SoundStruct () { if (next) { delete next; next = 0; } }
         };

         struct ObjectStruct {

            Vector pos;
            Vector vel;

            SoundStruct *list;

            ObjectStruct () : list (0) {;}

            ~ObjectStruct () {

               if (list) { delete list; list = 0; }
            }
         };

         virtual void _remove_object_module (ObjectModule &objMod);

         SoundDefStruct *_get_sound_list (const ObjectType &Type);
         SoundDefStruct *_object_type_to_sound_list (const ObjectType &Type);
         void _init_sound_struct (Config &soundData, SoundDefStruct &ss);
         void _lookup_sound_handles (SoundDefStruct &ss);

         void _update_object (
            const Handle ObjectHandle,
            const ObjectType &ObjType,
            const Mask &ObjState);

         void _stop_all_looped_sounds ();
         void _stop_looped_sounds (ObjectStruct &obj);

         void _init (Config &local);

         AudioModule *_audioMod;

         Log _log;
         Definitions _defs;

         Handle _defaultHandle;

         HashTableHandleTemplate<SoundDefStruct> _soundTable;
         HashTableHandleTemplate<ObjectStruct> _objectTable;
         HashTableHandleTemplate<ObjectStruct> _updateTable;
         //! \endcond

      private:
         AudioPluginObject ();
         AudioPluginObject (const AudioPluginObject &);
         AudioPluginObject &operator= (const AudioPluginObject &);
   };
};

#endif // DMZ_AUDIO_PLUGIN_OBJECT_DOT_H

