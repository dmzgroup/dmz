#include "dmzLuaExtObject.h"
#include "dmzLuaExtObjectObserver.h"
#include <dmzLuaKernel.h>
#include <dmzLuaKernelValidate.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesSphere.h>
#include <dmzTypesUUID.h>
#include <dmzTypesVector.h>

#include <luacpp.h>

/*!

\class dmz::LuaExtObject
\ingroup Lua
\brief Provides a Lua API for the Object Framework.
\details
\htmlonly See the <a href="dmzlua.html#dmz.object">Lua Object API</a>.
\endhtmlonly

*/

using namespace dmz;

namespace {

typedef LuaExtObject::ObjectStruct ostruct;

static const char ObjectKey = 'o';
static const char ObjectNamespace[] = "object";


static inline ObjectModule *
get_object_module (lua_State *L) {

  ObjectModule *result (0);
  lua_pushlightuserdata (L, (void *)&ObjectKey);
  lua_rawget (L, LUA_REGISTRYINDEX);
  ostruct **os = (ostruct **)lua_touserdata (L, -1);
  if (os && *os) { result = (*os)->objMod; }
  lua_pop (L, 1); // pop ostruct
  return result;
}


static inline ObjectModuleSelect *
get_object_module_select (lua_State *L) {

  ObjectModuleSelect *result (0);
  lua_pushlightuserdata (L, (void *)&ObjectKey);
  lua_rawget (L, LUA_REGISTRYINDEX);
  ostruct **os = (ostruct **)lua_touserdata (L, -1);
  if (os && *os) { result = (*os)->selectMod; }
  lua_pop (L, 1); // pop ostruct
  return result;
}


static inline ObjectModuleGrid *
get_object_module_grid (lua_State *L) {

  ObjectModuleGrid *result (0);
  lua_pushlightuserdata (L, (void *)&ObjectKey);
  lua_rawget (L, LUA_REGISTRYINDEX);
  ostruct **os = (ostruct **)lua_touserdata (L, -1);
  if (os && *os) { result = (*os)->gridMod; }
  lua_pop (L, 1); // pop ostruct
  return result;
}


static inline ostruct *
get_object_struct (lua_State *L) {

  ostruct *result (0);
  lua_pushlightuserdata (L, (void *)&ObjectKey);
  lua_rawget (L, LUA_REGISTRYINDEX);
  ostruct **os = (ostruct **)lua_touserdata (L, -1);
  if (os) { result = *os; }
  lua_pop (L, 1); // pop ostruct
  return result;
}


static inline Handle *
check_obj_handle (lua_State *L, int index, ObjectModule *objMod) {

   Handle *result (0);

   if (objMod) {

      if (index < 0) { index = lua_gettop (L) + index + 1; }

      result = lua_to_handle (L, index);

      if (!result && lua_isnumber (L, index)) {

         Handle value = (Handle)lua_tonumber (L, index);

         if (value) {

            result = lua_create_handle (L, value);
            lua_replace (L, index); // Replace number with handle
         }
      }

      if (!result) {

         UUID *uuid = lua_to_uuid (L, index);

         if (uuid) {

            Handle value = objMod->lookup_handle_from_uuid (*uuid);

            result = lua_create_handle (L, value);

            if (result) {

               lua_replace (L, index);
            }
         }
      }

      if (!result) {

         String error ("Paramter #");
         String typeName (luaL_typename (L, index));
         error << (Int32)index << " was: " << typeName << ". Expecting "
            << " dmz.handle or dmz.uuid type.";

         lua_pushstring (L, error.get_buffer ());
         lua_error (L);
      }
   }

   return result;
}


static inline Handle *
check_obj_handle (lua_State *L, int index, ostruct *os) {

   return check_obj_handle (L, index, os ? os->objMod : 0);
}


static int
object_create_lnv_name (lua_State *L) {

   const String Name (luaL_checkstring (L, 1));

   lua_pushstring (L, create_last_network_value_name (Name). get_buffer ());

   return 1;
}


static int
object_get_hil (lua_State *L) {

   int result (0);

   ostruct *os (get_object_struct (L));

   if (os && lua_create_handle (L, os->hilHandle)) { result = 1; }

   return result;
}


static int
object_is_object (lua_State *L) {

   int result (0);

   ObjectModule *objMod (get_object_module (L));
   Handle *ptr (check_obj_handle (L, 1, objMod));

   if (objMod && ptr) {

      lua_pushboolean (L, objMod->is_object (*ptr) ? 1 : 0);
      result = 1;
   }
   else {

      lua_pushboolean (L, 0);
      result = 1;
   }

   return result;
}


static int
object_is_link (lua_State *L) {

   int result (0);

   ObjectModule *objMod (get_object_module (L));
   Handle *ptr (check_obj_handle (L, 1, objMod));

   if (objMod && ptr) {

      lua_pushboolean (L, objMod->is_link (*ptr) ? 1 : 0);
      result = 1;
   }
   else {

      lua_pushboolean (L, 0);
      result = 1;
   }

   return result;
}


static int
object_get_all (lua_State *L) {

   int result (0);

   ObjectModule *objMod (get_object_module (L));

   if (objMod) {

      HandleContainer container;

      if (objMod->get_object_handles (container)) {

         lua_handle_container_to_table (L, container);
         result = 1;
      }
   }

   return result;
}


static int
object_create (lua_State *L) {

   int result (0);

   ostruct *os (get_object_struct (L));
   ObjectType *typePtr (lua_check_object_type (L, 1));

   if (os && os->objMod && typePtr) {

      ObjectLocalityEnum locality (ObjectLocalityUnknown);

      int localityLua (os->localityLocal);

      if (!lua_isnone (L, 2)) { localityLua = luaL_checkinteger (L, 2); }

      if (localityLua == os->localityLocal) { locality = ObjectLocal; }
      else if (localityLua == os->localityRemote) { locality = ObjectRemote; }
      else { lua_enum_error (L, 2, localityLua); }

      Handle value = os->objMod->create_object (*typePtr, locality);

      if (lua_create_handle (L, value)) { result = 1; }
   }

   return result;
}


static int
object_set_temporary (lua_State *L) {

   int result (0);

   ostruct *os (get_object_struct (L));
   Handle *ptr (check_obj_handle (L, 1, os ? os->objMod : 0));

   if (os && os->objMod && ptr) {

      if (os->objMod->is_object (*ptr)) {

         os->tempObjects.add_handle (*ptr);
         lua_pushboolean (L, 1);
      }
      else { lua_pushboolean (L, 0); }

      result = 1;
   }

   return result;
}


static int
object_unset_temporary (lua_State *L) {

   int result (0);

   ostruct *os (get_object_struct (L));
   Handle *ptr (check_obj_handle (L, 1, os ? os->objMod : 0));

   if (os && os->objMod && ptr) {

      if (os->objMod->is_object (*ptr)) {

         os->tempObjects.remove_handle (*ptr);
         lua_pushboolean (L, 1);
      }
      else { lua_pushboolean (L, 0); }

      result = 1;
   }

   return result;
}


static int
object_activate (lua_State *L) {

   int result (0);

   ObjectModule *objMod (get_object_module (L));
   Handle *ptr (check_obj_handle (L, 1, objMod));

   if (objMod && ptr) {

      lua_pushboolean (L, objMod->activate_object (*ptr) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
object_destroy (lua_State *L) {

   int result (0);

   ObjectModule *objMod (get_object_module (L));
   Handle *ptr (check_obj_handle (L, 1, objMod));

   if (objMod && ptr) {

      lua_pushboolean (L, objMod->destroy_object (*ptr) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
object_clone (lua_State *L) {

   int result (0);

   ostruct *os (get_object_struct (L));
   Handle *ptr (check_obj_handle (L, 1, os));

   if (os && os->objMod && ptr) {

      ObjectLinkRetentionEnum retain (ObjectRetainLinks);

      int retainLua (os->linksRetain);

      if (!lua_isnone (L, 2)) { retainLua = luaL_checkinteger (L, 2); }

      if (retainLua == os->linksRetain) { retain = ObjectRetainLinks; }
      else if (retainLua == os->linksIgnore) { retain = ObjectIgnoreLinks; }
      else { lua_enum_error (L, 2, retain); }

      if (lua_create_handle (L, os->objMod->clone_object (*ptr, retain))) { result = 1; }
   }

   return result;
}


static int
object_uuid_to_handle (lua_State *L) {

   int result (0);

   ObjectModule *objMod (get_object_module (L));
   UUID *uuid = lua_check_uuid (L, 1);

   if (objMod && uuid) {

      Handle value = objMod->lookup_handle_from_uuid (*uuid);

      if (lua_create_handle (L, value)) { lua_pushnil (L); }

      result = 1;
   }

   return result;
}


static int
object_uuid (lua_State *L) {

   int result (0);

   ObjectModule *objMod (get_object_module (L));
   Handle *handlePtr = check_obj_handle (L, 1, objMod);
   UUID *uuid = lua_to_uuid (L, 2);

   if (objMod && handlePtr) {

      if (uuid) {

         if (objMod->store_uuid (*handlePtr, *uuid)) { lua_pushvalue (L, 2); }
         else { lua_pushnil (L); }
      }
      else {

         UUID value;

         if (objMod->lookup_uuid (*handlePtr, value)) {

            if (!lua_create_uuid (L, value)) { lua_pushnil (L); }
         }
         else { lua_pushnil (L); }
      }

      result = 1;
   }

   return result;
}


static int
object_remove_attribute (lua_State *L) {

   int result (0);

   ObjectModule *objMod (get_object_module (L));
   Handle *handlePtr = check_obj_handle (L, 1, objMod);
   Handle *attrPtr = lua_check_handle (L, 2);
   Mask *maskPtr = lua_check_mask (L, 3);

   if (objMod && handlePtr && attrPtr && maskPtr) {

      lua_pushboolean (
         L,
         objMod->remove_attribute (*handlePtr, *attrPtr, *maskPtr) ? 1 : 0);

      result = 1;
   }

   return result;
}


static int
object_link (lua_State *L) {

   int result (0);

   ObjectModule *objMod (get_object_module (L));
   Handle *attrPtr = lua_check_handle (L, 1);
   Handle *superPtr = check_obj_handle (L, 2, objMod);
   Handle *subPtr = check_obj_handle (L, 3, objMod);

   if (objMod && attrPtr && superPtr && subPtr) {

      if (lua_create_handle (
            L,
            objMod->link_objects (*attrPtr, *superPtr, *subPtr))) { result = 1; }
   }

   return result;
}


static int
object_lookup_link_handle (lua_State *L) {

   int result (0);

   ObjectModule *objMod (get_object_module (L));
   Handle *attrPtr = lua_check_handle (L, 1);
   Handle *superPtr = check_obj_handle (L, 2, objMod);
   Handle *subPtr = check_obj_handle (L, 3, objMod);

   if (objMod && attrPtr && superPtr && subPtr) {

      if (lua_create_handle (
            L,
            objMod->lookup_link_handle (*attrPtr, *superPtr, *subPtr))) { result = 1; }
   }

   return result;
}


static int
object_lookup_linked_objects (lua_State *L) {

   int result (0);

   ObjectModule *objMod (get_object_module (L));
   Handle *ptr = lua_check_handle (L, 1);

   if (objMod && ptr) {

      Handle attr (0), super (0), sub (0);

      if (objMod->lookup_linked_objects (*ptr, attr, super, sub)) {

         lua_create_handle (L, attr);
         lua_create_handle (L, super);
         lua_create_handle (L, sub);

         result = 3;
      }
   }

   return result;
}


static int
object_unlink (lua_State *L) {

   int result (0);

   ObjectModule *objMod (get_object_module (L));
   Handle *ptr = lua_check_handle (L, 1);

   if (objMod && ptr) {

      lua_pushboolean (L, objMod->unlink_objects (*ptr) ? 1 : 0);

      result = 1;
   }

   return result;
}


static int
object_unlink_super_links (lua_State *L) {

   int result (0);

   ObjectModule *objMod (get_object_module (L));
   Handle *objPtr = lua_check_handle (L, 1);
   Handle *attrPtr = lua_check_handle (L, 2);

   if (objMod && attrPtr && objPtr) {

      lua_pushboolean (L, objMod->unlink_super_links (*objPtr, *attrPtr) ? 1 : 0);

      result = 1;
   }

   return result;
}


static int
object_unlink_sub_links (lua_State *L) {

   int result (0);

   ObjectModule *objMod (get_object_module (L));
   Handle *objPtr = lua_check_handle (L, 1);
   Handle *attrPtr = lua_check_handle (L, 2);

   if (objMod && attrPtr && objPtr) {

      lua_pushboolean (L, objMod->unlink_sub_links (*objPtr, *attrPtr) ? 1 : 0);

      result = 1;
   }

   return result;
}


static int
object_link_attribute_object (lua_State *L) {

   int result (0);

   ObjectModule *objMod (get_object_module (L));
   Handle *linkPtr = lua_check_handle (L, 1);
   Handle *objPtr = lua_to_handle (L, 2);

   if (objMod && linkPtr) {

      if (objPtr) {

         if (objMod->store_link_attribute_object (*linkPtr, *objPtr)) {

            lua_pushvalue (L, 2);
         }
         else { lua_pushnil (L); }

      }
      else {

         const Handle Value (
            objMod->lookup_link_attribute_object (*linkPtr));

         if (!lua_create_handle (L, Value)) { lua_pushnil (L); }
      }

      result = 1;
   }

   return result;
}


static int
object_attribute_object_links (lua_State *L) {

   int result (0);

   ObjectModule *objMod (get_object_module (L));
   Handle *objPtr = check_obj_handle (L, 1, objMod);

   if (objMod && objPtr) {

      HandleContainer container;

      if (objMod->lookup_attribute_object_links (*objPtr, container)) {

         lua_handle_container_to_table (L, container);
      }
      else { lua_pushnil (L); }

      result = 1;
   }

   return result;
}


static int
object_super_links (lua_State *L) {

   int result (0);

   ObjectModule *objMod (get_object_module (L));
   Handle *objPtr = check_obj_handle (L, 1, objMod);
   Handle *attrPtr = lua_check_handle (L, 2);

   if (objMod && attrPtr && objPtr) {

      HandleContainer container;

      if (objMod->lookup_super_links (*objPtr, *attrPtr, container)) {

         lua_handle_container_to_table (L, container);
      }
      else { lua_pushnil (L); }

      result = 1;
   }

   return result;
}


static int
object_sub_links (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   ObjectModule *objMod (get_object_module (L));
   Handle *objPtr = check_obj_handle (L, 1, objMod);
   Handle *attrPtr = lua_check_handle (L, 2);

   if (objMod && attrPtr && objPtr) {

      HandleContainer container;

      if (objMod->lookup_sub_links (*objPtr, *attrPtr, container)) {

         lua_handle_container_to_table (L, container);
      }
      else { lua_pushnil (L); }

      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
object_locality (lua_State *L) {

   int result (0);

   ostruct *os (get_object_struct (L));
   Handle *objPtr = check_obj_handle (L, 1, os);
   Boolean setLocality (False);
   int locality (0);

   if (!lua_isnone (L, 2)) {

      locality = luaL_checkinteger (L, 2);
      setLocality = True;
   }

   if (os && os->objMod && objPtr) {

      if (setLocality) {

         ObjectLocalityEnum value (ObjectLocalityUnknown);
         if (locality == os->localityLocal) { value = ObjectLocal; }
         else if (locality == os->localityRemote) { value = ObjectRemote; }
         else { lua_enum_error (L, 2, locality); }

         if (os->objMod->store_locality (*objPtr, value)) { lua_pushvalue (L, 2); }
         else { lua_pushnil (L); }
      }
      else {

         Boolean foundLocality (True);
         const ObjectLocalityEnum Value (os->objMod->lookup_locality (*objPtr));

         if (Value == ObjectLocal) { locality = os->localityLocal; }
         else if (Value == ObjectRemote) { locality = os->localityRemote; }
         else { foundLocality = False; }

         if (foundLocality) { lua_pushinteger (L, locality); }
         else { lua_pushnil (L); }
      }

      result = 1;
   }

   return result;
}


static int
object_type (lua_State *L) {

   int result (0);

   Handle objHandle (0);

   ostruct *os (get_object_struct (L));

   Handle *objPtr = check_obj_handle (L, 1, os);

   if (objPtr) { objHandle = *objPtr; }
 
   if (os && os->objMod && objHandle) {

      const ObjectType Type (os->objMod->lookup_object_type (objHandle));

      if (Type) {

         if (lua_create_object_type (L, &Type)) { result = 1; }
      }
   }

   return result;
}


ObjectModule *
get_obj_params (lua_State *L, Handle &objHandle, Handle &attrHandle) {

   ostruct *os (get_object_struct (L));

   Handle *ptr (check_obj_handle (L, 1, os));
   objHandle = (ptr ? *ptr : 0);

   if (os) {

      attrHandle = os->defaultHandle;

      if (!lua_isnoneornil (L, 2)) {

         ptr = lua_check_handle (L, 2);
         attrHandle = (ptr ? *ptr : 0);
      }
   }

   return os ? os->objMod : 0;
}


static int
object_counter (lua_State *L) {

   int result (0);

   Handle objHandle (0);
   Handle attrHandle (0);
   ObjectModule *objMod (get_obj_params (L, objHandle, attrHandle));

   if (objMod && objHandle && attrHandle) {

      if (!lua_isnone (L, 3)) {

         const Int64 Value = (Int64)luaL_checknumber (L, 3);

         if (objMod->store_counter (objHandle, attrHandle, Value)) {

            Int64 value (0);
            if (objMod->lookup_counter (objHandle, attrHandle, value)) {

               lua_pushnumber (L, (lua_Number (value)));
               result = 1;
            }
         }
      }
      else {

         Int64 value (0);
         if (objMod->lookup_counter (objHandle, attrHandle, value)) {

            lua_pushnumber (L, (lua_Number (value)));
            result = 1;
         }
      }
   }

   return result;
}


static int
object_counter_rollover (lua_State *L) {

   int result (0);

   Handle objHandle (0);
   Handle attrHandle (0);
   ObjectModule *objMod (get_obj_params (L, objHandle, attrHandle));

   if (objMod && objHandle && attrHandle) {

      if (!lua_isnone (L, 3)) {

         const Boolean Value = (lua_toboolean (L, 3) != 0);

         if (objMod->store_counter_rollover (objHandle, attrHandle, Value)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         Boolean value (False);
         if (objMod->lookup_counter_rollover (objHandle, attrHandle, value)) {

            lua_pushboolean (L, (value ? 1 : 0));
            result = 1;
         }
      }
   }

   return result;
}


static int
object_add_to_counter (lua_State *L) {

   int result (0);

   Handle objHandle (0);
   Handle attrHandle (0);
   ObjectModule *objMod (get_obj_params (L, objHandle, attrHandle));

   if (objMod && objHandle && attrHandle) {

      Int64 value (1);

      if (!lua_isnoneornil (L, 3)) { value = (Int64)luaL_checknumber (L, 3); }

      if (objMod->add_to_counter (objHandle, attrHandle, value)) {

         Int64 newValue (0);

         if (objMod->lookup_counter (objHandle, attrHandle, newValue)) {

            lua_pushnumber (L, lua_Number (newValue));
            result = 1;
         }
      }
   }

   return result;
}


static int
object_counter_minimum (lua_State *L) {

   int result (0);

   Handle objHandle (0);
   Handle attrHandle (0);
   ObjectModule *objMod (get_obj_params (L, objHandle, attrHandle));

   if (objMod && objHandle && attrHandle) {

      if (!lua_isnone (L, 3)) {

         const Int64 Value = (Int64)luaL_checknumber (L, 3);

         if (objMod->store_counter_minimum (objHandle, attrHandle, Value)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         Int64 value (0);
         if (objMod->lookup_counter_minimum (objHandle, attrHandle, value)) {

            lua_pushnumber (L, (lua_Number (value)));
            result = 1;
         }
      }
   }

   return result;
}


static int
object_counter_maximum (lua_State *L) {

   int result (0);

   Handle objHandle (0);
   Handle attrHandle (0);
   ObjectModule *objMod (get_obj_params (L, objHandle, attrHandle));

   if (objMod && objHandle && attrHandle) {

      if (!lua_isnone (L, 3)) {

         const Int64 Value = (Int64)luaL_checknumber (L, 3);

         if (objMod->store_counter_maximum (objHandle, attrHandle, Value)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         Int64 value (0);
         if (objMod->lookup_counter_maximum (objHandle, attrHandle, value)) {

            lua_pushnumber (L, (lua_Number (value)));
            result = 1;
         }
      }
   }

   return result;
}


static int
object_alternate_type (lua_State *L) {

   int result (0);

   Handle objHandle (0);
   Handle attrHandle (0);
   ObjectModule *objMod (get_obj_params (L, objHandle, attrHandle));

   if (objMod && objHandle && attrHandle) {

      ObjectType *typePtr (0);
      if (!lua_isnone (L, 3)) { typePtr = lua_check_object_type (L, 3); }

      if (typePtr) {

         if (objMod->store_alternate_object_type (objHandle, attrHandle, *typePtr)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         ObjectType type;

         if (objMod->lookup_alternate_object_type (objHandle, attrHandle, type)) {

            if (lua_create_object_type (L, &type)) { result = 1; }
         }
      }
   }

   return result;
}


static int
object_state (lua_State *L) {

   int result (0);

   Handle objHandle (0);
   Handle attrHandle (0);
   ObjectModule *objMod (get_obj_params (L, objHandle, attrHandle));

   if (objMod && objHandle && attrHandle) {

      Boolean storeMask = False;

      Mask value;

      if (!lua_isnone (L, 3)) {

         storeMask = True;

         if (lua_isstring (L, 3)) {

            Definitions defs (lua_get_runtime_context (L));

            const String Name (lua_tostring (L, 3));

            defs.lookup_state (Name, value);
         }
         else {

            Mask *ptr = lua_check_mask (L, 3);

            if (ptr) { value = *ptr; }
         }
      }

      if (storeMask) {

         if (objMod->store_state (objHandle, attrHandle, value)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         if (objMod->lookup_state (objHandle, attrHandle, value)) {

            if (lua_create_mask (L, &value)) { result = 1; }
         }
      }
   }

   return result;
}


static int
object_flag (lua_State *L) {

   int result (0);

   Handle objHandle (0);
   Handle attrHandle (0);
   ObjectModule *objMod (get_obj_params (L, objHandle, attrHandle));

   if (objMod && objHandle && attrHandle) {

      Boolean store (False);
      Boolean flag (False);
      if (!lua_isnone (L, 3)) { flag = (lua_toboolean (L, 3) == 1); store = True; }

      if (store) {

         if (objMod->store_flag (objHandle, attrHandle, flag)) {

            lua_pushboolean (L, flag ? 1 : 0);
            result = 1;
         }
      }
      else {

         lua_pushboolean (L, objMod->lookup_flag (objHandle, attrHandle) ? 1 : 0);
         result = 1;
      }
   }

   return result;
}


static int
object_time_stamp (lua_State *L) {

   int result (0);

   Handle objHandle (0);
   Handle attrHandle (0);
   ObjectModule *objMod (get_obj_params (L, objHandle, attrHandle));

   if (objMod && objHandle && attrHandle) {

      Boolean store (False);
      Float64 value (0.0);
      if (!lua_isnone (L, 3)) { value = luaL_checknumber (L, 3); store = True; }

      if (store) {

         if (objMod->store_time_stamp (objHandle, attrHandle, value)) {

            lua_pushnumber (L, value);
            result = 1;
         }
      }
      else {

         if (objMod->lookup_time_stamp (objHandle, attrHandle, value)) {

            lua_pushnumber (L, value);
            result = 1;
         }
      }
   }

   return result;
}


static int
object_position (lua_State *L) {

   int result (0);

   Handle objHandle (0);
   Handle attrHandle (0);
   ObjectModule *objMod (get_obj_params (L, objHandle, attrHandle));

   if (objMod && objHandle && attrHandle) {

      Vector *ptr (0);
      if (!lua_isnoneornil (L, 3)) { ptr = lua_check_vector (L, 3); }

      if (ptr) {

         if (objMod->store_position (objHandle, attrHandle, *ptr)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         Vector value;

         if (objMod->lookup_position (objHandle, attrHandle, value)) {

            if (lua_create_vector (L, &value)) { result = 1; }
         }
      }
   }

   return result;
}


static int
object_orientation (lua_State *L) {

   int result (0);

   Handle objHandle (0);
   Handle attrHandle (0);
   ObjectModule *objMod (get_obj_params (L, objHandle, attrHandle));

   if (objMod && objHandle && attrHandle) {

      Matrix *ptr (0);
      if (!lua_isnone (L, 3)) { ptr = lua_check_matrix (L, 3); }

      if (ptr) {

         if (objMod->store_orientation (objHandle, attrHandle, *ptr)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         Matrix value;

         if (objMod->lookup_orientation (objHandle, attrHandle, value)) {

            if (lua_create_matrix (L, &value)) { result = 1; }
         }
      }
   }

   return result;
}


static int
object_velocity (lua_State *L) {

   int result (0);

   Handle objHandle (0);
   Handle attrHandle (0);
   ObjectModule *objMod (get_obj_params (L, objHandle, attrHandle));

   if (objMod && objHandle && attrHandle) {

      Vector *ptr (0);
      if (!lua_isnone (L, 3)) { ptr = lua_check_vector (L, 3); }

      if (ptr) {

         if (objMod->store_velocity (objHandle, attrHandle, *ptr)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         Vector value;

         if (objMod->lookup_velocity (objHandle, attrHandle, value)) {

            if (lua_create_vector (L, &value)) { result = 1; }
         }
      }
   }

   return result;
}


static int
object_acceleration (lua_State *L) {

   int result (0);

   Handle objHandle (0);
   Handle attrHandle (0);
   ObjectModule *objMod (get_obj_params (L, objHandle, attrHandle));

   if (objMod && objHandle && attrHandle) {

      Vector *ptr (0);
      if (!lua_isnone (L, 3)) { ptr = lua_check_vector (L, 3); }

      if (ptr) {

         if (objMod->store_acceleration (objHandle, attrHandle, *ptr)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         Vector value;

         if (objMod->lookup_acceleration (objHandle, attrHandle, value)) {

            if (lua_create_vector (L, &value)) { result = 1; }
         }
      }
   }

   return result;
}


static int
object_scale (lua_State *L) {

   int result (0);

   Handle objHandle (0);
   Handle attrHandle (0);
   ObjectModule *objMod (get_obj_params (L, objHandle, attrHandle));

   if (objMod && objHandle && attrHandle) {

      Vector *ptr (0);
      if (!lua_isnone (L, 3)) { ptr = lua_check_vector (L, 3); }

      if (ptr) {

         if (objMod->store_scale (objHandle, attrHandle, *ptr)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         Vector value;

         if (objMod->lookup_scale (objHandle, attrHandle, value)) {

            if (lua_create_vector (L, &value)) { result = 1; }
         }
      }
   }

   return result;
}


static int
object_vector (lua_State *L) {

   int result (0);

   Handle objHandle (0);
   Handle attrHandle (0);
   ObjectModule *objMod (get_obj_params (L, objHandle, attrHandle));

   if (objMod && objHandle && attrHandle) {

      Vector *ptr (0);
      if (!lua_isnone (L, 3)) { ptr = lua_check_vector (L, 3); }

      if (ptr) {

         if (objMod->store_vector (objHandle, attrHandle, *ptr)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         Vector value;

         if (objMod->lookup_vector (objHandle, attrHandle, value)) {

            if (lua_create_vector (L, &value)) { result = 1; }
         }
      }
   }

   return result;
}


static int
object_scalar (lua_State *L) {

   int result (0);

   Handle objHandle (0);
   Handle attrHandle (0);
   ObjectModule *objMod (get_obj_params (L, objHandle, attrHandle));

   if (objMod && objHandle && attrHandle) {

      Boolean store (False);
      Float64 value (0.0);
      if (!lua_isnone (L, 3)) { value = luaL_checknumber (L, 3); store = True; }

      if (store) {

         if (objMod->store_scalar (objHandle, attrHandle, value)) {

            lua_pushnumber (L, value);
            result = 1;
         }
      }
      else {

         if (objMod->lookup_scalar (objHandle, attrHandle, value)) {

            lua_pushnumber (L, value);
            result = 1;
         }
      }
   }

   return result;
}


static int
object_text (lua_State *L) {

   int result (0);

   Handle objHandle (0);
   Handle attrHandle (0);
   ObjectModule *objMod (get_obj_params (L, objHandle, attrHandle));

   if (objMod && objHandle && attrHandle) {

      const char *ptr (0);
      if (!lua_isnone (L, 3)) { ptr = luaL_checkstring (L, 3); }

      if (ptr) {

         if (objMod->store_text (objHandle, attrHandle, ptr)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         String value;

         if (objMod->lookup_text (objHandle, attrHandle, value)) {

            lua_pushstring (L, value.get_buffer ());
            result = 1;
         }
      }
   }

   return result;
}


static int
object_data (lua_State *L) {

   int result (0);

   Handle objHandle (0);
   Handle attrHandle (0);
   ObjectModule *objMod (get_obj_params (L, objHandle, attrHandle));

   if (objMod && objHandle && attrHandle) {

      Data *ptr (0);
      if (!lua_isnone (L, 3)) { ptr = lua_check_data (L, 3); }

      if (ptr) {

         if (objMod->store_data (objHandle, attrHandle, *ptr)) {

            lua_pushvalue (L, 3);
            result = 1;
         }
      }
      else {

         Data value;

         if (objMod->lookup_data (objHandle, attrHandle, value)) {

            if (lua_create_data (L, &value)) { result = 1; }
         }
      }
   }

   return result;
}


static int
object_get_selected (lua_State *L) {

   int result (0);

   ObjectModuleSelect *mod (get_object_module_select (L));

   if (mod) {

      HandleContainer container;
      mod->get_selected_objects (container);
      lua_handle_container_to_table (L, container);
      result = 1;
   }

   return result;
}


static int
object_is_selected (lua_State *L) {

   int result (0);

   ObjectModuleSelect *mod (get_object_module_select (L));
   Handle *handle (lua_check_handle (L, 1));

   if (mod && handle) {

      lua_pushboolean (L, mod->is_selected (*handle) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
object_select (lua_State *L) {

   int result (0);

   ostruct *os (get_object_struct (L));
   Handle *handle (lua_check_handle (L, 1));
   int mode (os ? os->selectNew : 0);
   if (!lua_isnoneornil (L, 2)) { mode = luaL_checkinteger (L, 2); }

   if (os && os->selectMod && handle) {

      ObjectSelectModeEnum modeEnum (ObjectSelectNew);

      if (mode == os->selectNew) { modeEnum = ObjectSelectNew; }
      else if (mode == os->selectAdd) { modeEnum = ObjectSelectAdd; }
      else { lua_enum_error (L, 2, mode); }

      lua_pushboolean (L, os->selectMod->select_object (*handle, modeEnum) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
object_find (lua_State *L) {

   int result (0);

   ObjectModuleGrid *grid (get_object_module_grid (L));
   Sphere *sphere = lua_check_sphere (L, 1);

   if (grid && sphere) {

      HandleContainer list;

      grid->find_objects (*sphere, list, 0, 0);
      lua_handle_container_to_table (L, list);
      result = 1;
   }

   return result;
}


static int
object_unselect (lua_State *L) {

   int result (0);

   ObjectModuleSelect *mod (get_object_module_select (L));
   Handle *handle (lua_check_handle (L, 1));

   if (mod && handle) {

      lua_pushboolean (L, mod->unselect_object (*handle) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
object_unselect_all (lua_State *L) {

   ObjectModuleSelect *mod (get_object_module_select (L));

   if (mod) { mod->unselect_all_objects (); }

   return 0;
}


static const luaL_Reg arrayFunc[] = {
   {"create_last_nework_value_name", object_create_lnv_name},
   {"get_human_in_the_loop", object_get_hil},
   {"hil", object_get_hil}, // short cut!
   {"is_object", object_is_object},
   {"is_link", object_is_link},
   {"get_all", object_get_all},
   {"create", object_create},
   {"set_temporary", object_set_temporary},
   {"unset_temporary", object_unset_temporary},
   {"activate", object_activate},
   {"destroy", object_destroy},
   {"clone", object_clone},
   {"uuid_to_handle", object_uuid_to_handle},
   {"uuid", object_uuid},
   {"remove_attribute", object_remove_attribute},
   {"link", object_link},
   {"lookup_link_handle", object_lookup_link_handle},
   {"lookup_linked_objects", object_lookup_linked_objects},
   {"unlink", object_unlink},
   {"unlink_super_links", object_unlink_super_links},
   {"unlink_sub_links", object_unlink_sub_links},
   {"link_attribute_object", object_link_attribute_object},
   {"attribute_object_links", object_attribute_object_links},
   {"super_links", object_super_links},
   {"sub_links", object_sub_links},
   {"locality", object_locality},
   {"type", object_type},
   {"counter", object_counter},
   {"add_to_counter", object_add_to_counter},
   {"counter_rollover", object_counter_rollover},
   {"counter_minimum", object_counter_minimum},
   {"counter_maximum", object_counter_maximum},
   {"alternate_type", object_alternate_type},
   {"state", object_state},
   {"flag", object_flag},
   {"time_stamp", object_time_stamp},
   {"position", object_position},
   {"orientation", object_orientation},
   {"velocity", object_velocity},
   {"acceleration", object_acceleration},
   {"scale", object_scale},
   {"vector", object_vector},
   {"scalar", object_scalar},
   {"text", object_text},
   {"data", object_data},
   {"get_selected", object_get_selected},
   {"is_selected", object_is_selected},
   {"select", object_select},
   {"find", object_find},
   {"unselect", object_unselect},
   {"unselect_all", object_unselect_all},
   {NULL, NULL},
};

};


//! \cond
void
dmz::lua_register_object_observer (lua_State *L, ObjectObserver &obs) {

   ostruct *os (get_object_struct (L));

   if (os && os->ext) { os->ext->register_object_observer (obs); }
}


void
dmz::lua_release_object_observer (lua_State *L, ObjectObserver &obs) {

   ostruct *os = get_object_struct (L);

   if (os && os->ext) { os->ext->release_object_observer (obs); }
}


dmz::Handle
dmz::lua_get_default_handle (lua_State *L) {

   Handle result (0);

   ostruct *os = get_object_struct (L);

   if (os) { result = os->defaultHandle; }

   return result;
}


int
dmz::lua_locality_to_int (lua_State *L, const ObjectLocalityEnum Locality) {

   int result (0);

   ostruct *os = get_object_struct (L);

   if (Locality == ObjectLocal) { result = os->localityLocal; }
   else if (Locality == ObjectRemote) { result = os->localityRemote; }

   return result;
}


dmz::LuaExtObject::LuaExtObject (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      LuaExt (Info),
      ObjectObserverUtil (Info, local),
      L (0),
      _objPtr (0),
      _hilAttrHandle (0),
      _log (Info) {

   _obj.ext = this;
   _init (local);
}


dmz::LuaExtObject::~LuaExtObject () {

}


void
dmz::LuaExtObject::register_object_observer (ObjectObserver &obs) {

   if (_obsTable.store (obs.get_object_observer_handle (), &obs)) {

      if (_obj.objMod) { obs.store_object_module ("", *(_obj.objMod)); }
   }
}


void
dmz::LuaExtObject::release_object_observer (ObjectObserver &obs) {

   if (_obsTable.remove (obs.get_object_observer_handle ())) {

      if (_obj.objMod) { obs.remove_object_module ("", *(_obj.objMod)); }
   }
}


// Plugin Interface
void
dmz::LuaExtObject::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_obj.gridMod) { _obj.gridMod = ObjectModuleGrid::cast (PluginPtr); }

      if (!_obj.selectMod) { _obj.selectMod = ObjectModuleSelect::cast (PluginPtr); }

      ObjectModule *objMod (ObjectModule::cast (PluginPtr));

      if (PluginPtr && objMod) {

         store_object_module (PluginPtr->get_plugin_name (), *objMod);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_obj.selectMod && (_obj.selectMod == ObjectModuleSelect::cast (PluginPtr))) {

         _obj.selectMod = 0;
      }

      if (_obj.gridMod && (_obj.gridMod == ObjectModuleGrid::cast (PluginPtr))) {

         _obj.gridMod = 0;
      }

      ObjectModule *objMod (ObjectModule::cast (PluginPtr));

      if (PluginPtr && objMod) {

         remove_object_module (PluginPtr->get_plugin_name (), *objMod);
      }
   }
}


// LuaExt Interface
void
dmz::LuaExtObject::store_lua_module (LuaModule &module) {

}


void
dmz::LuaExtObject::open_lua_extension (lua_State *LuaState) {

   L = LuaState;

   LUA_START_VALIDATE (L);

   lua_init_object_observer (L);

   lua_pushlightuserdata (L, (void *)&ObjectKey);
   _objPtr = (ObjectStruct **)lua_newuserdata (L, sizeof (ObjectStruct *));

   if (_objPtr) {

      *_objPtr = &_obj;
      lua_rawset (L, LUA_REGISTRYINDEX);
   }
   else { lua_pop (L, 1); } // pop light data

   lua_create_dmz_namespace (L, ObjectNamespace);
   const int Table (lua_gettop (L));
   luaL_register (L, NULL, arrayFunc);

   _obj.localityLocal = lua_create_enum (L, "dmzObjectLocalityLocal");
   _obj.localityRemote = lua_create_enum (L, "dmzObjectLocalityRemote");
   _obj.linksRetain = lua_create_enum (L, "dmzObjectRetainLinks");
   _obj.linksIgnore = lua_create_enum (L, "dmzObjectIgnoreLinks");
   _obj.selectAdd = lua_create_enum (L, "dmzObjectSelectAdd");
   _obj.selectNew = lua_create_enum (L, "dmzObjectSelectNew");

   lua_pushinteger (L, _obj.localityLocal);
   lua_setfield (L, -2, "Local");

   lua_pushinteger (L, _obj.localityRemote);
   lua_setfield (L, -2, "Remote");

   lua_pushinteger (L, _obj.linksRetain);
   lua_setfield (L, -2, "RetainLinks");

   lua_pushinteger (L, _obj.linksIgnore);
   lua_setfield (L, -2, "IgnoreLinks");

   lua_pushinteger (L, _obj.selectAdd);
   lua_setfield (L, -2, "SelectAdd");

   lua_pushinteger (L, _obj.selectNew);
   lua_setfield (L, -2, "SelectNew");

   lua_create_mask (L, &ObjectCreateMask);
   lua_setfield (L, Table, "CreateMask");

   lua_create_mask (L, &ObjectDestroyMask);
   lua_setfield (L, Table, "DestroyMask");

   lua_create_mask (L, &ObjectLocalityMask);
   lua_setfield (L, Table, "LocalityMask");

   lua_create_mask (L, &ObjectUUIDMask);
   lua_setfield (L, Table, "UUIDMask");

   lua_create_mask (L, &ObjectRemoveAttributeMask);
   lua_setfield (L, Table, "RemoveAttributeMask");

   lua_create_mask (L, &ObjectLinkMask);
   lua_setfield (L, Table, "LinkMask");

   lua_create_mask (L, &ObjectUnlinkMask);
   lua_setfield (L, Table, "UnlinkMask");

   lua_create_mask (L, &ObjectLinkAttributeMask);
   lua_setfield (L, Table, "LinkAttributeObjectMask");

   lua_create_mask (L, &ObjectCounterMask);
   lua_setfield (L, Table, "CounterMask");

   lua_create_mask (L, &ObjectMinCounterMask);
   lua_setfield (L, Table, "MinCounterMask");

   lua_create_mask (L, &ObjectMaxCounterMask);
   lua_setfield (L, Table, "MaxCounterMask");

   lua_create_mask (L, &ObjectAltTypeMask);
   lua_setfield (L, Table, "AltTypeMask");

   lua_create_mask (L, &ObjectStateMask);
   lua_setfield (L, Table, "StateMask");

   lua_create_mask (L, &ObjectFlagMask);
   lua_setfield (L, Table, "FlagMask");

   lua_create_mask (L, &ObjectTimeStampMask);
   lua_setfield (L, Table, "TimeStampMask");

   lua_create_mask (L, &ObjectPositionMask);
   lua_setfield (L, Table, "PositionMask");

   lua_create_mask (L, &ObjectOrientationMask);
   lua_setfield (L, Table, "OrientationMask");

   lua_create_mask (L, &ObjectVelocityMask);
   lua_setfield (L, Table, "VelocityMask");

   lua_create_mask (L, &ObjectAccelerationMask);
   lua_setfield (L, Table, "AccelerationMask");

   lua_create_mask (L, &ObjectScaleMask);
   lua_setfield (L, Table, "ScaleMask");

   lua_create_mask (L, &ObjectVectorMask);
   lua_setfield (L, Table, "VectorMask");

   lua_create_mask (L, &ObjectScalarMask);
   lua_setfield (L, Table, "ScalarMask");

   lua_create_mask (L, &ObjectTextMask);
   lua_setfield (L, Table, "TextMask");

   lua_create_mask (L, &ObjectDataMask);
   lua_setfield (L, Table, "DataMask");

   lua_create_mask (L, &ObjectAllMask);
   lua_setfield (L, Table, "AllMask");

   lua_pushstring (L, ObjectAttributeDefaultName);
   lua_setfield (L, Table, "AttributeDefaultName");

   lua_pushstring (L, ObjectAttributeLastNetworkValueName);
   lua_setfield (L, Table, "AttributeLastNetworkValueName");

   lua_pushstring (L, ObjectAttributeHumanInTheLoopName);
   lua_setfield (L, Table, "AttributeHumanInTheLoopName");

   lua_pushstring (L, ObjectAttributeSelectName);
   lua_setfield (L, Table, "AttributeSelectName");

   const Message CreateType (ObjectCreateMessageName, lua_get_runtime_context (L));
   const Message DestroyType (ObjectDestroyMessageName, lua_get_runtime_context (L));

   if (lua_create_message (L, &CreateType)) {

      lua_setfield (L, Table, "CreateMessage");
   }

   if (lua_create_message (L, &DestroyType)) {

      lua_setfield (L, Table, "DestroyMessage");
   }

   lua_make_readonly (L, -1); // make object read only
   lua_pop (L, 1); // pop object table

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObject::close_lua_extension (lua_State *LuaState) {

   if (_obj.objMod) {

      Handle tmp (_obj.tempObjects.get_first ());

      while (tmp) {

         _obj.objMod->destroy_object (tmp);
         tmp = _obj.tempObjects.get_next ();
      }

      _obj.tempObjects.clear ();
   }

   HashTableHandleIterator it;
   ObjectObserver *obs (0);
   while (_obsTable.get_next (it, obs)) { release_object_observer (*obs); }

   _obsTable.clear ();

   L = 0;
}


void
dmz::LuaExtObject::remove_lua_module (LuaModule &module) {

}


// Object Observer Interface
void
dmz::LuaExtObject::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (AttributeHandle == _hilAttrHandle) {

      if (Value) { _obj.hilHandle = ObjectHandle; }
      else if (ObjectHandle == _obj.hilHandle) { _obj.hilHandle = 0; }
   }
}


void
dmz::LuaExtObject::_init (Config &local) {

   _hilAttrHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);

   Definitions defs (get_plugin_runtime_context ());

   _obj.defaultHandle = defs.create_named_handle (ObjectAttributeDefaultName);
}


void
dmz::LuaExtObject::_store_object_module (ObjectModule &objMod) {

   if (!_obj.objMod) {

      _obj.objMod = &objMod;

      HashTableHandleIterator it;

      ObjectObserver *obs (_obsTable.get_first (it));

      while (obs) {

         obs->store_object_module ("", objMod);

         obs = _obsTable.get_next (it);
      }
   }
}


void
dmz::LuaExtObject::_remove_object_module (ObjectModule &objMod) {

   if (_obj.objMod == &objMod) {

      _obj.objMod = 0;

      HashTableHandleIterator it;

      ObjectObserver *obs (_obsTable.get_first (it));

      while (obs) {

         obs->remove_object_module ("", objMod);

         obs = _obsTable.get_next (it);
      }
   }
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzLuaExtObject (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::LuaExtObject (Info, local);
}

};

