#include <dmzRuntimeConfig.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToMatrix.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigToPathContainer.h>
#include <dmzRuntimeConfigToState.h>
#include <dmzRuntimeConfigToStringContainer.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimeConfigWrite.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeObjectType.h>
#include <dmzTypesBase.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesString.h>
#include <dmzTypesVector.h>

/*!

\file dmzRuntimeConfigToTypesBase.h
\ingroup Runtime
\brief Contains functions to convert config contexts to various base types.

\file dmzRuntimeConfigToNamedHandle.h
\ingroup Runtime
\brief Contains functions to convert config contexts to named handles.

\file dmzRuntimeConfigToMatrix.h
\ingroup Runtime
\brief Contains functions to convert config contexts to Matrices.

\file dmzRuntimeConfigToVector.h
\ingroup Runtime
\brief Contains functions to convert config contexts to Vectors.

\file dmzRuntimeConfigWrite.h
\ingroup Runtime
\brief Contains functions to convert various object types into config contexts.

*/

namespace {

static dmz::Boolean
local_config_to_string (
      const dmz::String &Name,
      const dmz::Config &Source,
      dmz::String &result) {

   dmz::String realName (Name ? Name : "value");
   return Source.lookup_attribute (realName, result);
}

}

//! \addtogroup Runtime
//! @{

/*!

\fn dmz::EventType  dmz::config_to_event_type (const String &Name, const Config &Source, RuntimeContext *context)
\brief Converts Config to EventType.
\details Defined in dmzRuntimeEventType.h.
\code
RuntimeContext *context (get_plugin_runtime_context ());
dmz::EventType value = dmz::config_to_event_type ("dmz.event-type.name", local, context);
\endcode
\param[in] Name String containing name of config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] context Pointer to the runtime context.
\return Returns dmz::EventType containing value.

*/

/*!

\brief Converts Config to EventType.
\details Defined in dmzRuntimeEventType.h.
\code
RuntimeContext *context (get_plugin_runtime_context ());
const dmz::String DefaultEventTypeName ("Some_Event");
dmz::EventType value = dmz::config_to_event_type ("dmz.event-type.name", local, DefaultEventTypeName, context);
\endcode
\param[in] Name String containing name of config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] DefaultEventTypeName String containing default EventType name to use
if the config context is not found.
\param[in] context Pointer to the runtime context.
\return Returns dmz::EventType containing value.

*/
dmz::EventType
dmz::config_to_event_type (
      const String &Name,
      const Config &Source,
      const String &DefaultEventTypeName,
      RuntimeContext *context) {

   EventType defaultEventType;

   Definitions defs (context);

   defs.lookup_event_type (DefaultEventTypeName, defaultEventType);

   return config_to_event_type (Name, Source, defaultEventType, context);
}


/*!

\brief Converts Config to EventType.
\details Defined in dmzRuntimeEventType.h.
\code
RuntimeContext *context (get_plugin_runtime_context ());
const dmz::EventType DefaultEventType ("Some_Event", context);
dmz::EventType value = dmz::config_to_event_type ("dmz.event.type", local, DefaultEventType, context);
\endcode
\param[in] Name String containing name of config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] DefaultEventType EventType containing default EventType to use
if the config context is not found.
\param[in] context Pointer to the runtime context.
\return Returns dmz::EventType containing value.

*/
dmz::EventType
dmz::config_to_event_type (
      const String &Name,
      const Config &Source,
      const EventType &DefaultEventType,
      RuntimeContext *context) {

   EventType result;

   String eventTypeName;

   if (local_config_to_string (Name, Source, eventTypeName)) {

      Definitions defs (context);

      defs.lookup_event_type (eventTypeName, result);
   }
   else { result = DefaultEventType; }

   return result;
}


/*!

\brief Converts Config to EventTypeSet.
\details Defined in dmzRuntimeEventType.h.
\code
RuntimeContext *context (get_plugin_runtime_context ());
dmz::EventTypeSet value = dmz::config_to_event_type_set ("dmz.set", global, context);
\endcode
The Config for the above example would be formated as follows:
\code
<dmz>
<set>
   <event-type name="Some Event"/>
   <event-type name="Another Event"/>
   <event-type name="Yet Another Event"/>
</set>
</dmz>
\endcode
\param[in] Name String containing name of config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] context Pointer to the runtime context.
\return Returns dmz::EventTypeSet containing value.

*/
dmz::EventTypeSet
dmz::config_to_event_type_set (
      const String &Name,
      const Config &Source,
      RuntimeContext *context) {

   EventTypeSet result;

   Config list;

   String realName (Name);
   if (realName) { realName << "."; }

   if (Source.lookup_all_config (realName + "event-type", list)) {

      ConfigIterator it;
      Config type;

      while (list.get_next_config (it, type)) {

         result.add_event_type (config_to_event_type ("name", type, context));
      }
   }

   return result;
}


/*!

\fn dmz::ObjectType dmz::config_to_object_type (
const String &Name,
const Config &Source,
RuntimeContext *context)
\brief Converts Config to ObjectType.
\details Defined in dmzRuntimeObjectType.h.
\code
RuntimeContext *context (get_plugin_runtime_context ());
dmz::ObjectType value = dmz::config_to_object_type ("dmz.object.type", local, context);
\endcode
\param[in] Name String containing name of config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] context Pointer to the runtime context.
\return Returns dmz::ObjectType containing value.

*/

/*!

\brief Converts Config to ObjectType.
\details Defined in dmzRuntimeObjectType.h.
\code
RuntimeContext *context (get_plugin_runtime_context ());
const dmz::String DefaultObjectTypeName ("Some_Object");
dmz::ObjectType value = dmz::config_to_object_type ("dmz.object.type", local, DefaultObjectTypeName, context);
\endcode
\param[in] Name String containing name of config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] DefaultObjectTypeName String containing default ObjectType name to use
if the config context is not found.
\param[in] context Pointer to the runtime context.
\return Returns dmz::ObjectType containing value.

*/
dmz::ObjectType
dmz::config_to_object_type (
      const String &Name,
      const Config &Source,
      const String &DefaultObjectTypeName,
      RuntimeContext *context) {

   ObjectType defaultObjectType;

   Definitions defs (context);

   defs.lookup_object_type (DefaultObjectTypeName, defaultObjectType);

   return config_to_object_type (Name, Source, defaultObjectType, context);
}


/*!

\brief Converts Config to ObjectType.
\details Defined in dmzRuntimeObjectType.h.
\code
RuntimeContext *context (get_plugin_runtime_context ());
const dmz::ObjectType DefaultObjectType ("Some_Object", context);
dmz::ObjectType value = dmz::config_to_object_type ("dmz.object-type.name", local, DefaultObjectType, context);
\endcode
\param[in] Name String containing name of config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] DefaultObjectType ObjectType containing default ObjectType to use
if the config context is not found.
\param[in] context Pointer to the runtime context.
\return Returns dmz::ObjectType containing value.

*/

dmz::ObjectType
dmz::config_to_object_type (
      const String &Name,
      const Config &Source,
      const ObjectType &DefaultObjectType,
      RuntimeContext *context) {

   ObjectType result;

   String objectTypeName;

   if (local_config_to_string (Name, Source, objectTypeName)) {

      Definitions defs (context);

      defs.lookup_object_type (objectTypeName, result);
   }
   else { result = DefaultObjectType; }

   return result;
}


/*!

\brief Converts Config to ObjectTypeSet.
\details Defined in dmzRuntimeObjectType.h.
\code
RuntimeContext *context (get_plugin_runtime_context ());
dmz::ObjectTypeSet value = dmz::config_to_object_type_set ("dmz.set", global, context);
\endcode
The Config for the above example would be formated as follows:
\code
<dmz>
<set>
   <object-type name="Some Object"/>
   <object-type name="Another Object"/>
   <object-type name="Yet Another Object"/>
</set>
</dmz>
\endcode
\param[in] Name String containing name of config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] context Pointer to the runtime context.
\return Returns dmz::ObjectTypeSet containing value.

*/
dmz::ObjectTypeSet
dmz::config_to_object_type_set (
      const String &Name,
      const Config &Source,
      RuntimeContext *context) {

   ObjectTypeSet result;

   Config list;

   String realName (Name);
   if (realName) { realName << "."; }

   if (Source.lookup_all_config (realName + "object-type", list)) {

      ConfigIterator it;
      Config type;

      while (list.get_next_config (it, type)) {

         result.add_object_type (config_to_object_type ("name", type, context));
      }
   }

   return result;
}


/*!

\fn dmz::Mask dmz::config_to_state (
const String &Name,
const Config &Source,
RuntimeContext *context)
\brief Converts Config to a Mask containing state values.
\details Defined in dmzRuntimeConfigToState.h.
\code
RuntimeContext *context (get_plugin_runtime_context ());
dmz::Mask value = dmz::config_to_state ("dmz.state.value", local, context);
\endcode
\param[in] Name String containing name of config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] context Pointer to the runtime context.
\return Returns dmz::Mask containing the state values.

*/

/*!

\brief Converts Config to a Mask containing state values.
\details Defined in dmzRuntimeConfigToState.h.
\code
RuntimeContext *context (get_plugin_runtime_context ());
const dmz::String DefaultStateName ("Some_State|Another_State");
dmz::Mask value = dmz::config_to_state ("dmz.state.value", local, DefaultStateName, context);
\endcode
\param[in] Name String containing name of config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] DefaultStateName String containing default state name to use
if the config context is not found.
\param[in] context Pointer to the runtime context.
\return Returns dmz::Mask containing the state values.

*/
dmz::Mask
dmz::config_to_state (
      const String &Name,
      const Config &Source,
      const String &DefaultStateName,
      RuntimeContext *context) {

   Mask defaultMask;

   Definitions defs (context);

   defs.lookup_state (DefaultStateName, defaultMask);

   return config_to_state (Name, Source, defaultMask, context);
}


/*!

\brief Converts Config to a Mask containing state values.
\details Defined in dmzRuntimeConfigToState.h.
\code
RuntimeContext *context (get_plugin_runtime_context ());
const dmz::Mask DefaultState;
dmz::Mask value = dmz::config_to_state ("dmz.state.value", local, DefaultState, context);
\endcode
\param[in] Name String containing name of config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] DefaultState Mask containing default state to use
if the config context is not found.
\param[in] context Pointer to the runtime context.
\return Returns dmz::Mask containing the state values.

*/
dmz::Mask
dmz::config_to_state (
      const String &Name,
      const Config &Source,
      const Mask &DefaultState,
      RuntimeContext *context) {

   Mask result;

   String stateName;

   if (local_config_to_string (Name, Source, stateName)) {

      Definitions defs (context);

      defs.lookup_state (stateName, result);
   }
   else { result = DefaultState; }

   return result;
}


/*!

\fn dmz::PathContainer dmz::config_to_path_container (const dmz::Config &Source)
\brief Converts Config to a PathContainer.
\details Defined in dmzRuntimeConfigToPathContainer.h.
\code
dmz::PathContainer value = dmz::config_to_path_container (global);
\endcode
The Config for the above example would be formatted as follows:
\code
<path value="/a/path/of/some/sort"/>
<path value="/a/path/of/another/sort"/>
\endcode
\param[in] Source Config containing config context to convert.
\return Returns dmz::PathContainer of the path values.

*/

/*!

\brief Converts Config to a PathContainer.
\details Defined in dmzRuntimeConfigToPathContainer.h.
\code
dmz::PathContainer value = dmz::config_to_path_container ("dmz.path", global);
\endcode
The Config for the above example would be formatted as follows:
\code
<dmz>
   <path value="/a/path/of/some/sort"/>
   <path value="/a/path/of/another/sort"/>
</dmz>
\endcode
\param[in] Name String containing name of config context to convert.
\param[in] Source Config containing config context to convert.
\return Returns dmz::PathContainer of the path values.

*/
dmz::PathContainer
dmz::config_to_path_container (const String &Name, const Config &Source) {

   PathContainer result;

   Config pathList;

   if (Name) { Source.lookup_all_config (Name, pathList); }
   else { Source.lookup_all_config ("path", pathList); }

   ConfigIterator it;
   Config path;

   while (pathList.get_next_config (it, path)) {

      result.add_path (config_to_string ("value", path));
   }

   return result;
}


/*!

\fn dmz::StringContainer dmz::config_to_string_container (const dmz::Config &Source)
\brief Converts Config to a StringContainer.
\details Defined in dmzRuntimeConfigToStringContainer.h.
\code
dmz::StringContainer value = dmz::config_to_string_container (global);
\endcode
The Config for the above example would be formatted as follows:
\code
<string value="The string"/>
<string value="Another string"/>
\endcode
\param[in] Source Config containing config context to convert.
\return Returns dmz::StringContainer of the String values.

*/

/*!

\brief Converts Config to a StringContainer.
\details Defined in dmzRuntimeConfigToStringContainer.h.
\code
dmz::StringContainer value = dmz::config_to_string_container ("dmz.string", global);
\endcode
The Config for the above example would be formatted as follows:
\code
<dmz>
   <string value="The string"/>
   <string value="Another string"/>
</dmz>
\endcode
\param[in] Name String containing name of config context to convert.
\param[in] Source Config containing config context to convert.
\return Returns dmz::StringContainer of the String values.

*/
dmz::StringContainer
dmz::config_to_string_container (const String &Name, const Config &Source) {

   StringContainer result;

   Config stringList;

   if (Name) { Source.lookup_all_config (Name, stringList); }
   else { Source.lookup_all_config ("string", stringList); }

   ConfigIterator it;
   Config string;

   while (stringList.get_next_config (it, string)) {

      result.add_string (config_to_string ("value", string));
   }

   return result;
}


/*!

\brief Converts Config to Vector.
\details Defined in dmzRuntimeConfigToVector.h.
This function assumes that the Vector will be defined by three attributes stored
in the config context. The three attributes should be named "x", "y", and "z".
This function \b is case sensitive. The name value may be scoped. For example:
\code
const dmz::Vector DefaultValue;
dmz::Vector value = dmz::config_to_vector ("dmz.values.vector", global, DefaultValue);
\endcode
\param[in] Name String containing name of config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] DefaultValue Vector containing default value to use if config context is
not found.
\return Returns dmz::Vector containing value.

*/
dmz::Vector
dmz::config_to_vector (
      const String &Name,
      const Config &Source,
      const Vector &DefaultValue) {

   Config cd;

   if (Name) { Source.lookup_config (Name, cd); }
   else { cd = Source; }

   const Vector Result (
      config_to_float64 ("x", cd, DefaultValue.get_x ()),
      config_to_float64 ("y", cd, DefaultValue.get_y ()),
      config_to_float64 ("z", cd, DefaultValue.get_z ()));

   return Result;
}


dmz::VectorComponentEnum
dmz::config_to_vector_component (
   const String &Name,
   const Config &Source,
   const VectorComponentEnum DefaultValue) {

   VectorComponentEnum result (DefaultValue);

   String str;

   if (local_config_to_string (Name, Source, str)) {

      const String Value = str.get_lower ();

      if ((Value == "x") || (Value == "0") || (Value == "vectorcomponentx")) {

         result = VectorComponentX;
      }
      else if ((Value == "y") || (Value == "1") || (Value == "vectorcomponenty")) {

         result = VectorComponentY;
      }
      else if ((Value == "z") || (Value == "2") || (Value == "vectorcomponentz")) {

         result = VectorComponentZ;
      }
   }

   return result;
}


/*!

\brief Converts Config to Matrix.
\details Defined in dmzRuntimeConfigToMatrix.h.
This function assumes that the Matrix will be defined by nine attributes stored
in the config context. The nine attributes should be named "v0" to "v8".
This function \b is case sensitive. The \a Name value may be scoped. For example:
\code
const dmz::Matrix DefaultValue; // Identity matrix.
dmz::Matrix value = dmz::config_to_matrix ("dmz.values.rotation", global, DefaultValue);
\endcode
\param[in] Name String containing name of config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] DefaultValue Matrix containing default value to use if config context is
not found.
\return Returns dmz::Matrix containing value.

*/
dmz::Matrix
dmz::config_to_matrix (
      const String &Name,
      const Config &Source,
      const Matrix &DefaultValue) {

   Config cd;

   if (Name) { Source.lookup_config (Name, cd); }
   else { cd = Source; }

   Float64 inPack[9] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
   DefaultValue.to_array (inPack);

   Float64 outPack[9] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};

   outPack[0] = config_to_float64 ("v0", cd, inPack[0]),
   outPack[1] = config_to_float64 ("v1", cd, inPack[1]);
   outPack[2] = config_to_float64 ("v2", cd, inPack[2]);
   outPack[3] = config_to_float64 ("v3", cd, inPack[3]);
   outPack[4] = config_to_float64 ("v4", cd, inPack[4]);
   outPack[5] = config_to_float64 ("v5", cd, inPack[5]);
   outPack[6] = config_to_float64 ("v6", cd, inPack[6]);
   outPack[7] = config_to_float64 ("v7", cd, inPack[7]);
   outPack[8] = config_to_float64 ("v8", cd, inPack[8]);

   const Matrix Result (outPack);
   return Result;
}


/*!

\brief Converts Config to dmz::BaseTypeEnum.
\details Defined in dmzRuntimeConfigToTypesBase.h.
This function uses dmz::config_to_sring to retrieve the value and then uses
dmz::string_to_base_type_enum to convert the returned String to a dmz::BaseTypeEnum.
\param[in] Name String containing name of the attribute in the config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] DefaultValue dmz::BaseTypeEnum containing default value that is returned if
the config context is not found.
\return Returns dmz::BaseTypeEnum containing value.
\sa dmz::config_to_string(const String &Name, const Config &Source, const String &DefaultValue)

*/
dmz::BaseTypeEnum
dmz::config_to_base_type_enum (
      const String &Name,
      const Config &Source,
      const BaseTypeEnum DefaultValue) {

   BaseTypeEnum result (BaseTypeUnknown);

   String str;

   if (local_config_to_string (Name, Source, str)) {

      result = string_to_base_type_enum (str);
   }

   return result;
}


/*!

\brief Converts Config to dmz::Boolean.
\details Defined in dmzRuntimeConfigToTypesBase.h.
This function uses dmz::config_to_sring to retrieve the value and then uses
dmz::string_to_boolean to convert the returned String to a dmz::Boolean.
\param[in] Name String containing name of the attribute in the config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] DefaultValue dmz::Boolean containing default value that is returned if
the config context is not found.
\return Returns dmz::Boolean containing value.
\sa dmz::config_to_string(const String &Name, const Config &Source, const String &DefaultValue)

*/
dmz::Boolean
dmz::config_to_boolean (
      const String &Name,
      const Config &Source,
      const Boolean DefaultValue) {

   Boolean result (DefaultValue);
   String str;

   if (local_config_to_string (Name, Source, str)) {

      result = string_to_boolean (str);
   }

   return result;
}


/*!

\brief Converts Config to dmz::Int32.
\details Defined in dmzRuntimeConfigToTypesBase.h.
This function uses dmz::config_to_sring to retrieve the value and then uses
dmz::string_to_int32 to convert the returned String to a dmz::Int32.
\param[in] Name String containing name of the attribute in the config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] DefaultValue dmz::Int32 containing default value that is returned if
the config context is not found.
\return Returns dmz::Int32 containing value.
\sa dmz::config_to_string(const String &Name, const Config &Source, const String &DefaultValue)

*/
dmz::Int32
dmz::config_to_int32 (
      const String &Name,
      const Config &Source,
      const Int32 DefaultValue) {

   Int32 result (DefaultValue);
   String str;

   if (local_config_to_string (Name, Source, str)) { result = string_to_int32 (str); }

   return result;
}


/*!

\brief Converts Config to dmz::UInt32.
\details Defined in dmzRuntimeConfigToTypesBase.h.
This function uses dmz::config_to_sring to retrieve the value and then uses
dmz::string_to_uint32 to convert the returned String to a dmz::UInt32.
\param[in] Name String containing name of the attribute in the config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] DefaultValue dmz::UInt32 containing default value that is returned if
the config context is not found.
\return Returns dmz::UInt32 containing value.
\sa dmz::config_to_string(const String &Name, const Config &Source, const String &DefaultValue)

*/
dmz::UInt32
dmz::config_to_uint32 (
      const String &Name,
      const Config &Source,
      const UInt32 DefaultValue) {

   UInt32 result (DefaultValue);
   String str;

   if (local_config_to_string (Name, Source, str)) { result = string_to_uint32 (str); }

   return result;
}


/*!

\brief Converts Config to dmz::Int64.
\details Defined in dmzRuntimeConfigToTypesBase.h.
This function uses dmz::config_to_sring to retrieve the value and then uses
dmz::string_to_int64 to convert the returned String to a dmz::Int64.
\param[in] Name String containing name of the attribute in the config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] DefaultValue dmz::Int64 containing default value that is returned if
the config context is not found.
\return Returns dmz::Int64 containing value.
\sa dmz::config_to_string(const String &Name, const Config &Source, const String &DefaultValue)

*/
dmz::Int64
dmz::config_to_int64 (
      const String &Name,
      const Config &Source,
      const Int64 DefaultValue) {

   Int64 result (DefaultValue);
   String str;

   if (local_config_to_string (Name, Source, str)) { result = string_to_int64 (str); }

   return result;
}


/*!

\brief Converts Config to dmz::UInt64.
\details Defined in dmzRuntimeConfigToTypesBase.h.
This function uses dmz::config_to_sring to retrieve the value and then uses
dmz::string_to_uint64 to convert the returned String to a dmz::UInt64.
\param[in] Name String containing name of the attribute in the config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] DefaultValue dmz::UInt64 containing default value that is returned if
the config context is not found.
\return Returns dmz::UInt64 containing value.
\sa dmz::config_to_string(const String &Name, const Config &Source, const String &DefaultValue)

*/
dmz::UInt64
dmz::config_to_uint64 (
      const String &Name,
      const Config &Source,
      const UInt64 DefaultValue) {

   UInt64 result (DefaultValue);
   String str;

   if (local_config_to_string (Name, Source, str)) { result = string_to_uint64 (str); }

   return result;
}


/*!

\brief Converts Config to dmz::Float32.
\details Defined in dmzRuntimeConfigToTypesBase.h.
This function uses dmz::config_to_sring to retrieve the value and then uses
dmz::string_to_float32 to convert the returned String to a dmz::Float32.
\param[in] Name String containing name of the attribute in the config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] DefaultValue dmz::Float32 containing default value that is returned if
the config context is not found.
\return Returns dmz::Float32 containing value.
\sa dmz::config_to_string(const String &Name, const Config &Source, const String &DefaultValue)

*/
dmz::Float32
dmz::config_to_float32 (
      const String &Name,
      const Config &Source,
      const Float32 DefaultValue) {

   Float32 result (DefaultValue);
   String str;

   if (local_config_to_string (Name, Source, str)) {

      result = string_to_float32 (str);
   }

   return result;
}


/*!

\brief Converts Config to dmz::Float64.
\details Defined in dmzRuntimeConfigToTypesBase.h.
This function uses dmz::config_to_sring to retrieve the value and then uses
dmz::string_to_float64 to convert the returned String to a dmz::Float64.
\param[in] Name String containing name of the attribute in the config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] DefaultValue dmz::Float64 containing default value that is returned if
the config context is not found.
\return Returns dmz::Float64 containing value.
\sa dmz::config_to_string(const String &Name, const Config &Source, const String &DefaultValue)

*/
dmz::Float64
dmz::config_to_float64 (
      const String &Name,
      const Config &Source,
      const Float64 DefaultValue) {

   Float64 result (DefaultValue);
   String str;

   if (local_config_to_string (Name, Source, str)) {

      result = string_to_float64 (str);
   }

   return result;
}


/*!

\brief Converts Config to dmz::String.
\details Defined in dmzRuntimeConfigToTypesBase.h.
This function converts the named attribute to a dmz::String. It is assumed that the
last part of the \a Name variable specifies the attribute name. If the String
"foo.value" is passed in as \a Name, it will try to find a  config context called
"foo" and the attribute "value" stored in the "foo" config context.
\code
dmz::String value = dmz::config_to_string ("dmz.flag.value", global, "default name");
\endcode
\param[in] Name String containing name of the attribute in the config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] DefaultValue dmz::String containing default value that is returned if
the config context is not found.
\return Returns dmz::String containing value.

*/
dmz::String
dmz::config_to_string (
      const String &Name,
      const Config &Source,
      const String &DefaultValue) {

   String result (DefaultValue);
   String realName (Name ? Name : "value");
   Source.lookup_attribute (realName, result);
   return result;
}


/*!

\brief Converts Config to Message.
\details Defined in dmzRuntimeMessaging.h.
This function converts the named attribute to a dmz::Message. It is assumed that the
last part of the \a Name variable specifies the attribute name. If the String
"type.name" is passed in as \a Name, it will try to find a  config context called
"type" and the attribute "name" stored in the "type" config context.
\code
dmz::Message type = dmz::config_to_message_type ("dmz.type.name", global, get_plugin_runtime_context (), &_log);
\endcode
\param[in] Name String containing name of the attribute in the config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] context Pointer to the runtime context.
\param[in] log Pointer to the dmz::Log to use for log reporting.
\return Returns dmz::Message containing the message type. Returns an empty
dmz::Message if the message type is not found.

*/
dmz::Message
dmz::config_to_message_type (
      const String &Name,
      const Config &Source,
      RuntimeContext *context,
      Log *log) {

   Message result;

   String messageTypeName;

   if (Source.lookup_attribute (Name, messageTypeName)) {

      Definitions defs (context);

      if (!defs.lookup_message_type (messageTypeName, result)) {

         if (log) {

            log->warn << "Message type: " << messageTypeName << " not found" << endl;
         }
      }
      else if (log && !result) {

         log->warn << "Message type: " << messageTypeName
            << " look up returned null type" << endl;
      }
   }
   else if (log) {

      log->warn << "Message type config: " << Source.get_name ()
         << " does not contain attribute: " << Name << endl;
   }

   return result;
}


/*!

\brief Creates dmz::Message from dmz::Config.
\details Defined in dmzRuntimeMessaging.h.
This function converts the named attribute to a dmz::Message. It is assumed that the
last part of the \a Name variable specifies the attribute name. If the String
"type.name" is passed in as \a Name, it will try to find a  config context called
"type" and the attribute "name" stored in the "type" config context. If the named
Message does not exist, it is created.
\code
dmz::Message type = dmz::config_create_message_type ("dmz.type.name", global, "DefaultMessage", get_plugin_runtime_context (), &_log);
\endcode
\param[in] Name String containing name of the attribute in the config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] DefaultValue String containing default name of Message to use if not found
int the Config.
\param[in] context Pointer to the runtime context.
\param[in] log Pointer to the dmz::Log to use for log reporting.
\return Returns dmz::Message containing the message type. Returns an empty
dmz::Message if no message name is specified in either the Config or the
\a DefaultValue.

*/
dmz::Message
dmz::config_create_message_type (
      const String &Name,
      const Config &Source,
      const String &DefaultValue,
      RuntimeContext *context,
      Log *log) {

   Message result;

   String messageName;

   if (!Source.lookup_attribute (Name, messageName)) { messageName = DefaultValue; }

   if (messageName) {

      Definitions defs (context);
      defs.create_message_type (messageName, result);
   }

   return result;
}


/*!

\brief Converts Config to name Handle.
\details Defined in dmzRuntimeConfigToNamedHandle.h.
This function converts the named attribute to a named Handle It is assumed that the
last part of the \a Name variable specifies the attribute name. If the String
"type.name" is passed in as \a Name, it will try to find a  config context called
"type" and the attribute "name" stored in the "type" config context.
\code
dmz::Handle handle = dmz::config_to_named_handle ("dmz.handle.name", global, get_plugin_runtime_context (), "DefaultHandleName");
\endcode
\param[in] Name String containing name of the attribute in the config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] DefaultValue String containing name of handle if not found in the Config.
\param[in] context Pointer to the runtime context.
\return Returns dmz::Handle containing the named Handle. Returns a zero handle if no
name is given.

*/
dmz::Handle
dmz::config_to_named_handle (
      const String &Name,
      const Config &Source,
      const String &DefaultValue,
      RuntimeContext *context) {

   Handle result (0);

   const String HandleName (config_to_string (Name, Source, DefaultValue));

   if (HandleName) {

      Definitions defs (context);

      result = defs.create_named_handle (HandleName);
   }

   return result;
}

/*!

\brief Converts Config to a HandleContainer.
\details Defined in dmzRuntimeConfigToNamedHandle.h.
\code
dmz::HandleContainer value = dmz::config_to_handle_container ("dmz.handle", global);
\endcode
The Config for the above example would be formatted as follows:
\code
<dmz>
   <handle value="dmzToolPluginFoo"/>
   <handle value="dmzToolPluginBar"/>
</dmz>
\endcode
\param[in] Name String containing name of config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] context Pointer to the runtime context.
\return Returns dmz::HandleContainer of the String values.

*/
dmz::HandleContainer
dmz::config_to_handle_container (
      const String &Name,
      const Config &Source,
      RuntimeContext *context) {

   HandleContainer result;
   Definitions defs (context);

   Config nameList;

   if (Name) { Source.lookup_all_config (Name, nameList); }
   else { Source.lookup_all_config ("handle", nameList); }

   ConfigIterator it;
   Config name;

   while (nameList.get_next_config (it, name)) {

      result.add_handle (defs.create_named_handle (config_to_string ("value", name)));
   }

   return result;
}


/*!

\brief Converts Config to Data.
\details The named config context
will have children config contexts named "attribute". Each child config context
will have two attributes, one called "name" containing the name of the Data's attribute
and one called "type" containing the BaseTypeEnum of the Data's attribute.
Each "attribute" config context may contain one or more child config contexts.
Each one of these config contexts should have an attribute named "value" that contains
the value of the Data attribute. The order of these attributes is used to determine
the Data attributes element number. A Data object described in XML may look something
like this:
\code
<dmz>
<data>
   <attribute name="id" type="uint32">
      <element value="1"/>
      <element value="5"/>
      <element value="9"/>
      <element value="5"/>
   </attribute>
   <attribute name="position" type="float64">
      <element value="104.5"/>
      <element value="2045.4"/>
      <element value="5.1"/>
   </attribute>
</data>
</dmz>
\endcode
The Data object defined by the above XML may be created as follows:
\code
dmz::Data value;
dmz::config_to_data ("dmz.data", global, get_plugin_runtime_context (), value, &_log);
\endcode
\param[in] Name String containing name of the attribute in the config context to convert.
\param[in] Source Config containing config context to convert.
\param[in] context Pointer to the runtime context.
\param[out] target Data object for to store the result.
\param[in] log Pointer to the dmz::Log to use for log reporting.
\return Returns dmz::True if the config context was successfully converted to a Data
object.
\sa dmz::runtime_init()

*/
dmz::Boolean
dmz::config_to_data (
      const String &Name,
      const Config &Source,
      RuntimeContext *context,
      Data &target,
      Log *log) {

   Boolean result (False);

   Config config (Source);

   if (Name) {

      if (!Source.lookup_config (Name, config)) {

         config.set_config_context (0);

         if (log) {

            log->error << "Unable to find Config definition of Data with name: "
               << Name << " while converting from Config to Data" << endl;
         }
      }
   }

   if (config) {

      result = True;

      Definitions defs (context);

      Config attrConfig;

      config.lookup_all_config ("attribute", attrConfig);

      ConfigIterator it;

      Config attr;

      Boolean found = attrConfig.get_first_config (it, attr);

      while (found) {

         String attrName;

         if (attr.lookup_attribute ("name", attrName)) {

            const UInt32 Handle (defs.create_named_handle (attrName));
            const BaseTypeEnum Type (
               config_to_base_type_enum ("type", attr, BaseTypeUnknown));

            if (Handle) {

               ConfigIterator attrIt;

               Config element;

               Boolean attrFound = attr.get_first_config (attrIt, element);

               UInt32 offset (0);

               while (attrFound) {

                  target.store_string (
                     Handle,
                     offset,
                     Type,
                     config_to_string ("value", element));

                  offset++;

                  attrFound = attr.get_next_config (attrIt, element);
               }
            }
            else if (log) {

               log->error << "Unable to create named handle: " << attrName
                  << " while converting form Config to Data" << endl;
            }
         }
         else if (log) {

            log->error << "Named handle not defined "
               << "while converting form Config to Data" << endl;
         }

         found = attrConfig.get_next_config (it, attr);
      }
   }

   return result;
}


/*!

\brief Converts Vector to Config.
\details Defined in dmzRuntimeConfigWrite.h.
The Vector values are stored in three attributes named: "x", "y", and "z".
\note Unlike the config_to_* functions, the \a Name parameter can not be scoped.
The \a Name parameter should not contain "." characters.
\param[in] Name String containing name of config context to create.
\param[in] Value Vector to convert to Config.
\return Returns a Config object containing the converted Vector.

*/
dmz::Config
dmz::vector_to_config (
      const String &Name,
      const Vector &Value) {

   Config result (Name);

   String val;

   val << Value.get_x ();
   result.store_attribute ("x", val);

   val.flush () << Value.get_y ();
   result.store_attribute ("y", val);

   val.flush () << Value.get_z ();
   result.store_attribute ("z", val);

   return result;
}


/*!

\brief Converts Matrix to Config.
\details Defined in dmzRuntimeConfigWrite.h.
The Matrix values are stored in nine attributes named: "v0" to "v8".
\note Unlike the config_to_* functions, the \a Name parameter can not be scoped.
The \a Name parameter should not contain "." characters.
\param[in] Name String containing name of config context to create.
\param[in] Value Matrix to convert to Config.
\return Returns a Config object containing the converted Matrix.

*/
dmz::Config
dmz::matrix_to_config (
      const String &Name,
      const Matrix &Value) {

   Config result (Name);

   Float64 array[9];

   Value.to_array (array);

   for (Int32 ix = 0; ix < 9; ix++) {

      String attr ("v");
      attr << ix;

      String val;
      val << array[ix];

      result.store_attribute (attr, val);
   }

   return result;
}


/*!

\brief Converts BaseTypeEnum to Config.
\details Defined in dmzRuntimeConfigWrite.h.
\note Unlike the config_to_* functions, the \a Name parameter can not be scoped.
The \a Name parameter should not contain "." characters.
\param[in] Name String containing name of config context to create.
\param[in] AttrName String containing name of attribute to store the Boolean.
\param[in] Value BaseTypeEnum to convert to Config.
\return Returns a Config object containing the converted BaseTypeEnum.

*/
dmz::Config
dmz::base_type_enum_to_config (
      const String &Name,
      const String &AttrName,
      const BaseTypeEnum Value) {

   Config result (Name);

   String val (base_type_enum_to_string (Value));
   result.store_attribute (AttrName, val);

   return result;
}

/*!

\brief Converts Boolean to Config.
\details Defined in dmzRuntimeConfigWrite.h.
The Boolean value will be stored as either of the two strings "true" or "false".
\note Unlike the config_to_* functions, the \a Name parameter can not be scoped.
The \a Name parameter should not contain "." characters.
\param[in] Name String containing name of config context to create.
\param[in] AttrName String containing name of attribute to store the Boolean.
\param[in] Value Boolean to convert to Config.
\return Returns a Config object containing the converted Boolean.

*/
dmz::Config
dmz::boolean_to_config (
      const String &Name,
      const String &AttrName,
      const Boolean Value) {

   Config result (Name);

   String val (Value ? "true" : "false");
   result.store_attribute (AttrName, val);

   return result;
}


/*!

\brief Converts Int32 to Config.
\details Defined in dmzRuntimeConfigWrite.h.
\note Unlike the config_to_* functions, the \a Name parameter can not be scoped.
The \a Name parameter should not contain "." characters.
\param[in] Name String containing name of config context to create.
\param[in] AttrName String containing name of attribute to store the Int32.
\param[in] Value Int32 to convert to Config.
\return Returns a Config object containing the converted Int32.

*/
dmz::Config
dmz::int32_to_config (
      const String &Name,
      const String &AttrName,
      const Int32 Value) {

   Config result (Name);

   String val;
   val << Value;
   result.store_attribute (AttrName, val);

   return result;
}


/*!

\brief Converts UInt32 to Config.
\details Defined in dmzRuntimeConfigWrite.h.
\note Unlike the config_to_* functions, the \a Name parameter can not be scoped.
The \a Name parameter should not contain "." characters.
\param[in] Name String containing name of config context to create.
\param[in] AttrName String containing name of attribute to store the UInt32.
\param[in] Value UInt32 to convert to Config.
\return Returns a Config object containing the converted UInt32.

*/
dmz::Config
dmz::uint32_to_config (
      const String &Name,
      const String &AttrName,
      const UInt32 Value) {

   Config result (Name);

   String val;
   val << Value;
   result.store_attribute (AttrName, val);

   return result;
}


/*!

\brief Converts Int64 to Config.
\details Defined in dmzRuntimeConfigWrite.h.
\note Unlike the config_to_* functions, the \a Name parameter can not be scoped.
The \a Name parameter should not contain "." characters.
\param[in] Name String containing name of config context to create.
\param[in] AttrName String containing name of attribute to store the Int64.
\param[in] Value Int64 to convert to Config.
\return Returns a Config object containing the converted Int64.

*/
dmz::Config
dmz::int64_to_config (
      const String &Name,
      const String &AttrName,
      const Int64 Value) {

   Config result (Name);

   String val;
   val << Value;
   result.store_attribute (AttrName, val);

   return result;
}


/*!

\brief Converts UInt64 to Config.
\details Defined in dmzRuntimeConfigWrite.h.
\note Unlike the config_to_* functions, the \a Name parameter can not be scoped.
The \a Name parameter should not contain "." characters.
\param[in] Name String containing name of config context to create.
\param[in] AttrName String containing name of attribute to store the UInt64.
\param[in] Value UInt64 to convert to Config.
\return Returns a Config object containing the converted UInt64.

*/
dmz::Config
dmz::uint64_to_config (
      const String &Name,
      const String &AttrName,
      const UInt64 Value) {

   Config result (Name);

   String val;
   val << Value;
   result.store_attribute (AttrName, val);

   return result;
}


/*!

\brief Converts Float32 to Config.
\details Defined in dmzRuntimeConfigWrite.h.
\note Unlike the config_to_* functions, the \a Name parameter can not be scoped.
The \a Name parameter should not contain "." characters.
\param[in] Name String containing name of config context to create.
\param[in] AttrName String containing name of attribute to store the Float32.
\param[in] Value Float32 to convert to Config.
\return Returns a Config object containing the converted Float32.

*/
dmz::Config
dmz::float32_to_config (
      const String &Name,
      const String &AttrName,
      const Float32 Value) {

   Config result (Name);

   String val;
   val << Value;
   result.store_attribute (AttrName, val);

   return result;
}


/*!

\brief Converts Float64 to Config.
\details Defined in dmzRuntimeConfigWrite.h.
\note Unlike the config_to_* functions, the \a Name parameter can not be scoped.
The \a Name parameter should not contain "." characters.
\param[in] Name String containing name of config context to create.
\param[in] AttrName String containing name of attribute to store the Float64.
\param[in] Value Float64 to convert to Config.
\return Returns a Config object containing the converted Float64.

*/
dmz::Config
dmz::float64_to_config (
      const String &Name,
      const String &AttrName,
      const Float64 Value) {

   Config result (Name);

   String val;
   val << Value;
   result.store_attribute (AttrName, val);

   return result;
}


/*!

\brief Converts String to Config.
\details Defined in dmzRuntimeConfigWrite.h.
\note Unlike the config_to_* functions, the \a Name parameter can not be scoped.
The \a Name parameter should not contain "." characters.
\param[in] Name String containing name of config context to create.
\param[in] AttrName String containing name of attribute to store the String.
\param[in] Value String to convert to Config.
\return Returns a Config object containing the converted String.

*/
dmz::Config
dmz::string_to_config (
      const String &Name,
      const String &AttrName,
      const String &Value) {

   Config result (Name);

   String val;
   val << Value;
   result.store_attribute (AttrName, val);

   return result;
}


/*!

\brief Converts Data to Config.
\details Defined in dmzRuntimeConfigWrite.h.
\note Unlike the config_to_* functions, the \a Name parameter can not be scoped.
The \a Name parameter should not contain "." characters.
\param[in] Source Data object to convert.
\param[in] context Pointer to the runtime context.
\param[in] log Pointer to the Log to be used for log messages.
\return Returns a Config object containing the converted Data object.
\sa config_to_data(const String &Name, const Config &Source, RuntimeContext *context, Data &target, Log *log)

*/
dmz::Config
dmz::data_to_config (const Data &Source, RuntimeContext *context, Log *log) {

   Config result ("data");

   Definitions defs (context);

   DataIterator it;

   Handle handle (Source.get_first_attribute (it));

   while (handle) {

      Config attr ("attribute");

      attr.store_attribute ("name", defs.lookup_named_handle_name (handle));
      attr.store_attribute (
         "type",
         base_type_enum_to_string (Source.lookup_attribute_base_type_enum (handle)));

      result.add_config (attr);

      const Int32 ElementCount (Source.lookup_attribute_element_count (handle));

      for (Int32 ix = 0; ix < ElementCount; ix++) {

         Config element ("element");

         String value;
         Source.lookup_string (handle, ix, value);
         element.store_attribute ("value", value);

         attr.add_config (element);
      }

      handle = Source.get_next_attribute (it);
   }

   return result;
}

//! @}
