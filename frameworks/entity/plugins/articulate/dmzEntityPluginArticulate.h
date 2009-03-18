#ifndef DMZ_ENTITY_PLUGIN_ARTICULATE_DOT_H
#define DMZ_ENTITY_PLUGIN_ARTICULATE_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesConsts.h>
#include <dmzTypesHashTableUInt32Template.h>

namespace dmz {

   class EntityPluginArticulate :
         public Plugin,
         public TimeSlice,
         public InputObserverUtil,
         public ObjectObserverUtil {

      public:
         EntityPluginArticulate (const PluginInfo &Info, Config &local);
         ~EntityPluginArticulate ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State);

         virtual void receive_axis_event (
            const Handle Channel,
            const InputEventAxis &Value);

         virtual void receive_button_event (
            const Handle Channel,
            const InputEventButton &Value);

         // Object Observer Interface
         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

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

      protected:
         struct ComponentStruct {

            const Handle AttrHandle;
            Float64 axis;
            Float64 rate;
            Boolean limit;
            Float64 min;
            Float64 max;

            ComponentStruct (const Handle TheAttrHandle) :
                  AttrHandle (TheAttrHandle),
                  axis (0.0),
                  rate (Pi64),
                  limit (False),
                  min (1.0),
                  max (-1.0) {;}
         };

         void _init (Config &local);

         Log _log;

         Handle _hil;
         Handle _hilAttrHandle;

         HashTableUInt32Template<ComponentStruct> _compTable;

      private:
         EntityPluginArticulate ();
         EntityPluginArticulate (const EntityPluginArticulate &);
         EntityPluginArticulate &operator= (const EntityPluginArticulate &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_ARTICULATE_DOT_H
