#include <dmzEventConsts.h>
#include "dmzNetExtPacketCodecEventNative.h"
#include <dmzObjectModule.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeUUID.h>
#include <dmzSystemMarshal.h>
#include <dmzSystemUnmarshal.h>
#include <dmzTypesArrays.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesMask.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesUUID.h>
#include <dmzTypesVector.h>

/*!

\class dmz::NetExtPacketCodecEventNative
\ingroup Net
\brief Encodes and decodes events for the network.
\details
\code
<local-scope>
   <adapter
      type="Adapter Type"
      attribute="Attribute Name"
   />
   ...
</local-scope>
\endcode
Possible types are: id, object-type, state, position, orientation, velocity, acceleration, scale, vector, scalar, timestamp, and text. Data object are not currently supported.

*/

//! \cond
dmz::NetExtPacketCodecEventNative::NetExtPacketCodecEventNative (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      NetExtPacketCodecEvent (Info),
      _SysID (get_runtime_uuid (Info)),
      _log (Info),
      _time (Info),
      _defaultHandle (0),
      _lnvHandle (0),
      _eventMod (0),
      _attrMod (0),
      _adapterList (0) {

   _init (local);
}


dmz::NetExtPacketCodecEventNative::~NetExtPacketCodecEventNative () {

   if (_adapterList) { delete _adapterList; _adapterList = 0; }
}


// Plugin Interface
void
dmz::NetExtPacketCodecEventNative::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::NetExtPacketCodecEventNative::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_eventMod) { _eventMod = EventModule::cast (PluginPtr); }
      if (!_attrMod) { _attrMod = NetModuleAttributeMap::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_eventMod && (_eventMod == EventModule::cast (PluginPtr))) { _eventMod = 0; }

      if (_attrMod && (_attrMod == NetModuleAttributeMap::cast (PluginPtr))) {

         _attrMod = 0;
      }
   }

   EventAttributeAdapter *current (_adapterList);

   while (current) {

      current->discover_plugin (Mode, PluginPtr);
      current = current->next;
   }
}


// NetExtPacketCodecEvent Interface
dmz::Boolean
dmz::NetExtPacketCodecEventNative::decode (Unmarshal &data, Boolean &isLoopback) {

   Boolean result (False);

   UUID uuid;
   data.get_next_uuid (uuid);

   isLoopback = (uuid == _SysID);

   if (!isLoopback && _eventMod) {

      const Int32 TypeSize (Int32 (data.get_next_int8 ()));

      ArrayUInt32 typeArray (TypeSize);

      for (Int32 ix = 0; ix < TypeSize; ix++) {

         typeArray.set (ix, UInt32 (data.get_next_uint8 ()));
      }

      Handle eventHandle (0);

      EventType type;
      _attrMod->to_internal_event_type (typeArray, type);

      if (type) { eventHandle = _eventMod->create_event (type, EventRemote); }

      if (eventHandle) {

         result = True;

         EventAttributeAdapter *current (_adapterList);

         while (current) {

            current->decode (eventHandle, data, *_eventMod);
            current = current->next;
         }

         _eventMod->close_event (eventHandle);
      }
   }

   return result;
}


dmz::Boolean
dmz::NetExtPacketCodecEventNative::encode_event (
      const Handle EventHandle,
      Marshal &data) {

   Boolean result (False);

   if (_attrMod && _eventMod) {

      data.set_next_uuid (_SysID);

      EventType type;
      ArrayUInt32 typeArray;
      _eventMod->lookup_event_type (EventHandle, type);

      if (_attrMod->to_net_event_type (type, typeArray)) {

         const Int32 TypeSize (typeArray.get_size ());
         data.set_next_int8 (Int8 (TypeSize));

         for (Int32 ix = 0; ix < TypeSize; ix++) {

            data.set_next_uint8 (UInt8 (typeArray.get (ix)));
         }

         EventAttributeAdapter *current (_adapterList);

         while (current) {

            current->encode (EventHandle, *_eventMod, data);
            current = current->next;
         }

         result = True;
      }
   }

   return result;
}


void
dmz::NetExtPacketCodecEventNative::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());
   Definitions defs (context, &_log);

   _defaultHandle = defs.create_named_handle (EventAttributeDefaultName);

   Config adapters;
   EventAttributeAdapter *current (0);

   if (local.lookup_all_config ("adapter", adapters)) {

      ConfigIterator it;
      Config data;

      while (adapters.get_next_config (it, data)) {

         EventAttributeAdapter *next (create_event_adapter (data, context, _log));

         if (next) {

            if (current) { current->next = next; current = next; }
            else { _adapterList = current = next; }
         }
      }
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzNetExtPacketCodecEventNative (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::NetExtPacketCodecEventNative (Info, local);
}

};


namespace {

typedef dmz::NetExtPacketCodecEventNative::EventAttributeAdapter Adapter;

static dmz::Handle
local_create_attribute_handle (dmz::Config &local, dmz::RuntimeContext *context) {

   return  dmz::Definitions (context).create_named_handle (
      config_to_string ("attribute", local, dmz::EventAttributeDefaultName));
}


class ObjectID : public Adapter {

   public:
      ObjectID (dmz::Config &local, dmz::RuntimeContext *context);

      virtual void discover_plugin (
         const dmz::PluginDiscoverEnum Mode,
         const dmz::Plugin *PluginPtr);

      virtual void decode (
         const dmz::Handle EventHandle,
         dmz::Unmarshal &data,
         dmz::EventModule &eventMod);

      virtual void encode (
         const dmz::Handle EventHandle,
         dmz::EventModule &eventMod,
         dmz::Marshal &data);

   protected:
      dmz::ObjectModule *_objMod;

};


ObjectID::ObjectID (dmz::Config &local, dmz::RuntimeContext *context) :
      Adapter (local, context),
      _objMod (0) {;}


void
ObjectID::discover_plugin (
      const dmz::PluginDiscoverEnum Mode,
      const dmz::Plugin *PluginPtr) {

   if (Mode == dmz::PluginDiscoverAdd) {

      if (!_objMod) { _objMod = dmz::ObjectModule::cast (PluginPtr); }
   }
   else if (Mode == dmz::PluginDiscoverRemove) {

      if (_objMod && (_objMod == dmz::ObjectModule::cast (PluginPtr))) { _objMod = 0; }
   }
}


void
ObjectID::decode (
      const dmz::Handle EventHandle,
      dmz::Unmarshal &data,
      dmz::EventModule &eventMod) {

   dmz::UUID objectID;

   data.get_next_uuid (objectID);

   if (_objMod) {

      const dmz::Handle Value (_objMod->lookup_handle_from_uuid (objectID));
      eventMod.store_object_handle (EventHandle, _AttributeHandle, Value);
   }
}


void
ObjectID::encode (
      const dmz::Handle EventHandle,
      dmz::EventModule &eventMod,
      dmz::Marshal &data) {

   dmz::Handle value (0);
   eventMod.lookup_object_handle (EventHandle, _AttributeHandle, value);

   dmz::UUID objectID;

   if (_objMod) {

      _objMod->lookup_uuid (value, objectID);
   }

   data.set_next_uuid (objectID);
}


class ObjectTypeAttr : public Adapter {

   public:
      ObjectTypeAttr (dmz::Config &local, dmz::RuntimeContext *context);

      virtual void discover_plugin (
         const dmz::PluginDiscoverEnum Mode,
         const dmz::Plugin *PluginPtr);

      virtual void decode (
         const dmz::Handle EventHandle,
         dmz::Unmarshal &data,
         dmz::EventModule &eventMod);

      virtual void encode (
         const dmz::Handle EventHandle,
         dmz::EventModule &eventMod,
         dmz::Marshal &data);

   protected:
      dmz::NetModuleAttributeMap *_attrMod;

};


ObjectTypeAttr::ObjectTypeAttr (dmz::Config &local, dmz::RuntimeContext *context) :
      Adapter (local, context),
      _attrMod (0) {;}


void
ObjectTypeAttr::discover_plugin (
      const dmz::PluginDiscoverEnum Mode,
      const dmz::Plugin *PluginPtr) {

   if (Mode == dmz::PluginDiscoverAdd) {

      if (!_attrMod) { _attrMod = dmz::NetModuleAttributeMap::cast (PluginPtr); }
   }
   else if (Mode == dmz::PluginDiscoverRemove) {

      if (_attrMod && (_attrMod == dmz::NetModuleAttributeMap::cast (PluginPtr))) {

         _attrMod = 0;
      }
   }
}


void
ObjectTypeAttr::decode (
      const dmz::Handle EventHandle,
      dmz::Unmarshal &data,
      dmz::EventModule &eventMod) {

   const dmz::Int32 Size (data.get_next_int8 ());

   dmz::ArrayUInt32 typeArray (Size);

   for (dmz::Int32 ix = 0; ix < Size; ix++) {

      typeArray.set (ix, data.get_next_uint8 ());
   }

   if (_attrMod) {

      dmz::ObjectType type;
      _attrMod->to_internal_object_type (typeArray, type);

      eventMod.store_object_type (EventHandle, _AttributeHandle, type);
   }
}


void
ObjectTypeAttr::encode (
      const dmz::Handle EventHandle,
      dmz::EventModule &eventMod,
      dmz::Marshal &data) {

   dmz::ArrayUInt32 typeArray;

   if (_attrMod) {

      dmz::ObjectType type;
      eventMod.lookup_object_type (EventHandle, _AttributeHandle, type);
      _attrMod->to_net_object_type (type, typeArray);
   }

   const dmz::Int32 Size (typeArray.get_size ());

   data.set_next_int8 (dmz::Int8 (Size));

   for (dmz::Int32 ix = 0; ix < Size; ix++) {

      data.set_next_uint8 (dmz::UInt8 (typeArray.get (ix)));
   }
}


class State : public Adapter {

   public:
      State (dmz::Config &local, dmz::RuntimeContext *context);

      virtual void discover_plugin (
         const dmz::PluginDiscoverEnum Mode,
         const dmz::Plugin *PluginPtr);

      virtual void decode (
         const dmz::Handle EventHandle,
         dmz::Unmarshal &data,
         dmz::EventModule &eventMod);

      virtual void encode (
         const dmz::Handle EventHandle,
         dmz::EventModule &eventMod,
         dmz::Marshal &data);

   protected:
      dmz::NetModuleAttributeMap *_attrMod;

};


State::State (dmz::Config &local, dmz::RuntimeContext *context) :
      Adapter (local, context),
      _attrMod (0) {;}


void
State::discover_plugin (
      const dmz::PluginDiscoverEnum Mode,
      const dmz::Plugin *PluginPtr) {

   if (Mode == dmz::PluginDiscoverAdd) {

      if (!_attrMod) { _attrMod = dmz::NetModuleAttributeMap::cast (PluginPtr); }
   }
   else if (Mode == dmz::PluginDiscoverRemove) {

      if (_attrMod && (_attrMod == dmz::NetModuleAttributeMap::cast (PluginPtr))) {

         _attrMod = 0;
      }
   }
}


void
State::decode (
      const dmz::Handle EventHandle,
      dmz::Unmarshal &data,
      dmz::EventModule &eventMod) {

   const dmz::Int32 Size (data.get_next_int8 ());

   dmz::ArrayUInt32 stateArray (Size);

   for (dmz::Int32 ix = 0; ix < Size; ix++) {

      stateArray.set (ix, data.get_next_uint32 ());
   }

   if (_attrMod) {

      dmz::EventType type;
      eventMod.lookup_event_type (EventHandle, type);
      dmz::Mask value;
      _attrMod->to_internal_event_mask (type, stateArray, value);

      eventMod.store_state (EventHandle, _AttributeHandle, value);
   }
}


void
State::encode (
      const dmz::Handle EventHandle,
      dmz::EventModule &eventMod,
      dmz::Marshal &data) {

   dmz::ArrayUInt32 stateArray;

   if (_attrMod) {

      dmz::EventType type;
      eventMod.lookup_event_type (EventHandle, type);
      dmz::Mask value;
      eventMod.lookup_state (EventHandle, _AttributeHandle, value);

      _attrMod->to_net_event_mask (type, value, stateArray);
   }

   const dmz::Int32 Size (stateArray.get_size ());

   data.set_next_int8 (dmz::Int8 (Size));

   for (dmz::Int32 ix = 0; ix < Size; ix++) {

      data.set_next_uint32 (stateArray.get (ix));
   }
}


class TimeStamp : public Adapter {

   public:
      TimeStamp (dmz::Config &local, dmz::RuntimeContext *context) :
            Adapter (local, context) {;}

      virtual void decode (
         const dmz::Handle EventHandle,
         dmz::Unmarshal &data,
         dmz::EventModule &eventMod);

      virtual void encode (
         const dmz::Handle EventHandle,
         dmz::EventModule &eventMod,
         dmz::Marshal &data);
};


void
TimeStamp::decode (
      const dmz::Handle EventHandle,
      dmz::Unmarshal &data,
      dmz::EventModule &eventMod) {

   const dmz::Float64 Value (data.get_next_float64 ());
   eventMod.store_time_stamp (EventHandle, _AttributeHandle, Value);
}


void
TimeStamp::encode (
      const dmz::Handle EventHandle,
      dmz::EventModule &eventMod,
      dmz::Marshal &data) {

   dmz::Float64 value (0.0);
   eventMod.lookup_time_stamp (EventHandle, _AttributeHandle, value);
   data.set_next_float64 (value);
}


class Position : public Adapter {

   public:
      Position (dmz::Config &local, dmz::RuntimeContext *context) :
            Adapter (local, context) {;}

      virtual void decode (
         const dmz::Handle EventHandle,
         dmz::Unmarshal &data,
         dmz::EventModule &eventMod);

      virtual void encode (
         const dmz::Handle EventHandle,
         dmz::EventModule &eventMod,
         dmz::Marshal &data);
};


void
Position::decode (
      const dmz::Handle EventHandle,
      dmz::Unmarshal &data,
      dmz::EventModule &eventMod) {

   dmz::Vector value;
   data.get_next_vector (value);
   eventMod.store_position (EventHandle, _AttributeHandle, value);
}


void
Position::encode (
      const dmz::Handle EventHandle,
      dmz::EventModule &eventMod,
      dmz::Marshal &data) {

   dmz::Vector value;
   eventMod.lookup_position (EventHandle, _AttributeHandle, value);
   data.set_next_vector (value);
}


class Orientation : public Adapter {

   public:
      Orientation (dmz::Config &local, dmz::RuntimeContext *context) :
            Adapter (local, context) {;}

      virtual void decode (
         const dmz::Handle EventHandle,
         dmz::Unmarshal &data,
         dmz::EventModule &eventMod);

      virtual void encode (
         const dmz::Handle EventHandle,
         dmz::EventModule &eventMod,
         dmz::Marshal &data);
};


void
Orientation::decode (
      const dmz::Handle EventHandle,
      dmz::Unmarshal &data,
      dmz::EventModule &eventMod) {

   dmz::Matrix value;
   data.get_next_matrix (value);
   eventMod.store_orientation (EventHandle, _AttributeHandle, value);
}


void
Orientation::encode (
      const dmz::Handle EventHandle,
      dmz::EventModule &eventMod,
      dmz::Marshal &data) {

   dmz::Matrix value;
   eventMod.lookup_orientation (EventHandle, _AttributeHandle, value);
   data.set_next_matrix (value);
}


class Velocity : public Adapter {

   public:
      Velocity (dmz::Config &local, dmz::RuntimeContext *context) :
            Adapter (local, context) {;}

      virtual void decode (
         const dmz::Handle EventHandle,
         dmz::Unmarshal &data,
         dmz::EventModule &eventMod);

      virtual void encode (
         const dmz::Handle EventHandle,
         dmz::EventModule &eventMod,
         dmz::Marshal &data);
};


void
Velocity::decode (
      const dmz::Handle EventHandle,
      dmz::Unmarshal &data,
      dmz::EventModule &eventMod) {

   dmz::Vector value;
   data.get_next_vector (value);
   eventMod.store_velocity (EventHandle, _AttributeHandle, value);
}


void
Velocity::encode (
      const dmz::Handle EventHandle,
      dmz::EventModule &eventMod,
      dmz::Marshal &data) {

   dmz::Vector value;
   eventMod.lookup_velocity (EventHandle, _AttributeHandle, value);
   data.set_next_vector (value);
}


class Acceleration : public Adapter {

   public:
      Acceleration (dmz::Config &local, dmz::RuntimeContext *context) :
            Adapter (local, context) {;}

      virtual void decode (
         const dmz::Handle EventHandle,
         dmz::Unmarshal &data,
         dmz::EventModule &eventMod);

      virtual void encode (
         const dmz::Handle EventHandle,
         dmz::EventModule &eventMod,
         dmz::Marshal &data);
};


void
Acceleration::decode (
      const dmz::Handle EventHandle,
      dmz::Unmarshal &data,
      dmz::EventModule &eventMod) {

   dmz::Vector value;
   data.get_next_vector (value);
   eventMod.store_acceleration (EventHandle, _AttributeHandle, value);
}


void
Acceleration::encode (
      const dmz::Handle EventHandle,
      dmz::EventModule &eventMod,
      dmz::Marshal &data) {

   dmz::Vector value;
   eventMod.lookup_acceleration (EventHandle, _AttributeHandle, value);
   data.set_next_vector (value);
}


class Scale : public Adapter {

   public:
      Scale (dmz::Config &local, dmz::RuntimeContext *context) :
            Adapter (local, context) {;}

      virtual void decode (
         const dmz::Handle EventHandle,
         dmz::Unmarshal &data,
         dmz::EventModule &eventMod);

      virtual void encode (
         const dmz::Handle EventHandle,
         dmz::EventModule &eventMod,
         dmz::Marshal &data);
};


void
Scale::decode (
      const dmz::Handle EventHandle,
      dmz::Unmarshal &data,
      dmz::EventModule &eventMod) {

   dmz::Vector value;
   data.get_next_vector (value);
   eventMod.store_scale (EventHandle, _AttributeHandle, value);
}


void
Scale::encode (
      const dmz::Handle EventHandle,
      dmz::EventModule &eventMod,
      dmz::Marshal &data) {

   dmz::Vector value;
   eventMod.lookup_scale (EventHandle, _AttributeHandle, value);
   data.set_next_vector (value);
}


class VectorAttr : public Adapter {

   public:
      VectorAttr (dmz::Config &local, dmz::RuntimeContext *context) :
            Adapter (local, context) {;}

      virtual void decode (
         const dmz::Handle EventHandle,
         dmz::Unmarshal &data,
         dmz::EventModule &eventMod);

      virtual void encode (
         const dmz::Handle EventHandle,
         dmz::EventModule &eventMod,
         dmz::Marshal &data);
};


void
VectorAttr::decode (
      const dmz::Handle EventHandle,
      dmz::Unmarshal &data,
      dmz::EventModule &eventMod) {

   dmz::Vector value;
   data.get_next_vector (value);
   eventMod.store_vector (EventHandle, _AttributeHandle, value);
}


void
VectorAttr::encode (
      const dmz::Handle EventHandle,
      dmz::EventModule &eventMod,
      dmz::Marshal &data) {

   dmz::Vector value;
   eventMod.lookup_vector (EventHandle, _AttributeHandle, value);
   data.set_next_vector (value);
}


class Scalar : public Adapter {

   public:
      Scalar (dmz::Config &local, dmz::RuntimeContext *context) :
            Adapter (local, context) {;}

      virtual void decode (
         const dmz::Handle EventHandle,
         dmz::Unmarshal &data,
         dmz::EventModule &eventMod);

      virtual void encode (
         const dmz::Handle EventHandle,
         dmz::EventModule &eventMod,
         dmz::Marshal &data);
};


void
Scalar::decode (
      const dmz::Handle EventHandle,
      dmz::Unmarshal &data,
      dmz::EventModule &eventMod) {

   const dmz::Float64 Value (data.get_next_float64 ());
   eventMod.store_scalar (EventHandle, _AttributeHandle, Value);
}


void
Scalar::encode (
      const dmz::Handle EventHandle,
      dmz::EventModule &eventMod,
      dmz::Marshal &data) {

   dmz::Float64 value (0.0);
   eventMod.lookup_scalar (EventHandle, _AttributeHandle, value);
   data.set_next_float64 (value);
}


class Text : public Adapter {

   public:
      Text (dmz::Config &local, dmz::RuntimeContext *context) :
            Adapter (local, context) {;}

      virtual void decode (
         const dmz::Handle EventHandle,
         dmz::Unmarshal &data,
         dmz::EventModule &eventMod);

      virtual void encode (
         const dmz::Handle EventHandle,
         dmz::EventModule &eventMod,
         dmz::Marshal &data);
};


void
Text::decode (
      const dmz::Handle EventHandle,
      dmz::Unmarshal &data,
      dmz::EventModule &eventMod) {

   dmz::String value;
   data.get_next_string (value);
   eventMod.store_text (EventHandle, _AttributeHandle, value);
}


void
Text::encode (
      const dmz::Handle EventHandle,
      dmz::EventModule &eventMod,
      dmz::Marshal &data) {

   dmz::String value;
   eventMod.lookup_text (EventHandle, _AttributeHandle, value);
   data.set_next_string (value);
}


// TODO Still need a Data Adapter
};


dmz::NetExtPacketCodecEventNative::EventAttributeAdapter::EventAttributeAdapter (
      Config &local,
      RuntimeContext *context) :
      next (0),
      _AttributeHandle (local_create_attribute_handle (local, context)) {;}


dmz::NetExtPacketCodecEventNative::EventAttributeAdapter::~EventAttributeAdapter () {

   if (next) { delete next; next = 0; }
}


dmz::NetExtPacketCodecEventNative::EventAttributeAdapter *
dmz::NetExtPacketCodecEventNative::create_event_adapter (
      Config &local,
      RuntimeContext *context,
      Log &log) {

   Adapter *result (0);

   const String Type = config_to_string ("type", local).to_lower ();

   if (Type == "id") { result = new ObjectID (local, context); }
   else if (Type == "object-type") { result = new ObjectTypeAttr (local, context); }
   else if (Type == "state") { result = new State (local, context); }
   else if (Type == "position") { result = new Position (local, context); }
   else if (Type == "orientation") { result = new Orientation (local, context); }
   else if (Type == "velocity") { result = new Velocity (local, context); }
   else if (Type == "acceleration") { result = new Acceleration (local, context); }
   else if (Type == "scale") { result = new Scale (local, context); }
   else if (Type == "vector") { result = new VectorAttr (local, context); }
   else if (Type == "scalar") { result = new Scalar(local, context); }
   else if (Type == "timestamp") { result = new TimeStamp (local, context); }
   else if (Type == "text") { result = new Text (local, context); }
   else {

      log.error << "Unknown event attribute adapter type: " << Type << endl;
   }

   return result;
}
//! \endcond
