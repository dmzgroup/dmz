#ifndef DMZ_ENTITY_PLUGIN_WHEELS_DOT_H
#define DMZ_ENTITY_PLUGIN_WHEELS_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesDeleteListTemplate.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class EntityPluginWheels :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil {

      public:
         //! \cond
         EntityPluginWheels (const PluginInfo &Info, Config &local);
         ~EntityPluginWheels ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Time Slice Interface
         virtual void update_time_slice (const Float64 DeltaTime);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

      protected:
         struct WheelStruct {

            const Handle Attr;
            const Float64 InvertRadius;
            const Float64 Mod;
            WheelStruct *next;

            WheelStruct (
                  const Handle TheAttr,
                  const Float64 TheInvertRadius,
                  const Float64 TheMod) :
                  Attr (TheAttr),
                  InvertRadius (TheInvertRadius),
                  Mod (TheMod),
                  next (0) {;}

            ~WheelStruct () { delete_list (next); }
         };

         struct ObjectStruct {

            const Handle Object;
            WheelStruct *wheels;
            ObjectStruct (const Handle TheObject, WheelStruct *theWheels) :
                  Object (TheObject),
                  wheels (theWheels) {;}
         };

         WheelStruct *_lookup_wheels_def (const ObjectType &Type);
         WheelStruct *_create_wheels_def (const ObjectType &Type);
         void _init (Config &local);

         Log _log;
         Definitions _defs;

         Handle _defaultAttr;

         HashTableHandleTemplate<WheelStruct> _wheelTable;
         HashTableHandleTemplate<ObjectStruct> _objTable;
         //! \endcond

      private:
         EntityPluginWheels ();
         EntityPluginWheels (const EntityPluginWheels &);
         EntityPluginWheels &operator= (const EntityPluginWheels &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_WHEELS_DOT_H
