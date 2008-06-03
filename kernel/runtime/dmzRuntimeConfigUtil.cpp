#include <dmzTypesBase.h>
#include <dmzTypesString.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimeConfigWrite.h>

/*!

\file dmzRuntimeConfigRead.h
\brief Contains functions to convert config contexts into various object types.

\file dmzRuntimeConfigWrite.h
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

\brief Converts Config to Vector.
\details Defined in dmzRuntimeConfigRead.h.
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

   if (Name) {

      Source.lookup_config (Name, cd);
   }
   else { cd = Source; }

   const Vector Result (
      config_to_float64 ("x", cd, DefaultValue.get_x ()),
      config_to_float64 ("y", cd, DefaultValue.get_y ()),
      config_to_float64 ("z", cd, DefaultValue.get_z ()));

   return Result;
}


/*!

\brief Converts Config to Matrix.
\details Defined in dmzRuntimeConfigRead.h.
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

   if (Name) {

      Source.lookup_config (Name, cd);
   }
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
\details Defined in dmzRuntimeConfigRead.h.
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
\details Defined in dmzRuntimeConfigRead.h.
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
\details Defined in dmzRuntimeConfigRead.h.
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
\details Defined in dmzRuntimeConfigRead.h.
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
\details Defined in dmzRuntimeConfigRead.h.
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
\details Defined in dmzRuntimeConfigRead.h.
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
\details Defined in dmzRuntimeConfigRead.h.
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
\details Defined in dmzRuntimeConfigRead.h.
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
\details Defined in dmzRuntimeConfigRead.h.
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
\details Defined in dmzRuntimeConfigRead.h.
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
\details Defined in dmzRuntimeConfigRead.h.
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

   String name;

   if (!Source.lookup_attribute (Name, name)) { name = DefaultValue; }

   if (name) {

      Definitions defs (context);
      defs.create_message_type (name, result);
   }

   return result;
}


/*!

\brief Converts Config to name Handle.
\details Defined in dmzRuntimeConfigRead.h.
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
