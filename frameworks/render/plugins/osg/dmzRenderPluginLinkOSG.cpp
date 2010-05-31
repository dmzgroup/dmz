#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzRenderConfigToOSG.h>
#include <dmzRenderModuleCoreOSG.h>
#include "dmzRenderPluginLinkOSG.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::RenderPluginLinkOSG::RenderPluginLinkOSG (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      MessageObserver (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defaultAttrHandle (0),
      _render (0) {

   _init (local);
}


dmz::RenderPluginLinkOSG::~RenderPluginLinkOSG () {

   _defTable.empty ();
   _linkTable.empty ();
   _objTable.empty ();
}


// Plugin Interface
void
dmz::RenderPluginLinkOSG::update_plugin_state (
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
dmz::RenderPluginLinkOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_render) { _render = RenderModuleCoreOSG::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_render && (_render == RenderModuleCoreOSG::cast (PluginPtr))) { _render = 0; }
   }
}


// TimeSlice Interface
void
dmz::RenderPluginLinkOSG::update_time_slice (const Float64 TimeDelta) {

}


// Message Observer Interface
void
dmz::RenderPluginLinkOSG::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

}


// Object Observer Interface
void
dmz::RenderPluginLinkOSG::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   ObjectStruct *os = _objTable.remove (ObjectHandle);

   if (os) { delete os; os = 0; }
}


void
dmz::RenderPluginLinkOSG::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   LinkDefStruct *lds = _defTable.lookup (AttributeHandle);

   if (lds) {

      ObjectStruct *super = _lookup_object (SuperHandle);
      ObjectStruct *sub = _lookup_object (SubHandle);

      if (super && sub) {

         LinkStruct *ls = new LinkStruct (LinkHandle, *lds, *super, *sub);

         if (ls && _linkTable.store (LinkHandle, ls)) {

            _create_link (*ls);
         }
         else if (ls) { delete ls; ls = 0; }
      }
   }
}


void
dmz::RenderPluginLinkOSG::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   LinkStruct *ls = _linkTable.remove (LinkHandle);

   if (ls) {

      delete ls; ls = 0;
   }
}


void
dmz::RenderPluginLinkOSG::update_link_attribute_object (
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

}


void
dmz::RenderPluginLinkOSG::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

}


void
dmz::RenderPluginLinkOSG::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

}


void
dmz::RenderPluginLinkOSG::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   ObjectStruct *os = _objTable.lookup (ObjectHandle);

   if (os) {

      os->pos = Value;
   }
}


void
dmz::RenderPluginLinkOSG::update_object_vector (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

}


void
dmz::RenderPluginLinkOSG::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

}


// RenderPluginLinkOSG Interface
dmz::RenderPluginLinkOSG::ObjectStruct *
dmz::RenderPluginLinkOSG::_lookup_object (const Handle Object) {

   ObjectStruct *result = _objTable.lookup (Object);

   if (!result) {

      result = new ObjectStruct (Object);

      if (result && _objTable.store (Object, result)) { 

         ObjectModule *module = get_object_module ();

         if (module) {

            module->lookup_position (Object, _defaultAttrHandle, result->pos);
         }
      }
      else if (result) { delete result; result = 0; }
   }

   return result;
}


void
dmz::RenderPluginLinkOSG::_create_link (LinkStruct &ls) {

   if (_render) {

      osg::ref_ptr<osg::Group> root = _render->get_dynamic_objects ();
   }
}


void
dmz::RenderPluginLinkOSG::_init (Config &local) {

   RuntimeContext *context = get_plugin_runtime_context ();

   Config list;

   if (local.lookup_all_config ("link", list)) {

      ConfigIterator it;
      Config link;

      while (list.get_next_config (it, link)) {

         const Handle Attr = config_to_named_handle (
            "attribute",
            link,
            context);

         const osg::Vec4 Color = config_to_osg_vec4_color (
            link,
            osg::Vec4 (1.0, 1.0, 1.0, 1.0));

         const Float64 Radius = config_to_float64 ("radius", link, 0.1);

         LinkDefStruct *lds = new LinkDefStruct (Attr, Color, Radius);

         if (lds && _defTable.store (Attr, lds)) {

            activate_object_attribute (Attr, ObjectLinkMask | ObjectUnlinkMask);
         }
         else if (lds) { delete lds; lds = 0; }
      }
   }

   _defaultAttrHandle = activate_default_object_attribute (
      ObjectDestroyMask | ObjectPositionMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginLinkOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginLinkOSG (Info, local);
}

};
