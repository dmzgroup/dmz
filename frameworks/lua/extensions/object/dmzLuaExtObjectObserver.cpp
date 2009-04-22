#include "dmzLuaExtObject.h"
#include "dmzLuaExtObjectObserver.h"
#include <dmzLuaKernel.h>
#include <dmzLuaKernelErrorHandler.h>
#include <dmzLuaKernelValidate.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzRuntimeConfig.h>
#include <dmzTypesMask.h>

#include <luacpp.h>

#ifdef _WIN32
// This disables the warning regarding using the 'this' pointer as an
// argument in the constructor. The pointer isn't actually used for anything
// other than its value.
#pragma warning (disable : 4355)
#endif // _Win32


using namespace dmz;

namespace {

typedef LuaExtObjectObserver * optr;

static const char ObjectObserverName[] = "dmz.object.observer";

static const char ObjectCreateFunc[]          = "create_object";
static const char ObjectDestroyFunc[]         = "destroy_object";
static const char ObjectUUIDFunc[]            = "update_object_uuid";
static const char ObjectRemoveAttributeFunc[] = "remove_object_attribute";
static const char ObjectLinkFunc[]            = "link_objects";
static const char ObjectUnlinkFunc[]          = "unlink_objects";
static const char ObjectLinkAttributeFunc[]   = "update_link_object";
static const char ObjectLocalityFunc[]        = "update_object_locality";
static const char ObjectCounterFunc[]         = "update_object_counter";
static const char ObjectCounterMinFunc[]      = "update_object_counter_minimum";
static const char ObjectCounterMaxFunc[]      = "update_object_counter_maximum";
static const char ObjectAltTypeFunc[]         = "update_object_alternate_type";
static const char ObjectStateFunc[]           = "update_object_state";
static const char ObjectFlagFunc[]            = "update_object_flag";
static const char ObjectTimeStampFunc[]       = "update_object_time_stamp";
static const char ObjectPositionFunc[]        = "update_object_position";
static const char ObjectOrientationFunc[]     = "update_object_orientation";
static const char ObjectVelocityFunc[]        = "update_object_velocity";
static const char ObjectAccelerationFunc[]    = "update_object_acceleration";
static const char ObjectScaleFunc[]           = "update_object_scale";
static const char ObjectVectorFunc[]          = "update_object_vector";
static const char ObjectScalarFunc[]          = "update_object_scalar";
static const char ObjectTextFunc[]            = "update_object_text";
static const char ObjectDataFunc[]            = "update_object_data";

static inline optr
obs_check (lua_State *L, int index) {

   optr *obs = (optr *)luaL_checkudata (L, index, ObjectObserverName);

   return obs ? *obs : 0;
}


static inline optr
obs_to (lua_State *L, int index) {

   optr *obs = (optr *)lua_is_object (L, index, ObjectObserverName);

   return obs ? *obs : 0;
}


static int
obs_delete (lua_State *L) {

   optr *obs = (optr *)luaL_checkudata (L, 1, ObjectObserverName);

   if (obs && *obs) {

      lua_release_object_observer (L, **obs);
      delete *obs; *obs = 0;
   }

   return 0;
}


static const int
obs_setup_cb (
      lua_State *L,
      LuaExtObjectObserver &obs,
      const Handle AttrHandle,
      const String &FuncName) {

   int result (0);

   Boolean error (False);

   lua_pushcfunction (L, lua_error_handler);
   result = lua_gettop (L);

   lua_rawgeti (L, LUA_REGISTRYINDEX, obs.get_index ());
   const int IndexTable = lua_gettop (L);

   if (lua_istable (L, IndexTable)) {

      lua_create_handle (L, AttrHandle);
      lua_rawget (L, IndexTable);
      const int AttrTable = lua_gettop (L);

      if (lua_istable (L, AttrTable)) {

         lua_rawgeti (L, AttrTable, 1);

         if (lua_istable (L, -1)) {

            lua_getfield (L, -1, FuncName.get_buffer ());

            if (lua_isfunction (L, -1)) { lua_rawgeti (L, AttrTable, 2); }
         }
         else { error = True; }
      }
      else { error = True; }
   }
   else { error = True; }

   if (error) {

      lua_pop (L, lua_gettop (L) - (result - 1));
      result = 0;
   }

   return result;
}


static void
obs_do_cb (
      lua_State *L,
      LuaExtObjectObserver &obs,
      const int ArgCount,
      const int Handler,
      const Handle AttrHandle,
      const Mask &AttrMask) {

   if (Handler) {

      if (lua_pcall (L, ArgCount + 1, 0, Handler)) {

         lua_pop (L, 1); // pop error message

         obs.deactivate_object_attribute (AttrHandle, AttrMask);
      }

      lua_pop (L, lua_gettop (L) - (Handler - 1));
   }
   else {

      lua_pop (L, ArgCount);
      obs.deactivate_object_attribute (AttrHandle, AttrMask);
   }
}


static const Mask &
obs_has_func (lua_State *L, int index, const String &Name, const Mask &FunctionMask) {

   static const Mask EmptyMask;

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   lua_getfield (L, index, Name.get_buffer ());

   const Boolean Result = (lua_isfunction (L, -1) == 1);

   lua_pop (L, 1);

   return Result ? FunctionMask : EmptyMask;
}


int
obs_register (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   optr obs (obs_check (L, 1));
   Handle *handle (0);
   if (!lua_isnil (L, 2)) { handle = lua_check_handle (L, 2); }

   if (!handle) {

      handle = lua_create_handle (L, lua_get_default_handle (L));
      lua_replace (L, 2);
   }

   luaL_checktype (L, 3, LUA_TTABLE);
   int data = (lua_isnoneornil (L, 4) ? 3 : 4);

   if (obs && handle) {

      Mask cb;

      cb |= obs_has_func (L, 3, ObjectCreateFunc, ObjectCreateMask);
      cb |= obs_has_func (L, 3, ObjectDestroyFunc, ObjectDestroyMask);
      cb |= obs_has_func (L, 3, ObjectUUIDFunc, ObjectUUIDMask);
      cb |= obs_has_func (L, 3, ObjectRemoveAttributeFunc, ObjectRemoveAttributeMask);
      cb |= obs_has_func (L, 3, ObjectLinkFunc, ObjectLinkMask);
      cb |= obs_has_func (L, 3, ObjectUnlinkFunc, ObjectUnlinkMask);
      cb |= obs_has_func (L, 3, ObjectLinkAttributeFunc, ObjectLinkAttributeMask);
      cb |= obs_has_func (L, 3, ObjectLocalityFunc, ObjectLocalityMask);
      cb |= obs_has_func (L, 3, ObjectCounterFunc, ObjectCounterMask);
      cb |= obs_has_func (L, 3, ObjectCounterMinFunc, ObjectMinCounterMask);
      cb |= obs_has_func (L, 3, ObjectCounterMaxFunc, ObjectMaxCounterMask);
      cb |= obs_has_func (L, 3, ObjectAltTypeFunc, ObjectAltTypeMask);
      cb |= obs_has_func (L, 3, ObjectStateFunc, ObjectStateMask);
      cb |= obs_has_func (L, 3, ObjectFlagFunc, ObjectFlagMask);
      cb |= obs_has_func (L, 3, ObjectTimeStampFunc, ObjectTimeStampMask);
      cb |= obs_has_func (L, 3, ObjectPositionFunc, ObjectPositionMask);
      cb |= obs_has_func (L, 3, ObjectOrientationFunc, ObjectOrientationMask);
      cb |= obs_has_func (L, 3, ObjectVelocityFunc, ObjectVelocityMask);
      cb |= obs_has_func (L, 3, ObjectAccelerationFunc, ObjectAccelerationMask);
      cb |= obs_has_func (L, 3, ObjectScaleFunc, ObjectScaleMask);
      cb |= obs_has_func (L, 3, ObjectVectorFunc, ObjectVectorMask);
      cb |= obs_has_func (L, 3, ObjectScalarFunc, ObjectScalarMask);
      cb |= obs_has_func (L, 3, ObjectTextFunc, ObjectTextMask);
      cb |= obs_has_func (L, 3, ObjectDataFunc, ObjectDataMask);

      Mask unreg (ObjectAllMask);

      unreg ^= cb;
      obs->deactivate_object_attribute (*handle, unreg);

      lua_rawgeti (L, LUA_REGISTRYINDEX, obs->get_index ()); // get observer index table
      const int CBTable (lua_gettop (L));
      lua_pushvalue (L, 2); // Push attribute handle
      lua_createtable (L, 2, 0); // create table for callback and data tables
      const int Table (lua_gettop (L));
      lua_pushvalue (L, 3); // push callback table
      lua_rawseti (L, Table, 1); // store callback table
      lua_pushvalue (L, data); // push either callback or data table if it exists
      lua_rawseti (L, Table, 2); // store data table
      lua_rawset (L, CBTable); // store table of callback and data tables
      lua_pop (L, 1); // pop observer index table.
      lua_pushboolean (L, obs->activate_object_attribute (*handle, cb) != 0 ? 1 : 0);
      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


int
obs_release (lua_State *L) {

   int result (0);

   optr obs (obs_check (L, 1));
   Handle *handle (lua_check_handle (L, 2));
   Mask *mask (lua_check_mask (L, 3));

   if (obs && handle && mask) {

      obs->deactivate_object_attribute (*handle, *mask);
      lua_pushboolean (L, 1);
      result = 1;
   }

   return result;
}


static const luaL_Reg arrayMembers [] = {
   {"register", obs_register},
   {"release", obs_release},
   {"__gc", obs_delete},
   {NULL, NULL},
};


static int
obs_new (lua_State *L) { return lua_create_object_observer (L) ? 1 : 0; }


static const luaL_Reg arrayFunc [] = {
   {"new", obs_new},
   {NULL, NULL},
};

};


//! \cond
void
dmz::lua_init_object_observer (lua_State *L) {

   luaL_newmetatable (L, ObjectObserverName);
   luaL_register (L, NULL, arrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");

   lua_create_dmz_namespace (L, "object_observer");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1);
   lua_pop (L, 2);
}


dmz::LuaExtObjectObserver *
dmz::lua_create_object_observer (lua_State *L) {

   LuaExtObjectObserver *result (0);

   LuaExtObjectObserver **ptr =
      (LuaExtObjectObserver **)lua_newuserdata (L, sizeof (LuaExtObjectObserver));

   if (ptr) {

      Config empty;

      *ptr = new LuaExtObjectObserver (L, empty);

      if (*ptr) {

         lua_register_object_observer (L, **ptr);
         luaL_getmetatable (L, ObjectObserverName);
         lua_setmetatable (L, -2);
      }

      result = *ptr;
   }

   return result;
}


dmz::LuaExtObjectObserver *
dmz::lua_check_object_observer (lua_State *L, int index) {

    optr *obs = (optr *)luaL_checkudata (L, index, ObjectObserverName);

    return obs ? *obs : 0;
}


dmz::LuaExtObjectObserver::LuaExtObjectObserver (lua_State *TheState, Config &empty) :
      PluginInfo (
         "",
         PluginDeleteModeDoNotDelete,
         lua_get_runtime_context (TheState),
         0),
      ObjectObserverUtil (*((PluginInfo *)this), empty),
      _Handle ("LuaExtObjectObserver", lua_get_runtime_context (TheState)),
      L (TheState),
      _index (0) {

   lua_newtable (L);
   _index = luaL_ref (L, LUA_REGISTRYINDEX);
}


dmz::LuaExtObjectObserver::~LuaExtObjectObserver () {;}


void
dmz::LuaExtObjectObserver::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   LUA_START_VALIDATE (L);

   const Handle DefaultHandle (lua_get_default_handle (L));

   const int Handler (obs_setup_cb (L, *this, DefaultHandle, ObjectCreateFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_create_object_type (L, &Type);
      lua_pushinteger (L, lua_locality_to_int (L, Locality));

      obs_do_cb (L, *this, 3, Handler, DefaultHandle, ObjectCreateMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   LUA_START_VALIDATE (L);

   const Handle DefaultHandle (lua_get_default_handle (L));

   const int Handler (obs_setup_cb (L, *this,  DefaultHandle, ObjectDestroyFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);

      obs_do_cb (L, *this, 1, Handler, DefaultHandle, ObjectDestroyMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::update_object_uuid (
      const Handle ObjectHandle,
      const UUID &Identity,
      const UUID &PrevIdentity) {

   LUA_START_VALIDATE (L);

   const Handle DefaultHandle (lua_get_default_handle (L));

   const int Handler (obs_setup_cb (L, *this, DefaultHandle, ObjectUUIDFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_create_uuid (L, Identity);
      lua_create_uuid (L, PrevIdentity);

      obs_do_cb (L, *this, 3, Handler, DefaultHandle, ObjectUUIDMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::remove_object_attribute (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &AttrMask) {

   LUA_START_VALIDATE (L);

   const int Handler (
      obs_setup_cb (L, *this, AttributeHandle, ObjectRemoveAttributeFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_create_handle (L, AttributeHandle);
      lua_create_mask (L, &AttrMask);

      obs_do_cb (L, *this, 3, Handler, AttributeHandle, ObjectRemoveAttributeMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::update_object_locality (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectLocalityEnum Locality,
      const ObjectLocalityEnum PrevLocality) {

   LUA_START_VALIDATE (L);

   const Handle DefaultHandle (lua_get_default_handle (L));

   const int Handler (obs_setup_cb (L, *this, DefaultHandle, ObjectLocalityFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_pushinteger (L, lua_locality_to_int (L, Locality));
      lua_pushinteger (L, lua_locality_to_int (L, PrevLocality));

      obs_do_cb (L, *this, 3, Handler, DefaultHandle, ObjectLocalityMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, AttributeHandle, ObjectLinkFunc));

   if (Handler) {

      lua_create_handle (L, LinkHandle);
      lua_create_handle (L, AttributeHandle);
      lua_create_handle (L, SuperHandle);
      lua_create_handle (L, SubHandle);

      obs_do_cb (L, *this, 4, Handler, AttributeHandle, ObjectLinkMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, AttributeHandle, ObjectUnlinkFunc));

   if (Handler) {

      lua_create_handle (L, LinkHandle);
      lua_create_handle (L, AttributeHandle);
      lua_create_handle (L, SuperHandle);
      lua_create_handle (L, SubHandle);

      obs_do_cb (L, *this, 4, Handler, AttributeHandle, ObjectUnlinkMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::update_link_attribute_object (
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

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, AttributeHandle, ObjectLinkAttributeFunc));

   if (Handler) {

      lua_create_handle (L, LinkHandle);
      lua_create_handle (L, AttributeHandle);
      lua_create_handle (L, SuperHandle);
      lua_create_handle (L, SubHandle);
      lua_create_handle (L, AttributeObjectHandle);
      lua_create_handle (L, PrevAttributeObjectHandle);

      obs_do_cb (L, *this, 6, Handler, AttributeHandle, ObjectLinkAttributeMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::update_object_counter (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, AttributeHandle, ObjectCounterFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_create_handle (L, AttributeHandle);
      lua_pushnumber (L, (lua_Number)Value);
      if (PreviousValue) { lua_pushnumber (L, (lua_Number)(*PreviousValue)); }
      else { lua_pushnil (L); }

      obs_do_cb (L, *this, 4, Handler, AttributeHandle, ObjectCounterMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::update_object_counter_minimum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, AttributeHandle, ObjectCounterMinFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_create_handle (L, AttributeHandle);
      lua_pushnumber (L, (lua_Number)Value);
      if (PreviousValue) { lua_pushnumber (L, (lua_Number)(*PreviousValue)); }
      else { lua_pushnil (L); }

      obs_do_cb (L, *this, 4, Handler, AttributeHandle, ObjectMinCounterMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::update_object_counter_maximum (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Int64 Value,
      const Int64 *PreviousValue) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, AttributeHandle, ObjectCounterMaxFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_create_handle (L, AttributeHandle);
      lua_pushnumber (L, (lua_Number)Value);
      if (PreviousValue) { lua_pushnumber (L, (lua_Number)(*PreviousValue)); }
      else { lua_pushnil (L); }

      obs_do_cb (L, *this, 4, Handler, AttributeHandle, ObjectMaxCounterMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::update_object_alternate_type (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const ObjectType &Value,
      const ObjectType *PreviousValue) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, AttributeHandle, ObjectAltTypeFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_create_handle (L, AttributeHandle);
      lua_create_object_type (L, &Value);
      if (PreviousValue) { lua_create_object_type (L, PreviousValue); }
      else { lua_pushnil (L); }

      obs_do_cb (L, *this, 4, Handler, AttributeHandle, ObjectAltTypeMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, AttributeHandle, ObjectStateFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_create_handle (L, AttributeHandle);
      lua_create_mask (L, &Value);
      if (PreviousValue) { lua_create_mask (L, PreviousValue); }
      else { lua_pushnil (L); }

      obs_do_cb (L, *this, 4, Handler, AttributeHandle, ObjectStateMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, AttributeHandle, ObjectFlagFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_create_handle (L, AttributeHandle);
      lua_pushboolean (L, Value);
      if (PreviousValue) { lua_pushboolean (L, *PreviousValue); }
      else { lua_pushnil (L); }

      obs_do_cb (L, *this, 4, Handler, AttributeHandle, ObjectFlagMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::update_object_time_stamp (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, AttributeHandle, ObjectTimeStampFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_create_handle (L, AttributeHandle);
      lua_pushnumber (L, Value);
      if (PreviousValue) { lua_pushnumber (L, *PreviousValue); }
      else { lua_pushnil (L); }

      obs_do_cb (L, *this, 4, Handler, AttributeHandle, ObjectTimeStampMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, AttributeHandle, ObjectPositionFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_create_handle (L, AttributeHandle);
      lua_create_vector (L, &Value);
      if (PreviousValue) { lua_create_vector (L, PreviousValue); }
      else { lua_pushnil (L); }

      obs_do_cb (L, *this, 4, Handler, AttributeHandle, ObjectPositionMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, AttributeHandle, ObjectOrientationFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_create_handle (L, AttributeHandle);
      lua_create_matrix (L, &Value);
      if (PreviousValue) { lua_create_matrix (L, PreviousValue); }
      else { lua_pushnil (L); }

      obs_do_cb (L, *this, 4, Handler, AttributeHandle, ObjectOrientationMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::update_object_velocity (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, AttributeHandle, ObjectVelocityFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_create_handle (L, AttributeHandle);
      lua_create_vector (L, &Value);
      if (PreviousValue) { lua_create_vector (L, PreviousValue); }
      else { lua_pushnil (L); }

      obs_do_cb (L, *this, 4, Handler, AttributeHandle, ObjectVelocityMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::update_object_acceleration (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, AttributeHandle, ObjectAccelerationFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_create_handle (L, AttributeHandle);
      lua_create_vector (L, &Value);
      if (PreviousValue) { lua_create_vector (L, PreviousValue); }
      else { lua_pushnil (L); }

      obs_do_cb (L, *this, 4, Handler, AttributeHandle, ObjectAccelerationMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::update_object_scale (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, AttributeHandle, ObjectScaleFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_create_handle (L, AttributeHandle);
      lua_create_vector (L, &Value);
      if (PreviousValue) { lua_create_vector (L, PreviousValue); }
      else { lua_pushnil (L); }

      obs_do_cb (L, *this, 4, Handler, AttributeHandle, ObjectScaleMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::update_object_vector (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, AttributeHandle, ObjectVectorFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_create_handle (L, AttributeHandle);
      lua_create_vector (L, &Value);
      if (PreviousValue) { lua_create_vector (L, PreviousValue); }
      else { lua_pushnil (L); }

      obs_do_cb (L, *this, 4, Handler, AttributeHandle, ObjectVectorMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, AttributeHandle, ObjectScalarFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_create_handle (L, AttributeHandle);
      lua_pushnumber (L, Value);
      if (PreviousValue) { lua_pushnumber (L, *PreviousValue); }
      else { lua_pushnil (L); }

      obs_do_cb (L, *this, 4, Handler, AttributeHandle, ObjectScalarMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::update_object_text (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const String &Value,
      const String *PreviousValue) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, AttributeHandle, ObjectTextFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_create_handle (L, AttributeHandle);
      lua_pushstring (L, Value.get_buffer ());
      if (PreviousValue) { lua_pushstring (L, PreviousValue->get_buffer ()); }
      else { lua_pushnil (L); }

      obs_do_cb (L, *this, 4, Handler, AttributeHandle, ObjectTextMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtObjectObserver::update_object_data (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Data &Value,
      const Data *PreviousValue) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, AttributeHandle, ObjectDataFunc));

   if (Handler) {

      lua_create_handle (L, ObjectHandle);
      lua_create_handle (L, AttributeHandle);
      lua_create_data (L, &Value);
      if (PreviousValue) { lua_create_data (L, PreviousValue); }
      else { lua_pushnil (L); }

      obs_do_cb (L, *this, 4, Handler, AttributeHandle, ObjectDataMask);
   }

   LUA_END_VALIDATE (L, 0);
}
//! \endcond

