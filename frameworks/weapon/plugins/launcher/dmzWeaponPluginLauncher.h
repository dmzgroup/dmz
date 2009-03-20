#ifndef DMZ_WEAPON_PLUGIN_LAUNCHER_DOT_H
#define DMZ_WEAPON_PLUGIN_LAUNCHER_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzRuntimeTime.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesMask.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

namespace dmz {

   class WeaponPluginLauncher :
         public Plugin,
         public TimeSlice,
         public MessageObserver,
         public InputObserverUtil,
         public ObjectObserverUtil {

      public:
         //! \cond
         WeaponPluginLauncher (const PluginInfo &Info, Config &local);
         ~WeaponPluginLauncher ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Message Observer Interface
         virtual void receive_message (
            const Message &Type,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State);

         virtual void receive_button_event (
            const Handle Channel,
            const InputEventButton &Value);

         virtual void receive_switch_event (
            const Handle Channel,
            const InputEventSwitch &Value);

         virtual void receive_key_event (
            const Handle Channel,
            const InputEventKey &Value);

         virtual void receive_mouse_event (
            const Handle Channel,
            const InputEventMouse &Value);

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
            const Vector Axis;
            const Vector Offset;
            Float64 angle;
            ComponentStruct *next;

            ComponentStruct (
                  const Handle TheAttrHandle,
                  const Vector &TheAxis,
                  const Vector &TheOffset) :
                  AttrHandle (TheAttrHandle),
                  Axis (TheAxis),
                  Offset (TheOffset),
                  angle (0.0),
                  next (0) {;}

            ~ComponentStruct () { if (next) { delete next; next = 0; } }
         };

         struct LaunchStruct {

            const Handle Source;
            Int32 activeCount;
            Float64 lastLaunchTime;

            LaunchStruct (const Handle TheSource) :
               Source (TheSource),
               activeCount (0),
               lastLaunchTime (0.0) {;}
         };

         LaunchStruct *_get_struct (const Handle Source);
         void _create_munition (const Handle SourceHandle);
         void _init (Config &local);

         Log _log;
         Time _time;

         HashTableHandleTemplate<LaunchStruct> _launchTable;
         HashTableHandleTemplate<ComponentStruct> _componentTable;
         ComponentStruct *_transform;

         Float64 _delay;

         Mask _deadState;

         Boolean _hilActive;
         Handle _hilHandle;
         Handle _hil;

         Handle _defaultHandle;
         Handle _sourceEventHandle;

         ObjectType _ammoType;
         UInt32 _launchButton;
         //! \endcond

      private:
         WeaponPluginLauncher ();
         WeaponPluginLauncher (const WeaponPluginLauncher &);
         WeaponPluginLauncher &operator= (const WeaponPluginLauncher &);

   };
};

#endif // DMZ_WEAPON_PLUGIN_LAUNCHER_DOT_H
