#include <dmzEventConsts.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzWeaponAmmoUtil.h>
#include <dmzWeaponConsts.h>

struct dmz::WeaponAmmoUtil::State {

   Int32 id;
   Handle attrHandle;
   Handle sourceEventHandle;
   ObjectType defaultType;
   Log *log;

   State (RuntimeContext *context, Config &local, Log *theLog);
};

dmz::WeaponAmmoUtil::State::State (RuntimeContext *context, Config &local, Log *theLog) :
      id (1),
      attrHandle (0),
      sourceEventHandle (0),
      log (theLog) {

   id = config_to_int32 ("munitions.id", local, id);

   defaultType = config_to_object_type ("munitions.object-type.name", local, context);

   if (!defaultType && log) {

      log->warn << "No default munitions type defined for munitions id: " << id << endl;
   }

   attrHandle = config_to_named_handle (
      "munitions.attribute",
      local,
      create_weapon_attribute_name (id),
      context);

   sourceEventHandle = config_to_named_handle (
      "munitions.event.attribute",
      local,
      EventAttributeSourceName,
      context);
}


dmz::WeaponAmmoUtil::WeaponAmmoUtil (const PluginInfo &Info, Config &local, Log *log) :
      _state (*(new State (Info.get_context (), local, log))) {;}


dmz::WeaponAmmoUtil::WeaponAmmoUtil (const PluginInfo &Info, Config &local) :
      _state (*(new State (Info.get_context (), local, 0))) {;}


dmz::WeaponAmmoUtil::WeaponAmmoUtil (RuntimeContext *context, Config &local, Log *log) :
      _state (*(new State (context, local, log))) {;}


dmz::WeaponAmmoUtil::WeaponAmmoUtil (RuntimeContext *context, Config &local) :
      _state (*(new State (context, local, 0))) {;}


dmz::WeaponAmmoUtil::~WeaponAmmoUtil () { delete &_state; }


dmz::Handle
dmz::WeaponAmmoUtil::create_munition (const Handle SourceHandle, ObjectModule &objMod) {

   Handle result (0);

   if (SourceHandle) {

      Boolean createAmmo (True);

      ObjectType ammoType (_state.defaultType);

      objMod.lookup_alternate_object_type (SourceHandle, _state.attrHandle, ammoType);

      Int64 ammoCount (0);

      if (objMod.lookup_counter (SourceHandle, _state.attrHandle, ammoCount)) {

         if (ammoCount <= 0) { createAmmo = False; }
         else {

            const UInt64 Value =
               objMod.add_to_counter (SourceHandle, _state.attrHandle, -1);

            if (Value == ammoCount) { createAmmo = False; }
         }
      }

      if (ammoType && createAmmo) {

         result = objMod.create_object (ammoType, ObjectLocal);

         if (result && _state.sourceEventHandle) {

            objMod.link_objects (_state.sourceEventHandle, SourceHandle, result);
         }
      }
   }

   return result;
}
