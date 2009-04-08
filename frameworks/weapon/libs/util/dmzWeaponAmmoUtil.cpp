#include <dmzObjectModule.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzWeaponAmmoUtil.h>

struct dmz::WeaponAmmoUtil::State {

   State (RuntimeContext *context, Config &local) {;}
};

dmz::WeaponAmmoUtil::WeaponAmmoUtil (const PluginInfo &Info, Config &local) :
      _state (*(new State (Info.get_context (), local))) {;}


dmz::WeaponAmmoUtil::WeaponAmmoUtil (RuntimeContext *context, Config &local) :
      _state (*(new State (context, local))) {;}


dmz::WeaponAmmoUtil::~WeaponAmmoUtil () { delete &_state; }


dmz::Handle
dmz::WeaponAmmoUtil::create_munition (const Handle SourceHandle, ObjectModule &objMod) {

   Handle result (0);

   return result;
}
