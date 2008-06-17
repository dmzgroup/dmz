#include "dmzNetPluginRemoteTimeout.h"
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::NetPluginRemoteTimeout::NetPluginRemoteTimeout (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _time (Info.get_context ()),
      _lnvHandle (0),
      _timeoutInterval (10.0),
      _objMod (0) {

   _init (local);

   activate_default_object_attribute (
      ObjectCreateMask | ObjectDestroyMask | ObjectLocalityMask);

   _lnvHandle = activate_object_attribute (
      ObjectAttributeLastNetworkValueName,
      ObjectTimeStampMask);
}


dmz::NetPluginRemoteTimeout::~NetPluginRemoteTimeout () {

   _objTable.clear ();
}


// Plugin Interface
void
dmz::NetPluginRemoteTimeout::discover_plugin (
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
dmz::NetPluginRemoteTimeout::update_time_slice (const Float64 TimeDelta) {

   if (_lnvHandle && _objMod) {

      const Float64 CurrentTime (_time.get_frame_time ());

      HashTableHandleIterator it;

      ObjStruct *ptr (_objTable.get_first (it));

      while (ptr) {

         if (ptr->isSet && ((CurrentTime - ptr->value) > _timeoutInterval)) {

            _objMod->destroy_object (ptr->ObjectHandle);
         }

         ptr = _objTable.get_next (it);
      }
   }
}


// Object Observer Interface
void
dmz::NetPluginRemoteTimeout::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (Locality == ObjectRemote) {

      ObjStruct *os (new ObjStruct (ObjectHandle));

      if (os && !_objTable.store (ObjectHandle, os)) { delete os; os = 0; }
   }
}


void
dmz::NetPluginRemoteTimeout::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   ObjStruct *os (_objTable.remove (ObjectHandle));

   if (os) { delete os; os = 0; }
}


void
dmz::NetPluginRemoteTimeout::update_object_locality (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectLocalityEnum Locality,
      const ObjectLocalityEnum PrevLocality) {

}


void
dmz::NetPluginRemoteTimeout::update_object_time_stamp (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   if (AttributeHandle == _lnvHandle) {

      ObjStruct *os (_objTable.lookup (ObjectHandle));

      if (os) { os->value = Value; os->isSet = True; }
   }
}


void
dmz::NetPluginRemoteTimeout::_init (Config &local) {

   _timeoutInterval = config_to_float64 ("timeout.value", local, _timeoutInterval);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzNetPluginRemoteTimeout (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::NetPluginRemoteTimeout (Info, local);
}

};
