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

/*!

\class dmz::WeaponAmmoUtil
\ingroup Weapon
\brief Creates a new munitions object and links it to the source object.
\details
\code
<dmz>
<init-scope>
   <munitions id="Weapon Attribute id" attribute="[Optional]Weapon Attribute Name">
      <object-type name="Munition Object Type"/>
      <event attribute="[Optional]Link name"/>
   </munitions>
</init-scope>
</dmz>
\endcode

*/

//! \cond
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
//! \endcond


/*!

\brief Constructor
\param[in] Info PluginInfo object.
\param[in] local Config object used to initialize the class.
\param[in] log Log pointer used for error reporting.

*/
dmz::WeaponAmmoUtil::WeaponAmmoUtil (const PluginInfo &Info, Config &local, Log *log) :
      _state (*(new State (Info.get_context (), local, log))) {;}


/*!

\brief Constructor
\param[in] Info PluginInfo object.
\param[in] local Config object used to initialize the class.

*/
dmz::WeaponAmmoUtil::WeaponAmmoUtil (const PluginInfo &Info, Config &local) :
      _state (*(new State (Info.get_context (), local, 0))) {;}


/*!

\brief Constructor
\param[in] context RuntimeContext pointer.
\param[in] local Config object used to initialize the class.
\param[in] log Log pointer used for error reporting.

*/
dmz::WeaponAmmoUtil::WeaponAmmoUtil (RuntimeContext *context, Config &local, Log *log) :
      _state (*(new State (context, local, log))) {;}


/*!

\brief Constructor
\param[in] context RuntimeContext pointer.
\param[in] local Config object used to initialize the class.

*/
dmz::WeaponAmmoUtil::WeaponAmmoUtil (RuntimeContext *context, Config &local) :
      _state (*(new State (context, local, 0))) {;}


//! Destructor
dmz::WeaponAmmoUtil::~WeaponAmmoUtil () { delete &_state; }


/*!

\brief Creates a munition.
\details Creates a munition object if the source object's weapon counter is greater than
zero. If the source object does not have a weapon counter, the munition is create.
The source object is also checked for for a munition type from the alternate object
type attribute. If an alternate object type is not defined, the default object type
is used as the munition type. The create munition object is created but not activate
in the ObjectModule.
\param[in] SourceHandle Handle of the source of the munition.
\param[in] objMod Reference to the ObjectModule used to create the munition object.
\return Returns the Handle of the create munition object.
\sa
dmz::ObjectModule::create_object()
dmz::ObjectModule::activate_object()

*/
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
