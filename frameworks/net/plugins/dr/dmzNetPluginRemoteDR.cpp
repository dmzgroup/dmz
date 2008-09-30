#include "dmzNetPluginRemoteDR.h"
#include <dmzObjectConsts.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesVector.h>

/*!

\class dmz::NetPluginRemoteDR
\ingroup Net
\brief Performs basic linear dead-reckoning of remote objects using the last network value
times stamp, velocity, and position.
\code
CurrentPosition = LastNetworkValuePosition + (LastNetworkValueVelocity * (CurrentTime - LastNetworkValueTimeStamp))
\endcode

*/

//! \cond
dmz::NetPluginRemoteDR::NetPluginRemoteDR (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _time (Info.get_context ()),
      _objMod (0),
      _defaultHandle (0),
      _lnvHandle (0) {

   _init (local);

   _defaultHandle = activate_object_attribute (
      ObjectAttributeDefaultName,
      ObjectCreateMask | ObjectDestroyMask | ObjectLocalityMask);

   _lnvHandle = activate_object_attribute (
      ObjectAttributeLastNetworkValueName,
      ObjectCreateMask);
}


dmz::NetPluginRemoteDR::~NetPluginRemoteDR () {

   _objTable.clear ();
}


// Plugin Interface
void
dmz::NetPluginRemoteDR::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_objMod) { _objMod = ObjectModule::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_objMod && (_objMod == ObjectModule::cast (PluginPtr))) { _objMod = 0; }
   }
}


// TimeSlice Interface
void
dmz::NetPluginRemoteDR::update_time_slice (const Float64 TimeDelta) {

   if (_objMod && _lnvHandle && _defaultHandle) {

      const Float64 CurrentTime (_time.get_frame_time ());

      HashTableHandleIterator it;

      for (
            NetPluginRemoteDR *current = _objTable.get_first (it);
            current;
            current = _objTable.get_next (it)) {

         const Handle ObjectHandle (it.get_hash_key ());

         Vector lnvPos;
         Vector vel;
         Float64 timestamp (0.0);

         if (_objMod->lookup_position (ObjectHandle, _lnvHandle, lnvPos) &&
               _objMod->lookup_velocity (ObjectHandle, _defaultHandle, vel) &&
               _objMod->lookup_time_stamp (ObjectHandle, _lnvHandle, timestamp)) {

            const Vector Pos (lnvPos + (vel * (CurrentTime - timestamp)));
            _objMod->store_position (ObjectHandle, _defaultHandle, Pos);
         }
      }
   }
}


// Object Observer Interface
void
dmz::NetPluginRemoteDR::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (Locality == ObjectRemote) { _objTable.store (ObjectHandle, this); }
}


void
dmz::NetPluginRemoteDR::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   _objTable.remove (ObjectHandle);
}


void
dmz::NetPluginRemoteDR::update_object_locality (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectLocalityEnum Locality,
      const ObjectLocalityEnum PrevLocality) {

   if (Locality == ObjectLocal) { _objTable.remove (ObjectHandle); }
}


void
dmz::NetPluginRemoteDR::_init (Config &local) {

}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzNetPluginRemoteDR (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::NetPluginRemoteDR (Info, local);
}

};
