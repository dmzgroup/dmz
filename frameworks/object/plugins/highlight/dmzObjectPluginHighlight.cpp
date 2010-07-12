#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include "dmzObjectPluginHighlight.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::ObjectPluginHighlight
\ingroup Object
\brief Sets the highlight flag on objects and links.
\details
\code
<dmz>
<dmzObjectPluginHighlight>
   <data-convert-handle name="String"/>
   <mouse-move-message name="String/>
</dmzObjectPluginHighlight>
</dmz>
\endcode
- data-convert-handle.name Name of the Data object's attribute that contains the highlight Handle. Defaults to "handle".
- mouse-move-message.name Name of the message send when the mouse moves. Defaults to "Mouse_Move_Message".

This plugin only allows one object to be highlighted at a time.

*/

//! \cond
dmz::ObjectPluginHighlight::ObjectPluginHighlight (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      MessageObserver (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _convert (Info),
      _current (0),
      _highlightAttr (0) {

   _init (local);
}


dmz::ObjectPluginHighlight::~ObjectPluginHighlight () {

}


// Plugin Interface
void
dmz::ObjectPluginHighlight::update_plugin_state (
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
dmz::ObjectPluginHighlight::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Message Observer Interface
void
dmz::ObjectPluginHighlight::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   if (Type.is_of_type (_mouseMoveMsg)) {

      const Handle Object = _convert.to_handle (InData);

      if (Object != _current) {

         ObjectModule *module = get_object_module ();

         if (module) {

            module->store_flag (_current, _highlightAttr, False);

            if (Object) {

               if (module->is_object (Object)) {

                  _current = Object;

                  module->store_flag (_current, _highlightAttr, True);
               }
               else if (module->is_link (Object)) {

                  Handle attrObj = module->lookup_link_attribute_object (Object);

                  if (attrObj) {

                     _current = attrObj;
                     module->store_flag (_current, _highlightAttr, True);
                  }
               }
            }
            else { _current = 0; }
         }
      }
   }
   else if (Type.is_of_type (_deactivateMsg)) {

      if (_current) { 

         ObjectModule *module = get_object_module ();

         if (module) { module->store_flag (_current, _highlightAttr, False); }

         _current = 0;
      }
   }
}


// Object Observer Interface
void
dmz::ObjectPluginHighlight::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (Value) {

      if (ObjectHandle != _current) {

         ObjectModule *module (get_object_module ());

         if (module) { module->store_flag (_current, _highlightAttr, False); }

         _current = ObjectHandle;
      }
   }
   else if (ObjectHandle == _current) { _current = 0; } 
}


// ObjectPluginHighlight Interface
void
dmz::ObjectPluginHighlight::_init (Config &local) {

   _highlightAttr = activate_object_attribute (
      ObjectAttributeHighlightName,
      ObjectFlagMask);

   _convert.set_handle (
      config_to_string ("data-convert-handle.name", local, "handle"),
      get_plugin_runtime_context ());

   _mouseMoveMsg = config_create_message (
      "mouse-move-message.name",
      local,
      "Mouse_Move_Message",
      get_plugin_runtime_context ());

   _deactivateMsg = config_create_message (
      "deactivate-message.name",
      local,
      "Object_Highlight_Deactivate_Message",
      get_plugin_runtime_context ());

   subscribe_to_message (_mouseMoveMsg);
   subscribe_to_message (_deactivateMsg);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzObjectPluginHighlight (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ObjectPluginHighlight (Info, local);
}

};
