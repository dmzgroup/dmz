#ifndef DMZ_NET_PLUGIN_REMOTE_SCALAR_DR_DOT_H
#define DMZ_NET_PLUGIN_REMOTE_SCALAR_DR_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzRuntimeTime.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHandleContainer.h>

namespace dmz {

   class NetPluginRemoteScalarDR :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil {

      public:
         //! \cond
         NetPluginRemoteScalarDR (const PluginInfo &Info, Config &local);
         ~NetPluginRemoteScalarDR ();

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
         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_scalar (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue);

      protected:
         struct RateStruct {

            const Handle RateHandle;
            const Handle AttrHandle;
            const Handle LNVAttrHandle;
            const Handle TimeStampHandle;

            RateStruct (
                  const Handle TheRateHandle,
                  const Handle TheAttrHandle,
                  const Handle TheLNVAttrHandle,
                  const Handle TheTimeStampHandle) :
                  RateHandle (TheRateHandle),
                  AttrHandle (TheAttrHandle),
                  LNVAttrHandle (TheLNVAttrHandle),
                  TimeStampHandle (TheTimeStampHandle) {;}
         };

         struct ValueStruct {

            Float64 rate;
            RateStruct &rs;

            ValueStruct (RateStruct &theRS) : rs (theRS), rate (0.0) {;}
         };

         struct ObjectStruct {

            const Handle ObjectHandle;
            HashTableHandleTemplate<ValueStruct> table;

            ObjectStruct (const Handle TheObjectHandle) :
                  ObjectHandle (TheObjectHandle) {;}

            ~ObjectStruct () { table.empty (); }
         };

         void _init (Config &local);

         Log _log;
         Time _time;

         HandleContainer _ignoreObjects;
         HashTableHandleTemplate<RateStruct> _rateTable;
         HashTableHandleTemplate<ObjectStruct> _objTable;
         //! \endcond

      private:
         NetPluginRemoteScalarDR ();
         NetPluginRemoteScalarDR (const NetPluginRemoteScalarDR &);
         NetPluginRemoteScalarDR &operator= (const NetPluginRemoteScalarDR &);

   };
};

#endif // DMZ_NET_PLUGIN_REMOTE_SCALAR_DR_DOT_H
