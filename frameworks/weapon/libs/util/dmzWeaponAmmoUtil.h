#ifndef DMZ_WEAPON_AMMO_UTIL_DOT_H
#define DMZ_WEAPON_AMMO_UTIL_DOT_H

#include <dmzTypesBase.h>
#include <dmzWeaponUtilExport.h>

namespace dmz {

class Config;
class Log;
class ObjectModule;
class ObjectType;
class PluginInfo;
class RuntimeContext;

class DMZ_WEAPON_UTIL_LINK_SYMBOL WeaponAmmoUtil {

   public:
      WeaponAmmoUtil (const PluginInfo &Info, Config &local, Log *log);
      WeaponAmmoUtil (const PluginInfo &Info, Config &local);
      WeaponAmmoUtil (RuntimeContext *context, Config &local, Log *log);
      WeaponAmmoUtil (RuntimeContext *context, Config &local);

      ~WeaponAmmoUtil ();

      Handle create_munition (const Handle SourceHandle, ObjectModule &objMod);

   protected:
      struct State;
      State &_state;
};

};

#endif // DMZ_WEAPON_AMMO_UTIL_DOT_H
