#include "dmzRenderPluginArticulateOSG.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::RenderPluginArticulateOSG::RenderPluginArticulateOSG (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      _log (Info) {

   _init (local);
}


dmz::RenderPluginArticulateOSG::~RenderPluginArticulateOSG () {

}


// Plugin Interface
void
dmz::RenderPluginArticulateOSG::update_plugin_state (
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
dmz::RenderPluginArticulateOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// TimeSlice Interface
void
dmz::RenderPluginArticulateOSG::update_time_slice (const Float64 TimeDelta) {


}


// Object Observer Interface
void
dmz::RenderPluginArticulateOSG::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

}


void
dmz::RenderPluginArticulateOSG::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

}


void
dmz::RenderPluginArticulateOSG::update_object_locality (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectLocalityEnum Locality,
      const ObjectLocalityEnum PrevLocality) {

}


void
dmz::RenderPluginArticulateOSG::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

}


void
dmz::RenderPluginArticulateOSG::update_object_velocity (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

}


void
dmz::RenderPluginArticulateOSG::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   ObjectStruct *obj = _objTable.lookup (ObjectHandle);

   if (obj) {

      AttrStruct *attr = obj->attr.lookup (AttributeHandle);

      ScalarStruct *current = (attr ? attr->scalarList : 0);

      while (current) {

         current->update_scalar (Value, PreviousValue);
         current = current->next;
      }
   }
}


void
dmz::RenderPluginArticulateOSG::_init (Config &local) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginArticulateOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginArticulateOSG (Info, local);
}

};
