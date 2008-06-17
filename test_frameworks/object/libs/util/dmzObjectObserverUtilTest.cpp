#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include "dmzObjectObserverUtilTest.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzTypesVector.h>


dmz::ObjectObserverUtilTest::ObjectObserverUtilTest (
      const PluginInfo &Info,
      Config &local,
      Config &global) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      test (Info.get_name (), Info.get_context ()),
      _log (Info.get_name (), Info.get_context ()),
      _type ("rootType", Info.get_context ()),
      _objMod (0),
      _defaultHandle (0) {

   _defaultHandle = activate_default_object_attribute (ObjectPositionMask);
}


dmz::ObjectObserverUtilTest::~ObjectObserverUtilTest () {;}


// TimeSlice Interface
void
dmz::ObjectObserverUtilTest::update_time_slice (const Float64 TimeDelta) {

   _objMod = get_object_module ();

   if (_objMod) {

_log.out << "Discovered object module! " << _type.get_name () << endl;

     Handle handle = _objMod->create_object (_type, ObjectLocal);

     Vector pos (1.0, 2.0, 3.0);
     Vector pos2 (4.0, 5.0, 6.0);

     Definitions defs (get_plugin_runtime_context ());

_log.out << "ObjectHandle: " << defs.lookup_runtime_name (handle) << endl;
_log.out << "PluginHandle: " << defs.lookup_runtime_name (get_plugin_handle ()) << endl;
_log.out << "ObjectType: " << defs.lookup_runtime_name (_type.get_handle ()) << endl;

     if (handle) {

_log.out << "Created object with handle " << handle << endl;

         _objMod->activate_object (handle);
         _objMod->store_position (handle, _defaultHandle, pos);
         _objMod->store_position (handle, _defaultHandle, pos2);
     }
   }

   test.exit ("Test completed");
}


void
dmz::ObjectObserverUtilTest::store_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Position,
      const Vector *LastPosition) {

   String value ("<NULL>");
   if (LastPosition) { value.flush () << *LastPosition; }
   _log.out << "Got postion[" << _defaultHandle << "]: " << Position << " last position:"
      << value << endl;
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzObjectObserverUtilTest (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ObjectObserverUtilTest (Info, local, global);
}

};


