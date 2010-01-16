#include <dmzRuntimeDataConverterTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginInfo.h>
#include "dmzRuntimeDataConverterCommon.h"

/*!

\class dmz::DataConverterBoolean
\ingroup Runtime
\brief Class for converting between a Boolean and a Data object.
\note This class does not serialize a Data object into a Boolean. It provides
a simple way to package a string into a Data object and then extract that Boolean.

*/

struct dmz::DataConverterBoolean::State : public DataConverterCommonStruct {

   State (RuntimeContext *context) {

      set_handle (base_type_enum_to_string (BaseTypeBoolean), context);
   }
};


/*!

\brief Constructor.
\param[in] Info reference to PluginInfo.

*/
dmz::DataConverterBoolean::DataConverterBoolean (const PluginInfo &Info) :
      _state (*(new State (Info.get_context ()))) {;}


/*!

\brief Constructor.
\param[in] context Pointer to the runtime context.

*/
dmz::DataConverterBoolean::DataConverterBoolean (RuntimeContext *context) :
      _state (*(new State (context))) {;}


//! Destructor.
dmz::DataConverterBoolean::~DataConverterBoolean () { delete &_state; }


/*!

\brief Sets the attribute handle used to store and retrieve the value in the Data object.
\param[in] HandleName String containing the name of the attribute handle.
\param[in] context Pointer to the runtime context.
\return Returns the Handle being used.

*/
dmz::Handle
dmz::DataConverterBoolean::set_handle (
      const String &HandleName,
      RuntimeContext *context) {

   return _state.set_handle (HandleName, context);
}


/*!

\brief Converts Data object to a Boolean.
\note This function assumes the Boolean in the Data object was stored
using the dmz::DataConverterBoolean::to_data function.
\param[in] Value Data object to convert to a Boolean.
\param[in] Element Index of attribute element.
\return Returns Boolean containing converted data. The Boolean will be False if
the Data object is not the correct type or is empty.

*/
dmz::Boolean
dmz::DataConverterBoolean::to_boolean (const Data &Value, const Int32 Element) {

   Boolean result (False);

   Value.lookup_boolean (_state.handle, Element, result);

   return result;
}


/*!

\brief Converts Pointer to a Data object to a Boolean.
\note This function assumes the Boolean in the Data object was stored
using the dmz::DataConverterBoolean::to_data function.
\param[in] Value Pointer to the Data object to convert to a Boolean.
\param[in] Element Index of attribute element.
\return Returns Boolean containing converted data. The Boolean will be False if
the Data object is not the correct type or is empty.

*/
dmz::Boolean
dmz::DataConverterBoolean::to_boolean (const Data *Value, const Int32 Element) {

   Boolean result (False);

   if (Value) { result = to_boolean (*Value, Element); }

   return result;
}


/*!

\brief Converts a Boolean to a Data object.
\param[in] Value Boolean containing value to store in returned Data object.
\return Returns a Data object containing the Boolean in \a Value.

*/
dmz::Data
dmz::DataConverterBoolean::to_data (const Boolean Value) {

   Data result;

   if (_state.handle) { result.store_boolean (_state.handle, 0, Value); }

   return result;
}


/*!

\brief Writes a Boolean to a Data object.
\param[in] Value Boolean to store in the Data object.
\param[out] data Data object used to store the \a Value
\param[in] Element Index of attribute element.

*/
void
dmz::DataConverterBoolean::write_data (
      const Boolean Value,
      Data &data,
      const Int32 Element) {

   if (_state.handle) { data.store_boolean (_state.handle, Element, Value); }
}


/*!

\class dmz::DataConverterFloat64
\ingroup Runtime
\brief Class for converting between a Float64 and a Data object.

*/

struct dmz::DataConverterFloat64::State : public DataConverterCommonStruct {

   State (RuntimeContext *context) {

      set_handle (base_type_enum_to_string (BaseTypeFloat64), context);
   }
};


/*!

\brief Constructor.
\param[in] Info reference to PluginInfo.

*/
dmz::DataConverterFloat64::DataConverterFloat64 (const PluginInfo &Info) :
      _state (*(new State (Info.get_context ()))) {;}


/*!

\brief Constructor.
\param[in] context Pointer to the runtime context.

*/
dmz::DataConverterFloat64::DataConverterFloat64 (RuntimeContext *context) :
      _state (*(new State (context))) {;}


//! Destructor.
dmz::DataConverterFloat64::~DataConverterFloat64 () { delete &_state; }


/*!

\brief Sets the attribute handle used to store and retrieve the value in the Data object.
\param[in] HandleName String containing the name of the attribute handle.
\param[in] context Pointer to the runtime context.
\return Returns the Handle being used.

*/
dmz::Handle
dmz::DataConverterFloat64::set_handle (
      const String &HandleName,
      RuntimeContext *context) {

   return _state.set_handle (HandleName, context);
}


/*!

\brief Converts Data object to a Float64.
\note This function assumes the Float64 in the Data object was stored
using the dmz::DataConverterFloat64::to_data function.
\param[in] Value Data object to convert to a Float64.
\param[in] Element Index of attribute element.
\return Returns Float64 containing converted data. The Float64 will be zero if
the Data object is not the correct type or is empty.

*/
dmz::Float64
dmz::DataConverterFloat64::to_float64 (const Data &Value, const Int32 Element) {

   Float64 result (0.0);

   Value.lookup_float64 (_state.handle, Element, result);

   return result;
}


/*!

\brief Converts Pointer to a Data object to a Float64.
\note This function assumes the Float64 in the Data object was stored
using the dmz::DataConverterFloat64::to_data function.
\param[in] Value Pointer to the Data object to convert to a Float64.
\param[in] Element Index of attribute element.
\return Returns Float64 containing converted data. The Float64 will be zero if
the Data object is not the correct type or is empty.

*/
dmz::Float64
dmz::DataConverterFloat64::to_float64 (const Data *Value, const Int32 Element) {

   Float64 result (0.0);

   if (Value) { result = to_float64 (*Value, Element); }

   return result;
}


/*!

\brief Converts a Float64 to a Data object.
\param[in] Value Float64 containing value to store in returned Data object.
\return Returns a Data object containing the string in \a Value.

*/
dmz::Data
dmz::DataConverterFloat64::to_data (const Float64 Value) {

   Data result;

   if (_state.handle) { result.store_float64 (_state.handle, 0, Value); }

   return result;
}


/*!

\brief Writes a Float64 to a Data object.
\param[in] Value Float64 to store in the Data object.
\param[out] data Data object used to store the \a Value
\param[in] Element Index of attribute element.

*/
void
dmz::DataConverterFloat64::write_data (
      const Float64 Value,
      Data &data,
      const Int32 Element) {

   if (_state.handle) { data.store_float64 (_state.handle, Element, Value); }
}


/*!

\class dmz::DataConverterString
\ingroup Runtime
\brief Class for converting between a String and a Data object.
\note This class does not serialize a Data object into a String. It provides
a simple way to package a string into a Data object and then extract that String.

*/

struct dmz::DataConverterString::State : public DataConverterCommonStruct {

   State (RuntimeContext *context)  {

      set_handle (base_type_enum_to_string (BaseTypeString), context);
   }
};


/*!

\brief Constructor.
\param[in] Info reference to PluginInfo.

*/
dmz::DataConverterString::DataConverterString (const PluginInfo &Info) :
      _state (*(new State (Info.get_context ()))) {;}


/*!

\brief Constructor.
\param[in] context Pointer to the runtime context.

*/
dmz::DataConverterString::DataConverterString (RuntimeContext *context) :
      _state (*(new State (context))) {;}


//! Destructor.
dmz::DataConverterString::~DataConverterString () { delete &_state; }


/*!

\brief Sets the attribute handle used to store and retrieve the value in the Data object.
\param[in] HandleName String containing the name of the attribute handle.
\param[in] context Pointer to the runtime context.
\return Returns the Handle being used.

*/
dmz::Handle
dmz::DataConverterString::set_handle (
      const String &HandleName,
      RuntimeContext *context) {

   return _state.set_handle (HandleName, context);
}


/*!

\brief Converts Data object to a String.
\note This function assumes the String in the Data object was stored
using the dmz::DataConverterString::to_data function.
\param[in] Value Data object to convert to a String.
\param[in] Element Index of attribute element.
\return Returns String containing converted data. The String will be empty if
the Data object is not the correct type or is empty.

*/
dmz::String
dmz::DataConverterString::to_string (const Data &Value, const Int32 Element) {

   String result;

   Value.lookup_string (_state.handle, Element, result);

   return result;
}


/*!

\brief Converts Pointer to a Data object to a String.
\note This function assumes the String in the Data object was stored
using the dmz::DataConverterString::to_data function.
\param[in] Value Pointer to the Data object to convert to a String.
\param[in] Element Index of attribute element.
\return Returns String containing converted data. The String will be empty if
the Data object is not the correct type or is empty.

*/
dmz::String
dmz::DataConverterString::to_string (const Data *Value, const Int32 Element) {

   String result;

   if (Value) { result = to_string (*Value, Element); }

   return result;
}


/*!

\brief Converts a String to a Data object.
\param[in] Value String containing value to store in returned Data object.
\return Returns a Data object containing the string in \a Value.

*/
dmz::Data
dmz::DataConverterString::to_data (const String &Value) {

   Data result;

   if (_state.handle) { result.store_string (_state.handle, 0, Value); }

   return result;
}


/*!

\brief Converts a String pointer to a Data object.
\param[in] Value String pointer containing value to store in returned Data object.
\return Returns a Data object containing the string in \a Value.

*/
dmz::Data
dmz::DataConverterString::to_data (const String *Value) {

   Data result;

   if (Value) { result = to_data (*Value); }

   return result;
}


/*!

\brief Writes a String to a Data object.
\param[in] Value String to store in the Data object.
\param[out] data Data object used to store the \a Value
\param[in] Element Index of attribute element.

*/
void
dmz::DataConverterString::write_data (
      const String &Value,
      Data &data,
      const Int32 Element) {

   if (_state.handle) { data.store_string (_state.handle, Element, Value); }
}


/*!

\class dmz::DataConverterHandle
\ingroup Runtime
\brief Class for converting between an Handle and a Data object.
\details This class provides a simple way to package an Handle into a Data
object and then extract that Handle.

*/

struct dmz::DataConverterHandle::State : public DataConverterCommonStruct {

   State (RuntimeContext *context) { set_handle ("handle", context); }
};


/*!

\brief Constructor.
\param[in] Info Reference to PluginInfo.

*/
dmz::DataConverterHandle::DataConverterHandle (const PluginInfo &Info) :
      _state (*(new State (Info.get_context ()))) {;}


/*!

\brief Constructor.
\param[in] context Pointer to the runtime context.

*/
dmz::DataConverterHandle::DataConverterHandle (RuntimeContext *context) :
      _state (*(new State (context))) {;}


//! Destructor.
dmz::DataConverterHandle::~DataConverterHandle () { delete &_state; }


/*!

\brief Sets the attribute handle used to store and retrieve the value in the Data object.
\param[in] HandleName String containing the name of the attribute handle.
\param[in] context Pointer to the runtime context.
\return Returns the Handle being used.

*/
dmz::Handle
dmz::DataConverterHandle::set_handle (
      const String &HandleName,
      RuntimeContext *context) {

   return _state.set_handle (HandleName, context);
}


/*!

\brief Converts a Data object to an Handle.
\note This function assumes the Handle in the Data object was stored
using the dmz::DataConverterHandle::to_data function.
\param[in] Value Data object containing the Handle as an attribute.
\param[in] Element Index of attribute element.
\return Returns the Handle store in the Data object. Will return and empty handle
if the Data object is empty or the wrong type.

*/
dmz::Handle
dmz::DataConverterHandle::to_handle (const Data &Value, const Int32 Element) {

   Handle result (0);

   Value.lookup_uint32 (_state.handle, Element, result);

   return result;
}


/*!

\brief Converts a Data object pointer to an Handle.
\note This function assumes the Handle in the Data object was stored
using the dmz::DataConverterHandle::to_data function.
\param[in] Value Pointer to the Data object containing the Handle as an attribute.
\param[in] Element Index of attribute element.
\return Returns the Handle store in the Data object. Will return and empty handle
if the Data object is empty or the wrong type.

*/
dmz::Handle
dmz::DataConverterHandle::to_handle (const Data *Value, const Int32 Element) {

   Handle result (0);

   if (Value) { result = to_handle (*Value, Element); }

   return result;
}


/*!

\brief Converts an Handle to a Data object.
\param[in] Value Handle containing value to store in returned Data object.
\return Returns a Data object containing the Handle in \a Value.

*/
dmz::Data
dmz::DataConverterHandle::to_data (const Handle &Value) {

   Data result;

   if (_state.handle) { result.store_uint32 (_state.handle, 0, Value); }

   return result;
}


/*!

\brief Writes a Handle to a Data object.
\param[in] Value Handle to store in the Data object.
\param[out] data Data object used to store the \a Value
\param[in] Element Index of attribute element.

*/
void
dmz::DataConverterHandle::write_data (
      const Handle Value,
      Data &data,
      const Int32 Element) {

   if (_state.handle) { data.store_uint32 (_state.handle, Element, Value); }
}


