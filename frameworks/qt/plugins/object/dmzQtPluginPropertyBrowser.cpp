#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzQtConfigRead.h>
#include "dmzQtPluginPropertyBrowser.h"
#include "dmzQtPropertyBrowser.h"
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesUUID.h>


dmz::QtPluginPropertyBrowser::QtPluginPropertyBrowser (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      MessageObserver (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defs (Info, &_log),
      _defaultAttrHandle (0),
      _browser (0) {

   _init (local);
   
   _browser = new QtPropertyBrowser (*this, Info, local);
}


dmz::QtPluginPropertyBrowser::~QtPluginPropertyBrowser () {

   if (_browser) { delete _browser; _browser = 0; }
}


// Plugin Interface
void
dmz::QtPluginPropertyBrowser::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

      if (_browser) { _browser->show (); }
   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

      if (_browser) { _browser->hide (); }
   }
}


void
dmz::QtPluginPropertyBrowser::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Message Observer Interface
void
dmz::QtPluginPropertyBrowser::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

}


// Object Observer Interface
void
dmz::QtPluginPropertyBrowser::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {


   if (_browser) {
      
      _browser->create_object (Identity, ObjectHandle, Type, Locality);
   }
}

void
dmz::QtPluginPropertyBrowser::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   if (_browser) {

      _browser->destroy_object (Identity, ObjectHandle);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_uuid (
      const Handle ObjectHandle,
      const UUID &Identity,
      const UUID &PrevIdentity) {

   if (_browser) {
   
      _browser->update_object_uuid (ObjectHandle, Identity, PrevIdentity);
   }
}


void
dmz::QtPluginPropertyBrowser::remove_object_attribute (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &AttrMask) {

   if (_browser) {

      _browser->remove_object_attribute (
         Identity, ObjectHandle, AttributeHandle, AttrMask);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_locality (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectLocalityEnum Locality,
      const ObjectLocalityEnum PrevLocality) {

   if (_browser) {

      _browser->update_object_locality (Identity, ObjectHandle, Locality, PrevLocality);
   }
}


void
dmz::QtPluginPropertyBrowser::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   if (_browser) {

      _browser->link_objects (
         LinkHandle, AttributeHandle, SuperIdentity, SuperHandle, SubIdentity, SubHandle);
   }
}


void
dmz::QtPluginPropertyBrowser::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   if (_browser) {
      
      _browser->unlink_objects (
         LinkHandle, AttributeHandle, SuperIdentity, SuperHandle, SubIdentity, SubHandle);
   }
}


void
dmz::QtPluginPropertyBrowser::update_link_attribute_object (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle,
      const UUID &AttributeIdentity,
      const Handle AttributeObjectHandle,
      const UUID &PrevAttributeIdentity,
      const Handle PrevAttributeObjectHandle) {


   if (_browser) {
      
      _browser->update_link_attribute_object (
         LinkHandle,
         AttributeHandle,
         SuperIdentity,
         SuperHandle,
         SubIdentity,
         SubHandle,
         AttributeIdentity,
         AttributeObjectHandle,
         PrevAttributeIdentity,
         PrevAttributeObjectHandle);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_counter (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   if (_browser) {

      _browser->update_object_counter (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_counter_minimum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

    if (_browser) {
    
      _browser->update_object_counter_minimum (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
    }
}


void
dmz::QtPluginPropertyBrowser::update_object_counter_maximum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   if (_browser) {

     _browser->update_object_counter_maximum (
        Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_alternate_type (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const ObjectType &Value,
      const ObjectType *PreviousValue) {

   if (_browser) {

      _browser->update_object_alternate_type (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   if (_browser) {

      _browser->update_object_state (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (_browser) {

      _browser->update_object_flag (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_time_stamp (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 &Value,
      const Float64 *PreviousValue) {

   if (_browser) {

      _browser->update_object_time_stamp (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {


   if (_browser) {

      _browser->update_object_position (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

   if (_browser) {

      _browser->update_object_orientation (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_velocity (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (_browser) {

      _browser->update_object_velocity (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_acceleration (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (_browser) {

      _browser->update_object_acceleration (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_scale (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (_browser) {

      _browser->update_object_scale (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_vector (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (_browser) {

      _browser->update_object_vector (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   if (_browser) {

      _browser->update_object_scalar (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_text (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const String &Value,
      const String *PreviousValue) {

   if (_browser) {

      _browser->update_object_text (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}


void
dmz::QtPluginPropertyBrowser::update_object_data (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Data &Value,
      const Data *PreviousValue) {

   if (_browser) {

      _browser->update_object_data (
         Identity, ObjectHandle, AttributeHandle, Value, PreviousValue);
   }
}



void
dmz::QtPluginPropertyBrowser::_init (Config &local) {

   setObjectName (get_plugin_name ().get_buffer ());

   activate_global_object_observer ();
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginPropertyBrowser (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginPropertyBrowser (Info, local);
}

};
