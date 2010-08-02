#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include "dmzObjectModuleSelectBasic.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
/*!

\class dmz::ObjectModuleSelectBasic
\ingroup Object
\brief Basic ObjectModuleSelect implementation.
\details This provides a basic implementation of the ObjectModuleSelect. At this time the
ObjectModuleSelectBasic has no configuration parameters.
\sa ObjectModuleSelect

*/

//! \cond
dmz::ObjectModuleSelectBasic::ObjectModuleSelectBasic (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ObjectModuleSelect (Info),
      ObjectObserverUtil (Info, local),
      _selectHandle (0),
      _log (Info) {

   _init (local);
}


dmz::ObjectModuleSelectBasic::~ObjectModuleSelectBasic () {

   _selectTable.clear ();
}


// ObjectModuleSelect Interface
void
dmz::ObjectModuleSelectBasic::get_selected_objects (HandleContainer &container) {

   HashTableHandleIterator it;

   while (_selectTable.get_next (it)) { container.add (it.get_hash_key ()); }
}


dmz::Boolean
dmz::ObjectModuleSelectBasic::is_selected (const Handle ObjectHandle) {

   return _selectTable.lookup (ObjectHandle) != 0;
}


dmz::Boolean
dmz::ObjectModuleSelectBasic::select_object (
      const Handle ObjectHandle,
      const ObjectSelectModeEnum Mode) {

   Boolean result (False);

   if (Mode != ObjectSelectAdd) { unselect_all_objects (); }

   if (_selectTable.store (ObjectHandle, this)) {

      ObjectModule *objMod (get_object_module ());

      if (objMod && _selectHandle) {

         objMod->store_flag (ObjectHandle, _selectHandle, True);
      }

      result = True;
   }

   return result;
}


dmz::Boolean
dmz::ObjectModuleSelectBasic::unselect_object (const Handle ObjectHandle) {

   Boolean result (False);

   if (_selectTable.remove (ObjectHandle)) {

      ObjectModule *objMod (get_object_module ());

      if (objMod && _selectHandle) {

         result = objMod->store_flag (ObjectHandle, _selectHandle, False);
      }
   }

   return result;
}


void
dmz::ObjectModuleSelectBasic::unselect_all_objects () {

   ObjectModule *objMod (get_object_module ());

   if (_selectHandle && objMod) {

      HashTableHandleIterator it;

      while (_selectTable.get_next (it)) {

         objMod->store_flag (it.get_hash_key (), _selectHandle, False);
      }
   }

   _selectTable.clear ();
}


// Object Observer Interface
void
dmz::ObjectModuleSelectBasic::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) { unselect_object (ObjectHandle); }


void
dmz::ObjectModuleSelectBasic::remove_object_attribute (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &AttrMask) {

   if ((AttributeHandle == _selectHandle) && (AttrMask & ObjectFlagMask)) {

      unselect_object (ObjectHandle);
   }
}


void
dmz::ObjectModuleSelectBasic::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (AttributeHandle == _selectHandle) {

      if (Value) { _selectTable.store (ObjectHandle, this); }
      else { _selectTable.remove (ObjectHandle); }
   }
}


void
dmz::ObjectModuleSelectBasic::_init (Config &local) {

   activate_default_object_attribute (ObjectDestroyMask);

   _selectHandle = activate_object_attribute (
      config_to_string ("select.name", local, ObjectAttributeSelectName),
      ObjectRemoveAttributeMask | ObjectFlagMask);
}


void
dmz::ObjectModuleSelectBasic::_store_object_module (ObjectModule &objMod) {

   if (_selectHandle) {

      HashTableHandleIterator it;

      while (_selectTable.get_next (it)) {

         if (!objMod.store_flag (it.get_hash_key (), _selectHandle, True)) {

            _selectTable.remove (it.get_hash_key ());
         }
      }
   }
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzObjectModuleSelectBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ObjectModuleSelectBasic (Info, local);
}

};
