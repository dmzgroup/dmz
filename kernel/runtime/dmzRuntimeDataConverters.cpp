#include <dmzRuntimeDataConverters.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::DataConverterBoolean
\ingroup Runtime
\brief Class for converting between a Boolean and a Data object.
\note This class does not serialize a Data object into a Boolean. It provides
a simple way to package a string into a Data object and then extract that Boolean.

*/

struct dmz::DataConverterBoolean::State {

   Handle handle;

   State (RuntimeContext *context) : handle (0)  {

      Definitions defs (context);

      handle = defs.create_named_handle (base_type_enum_to_string (BaseTypeBoolean));
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

\brief Converts Data object to a Boolean.
\note This function assumes the Boolean in the Data object was stored
using the dmz::DataConverterBoolean::to_data function.
\param[in] Value Data object to convert to a Boolean.
\return Returns Boolean containing converted data. The Boolean will be empty if
the Data object is not the correct type or is empty.

*/
dmz::Boolean
dmz::DataConverterBoolean::to_boolean (const Data &Value) {

   Boolean result (False);

   Value.lookup_boolean (_state.handle, 0, result);

   return result;
}


/*!

\brief Converts Pointer to a Data object to a Boolean.
\note This function assumes the Boolean in the Data object was stored
using the dmz::DataConverterBoolean::to_data function.
\param[in] Value Pointer to the Data object to convert to a Boolean.
\return Returns Boolean containing converted data. The Boolean will be empty if
the Data object is not the correct type or is empty.

*/
dmz::Boolean
dmz::DataConverterBoolean::to_boolean (const Data *Value) {

   Boolean result (False);

   if (Value) { result = to_boolean (*Value); }

   return result;
}


/*!

\brief Converts a Boolean to a Data object.
\param[in] Value Boolean containing value to store in returned Data object.
\return Returns a Data object containing the string in \a Value.

*/
dmz::Data
dmz::DataConverterBoolean::to_data (const Boolean Value) {

   Data result;

   if (_state.handle) { result.store_boolean (_state.handle, 0, Value); }

   return result;
}


/*!

\class dmz::DataConverterString
\ingroup Runtime
\brief Class for converting between a String and a Data object.
\note This class does not serialize a Data object into a String. It provides
a simple way to package a string into a Data object and then extract that String.

*/

struct dmz::DataConverterString::State {

   Handle handle;

   State (RuntimeContext *context) : handle (0)  {

      Definitions defs (context);

      handle = defs.create_named_handle (base_type_enum_to_string (BaseTypeString));
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

\brief Converts Data object to a String.
\note This function assumes the String in the Data object was stored
using the dmz::DataConverterString::to_data function.
\param[in] Value Data object to convert to a String.
\return Returns String containing converted data. The String will be empty if
the Data object is not the correct type or is empty.

*/
dmz::String
dmz::DataConverterString::to_string (const Data &Value) {

   String result;

   Value.lookup_string (_state.handle, 0, result);

   return result;
}


/*!

\brief Converts Pointer to a Data object to a String.
\note This function assumes the String in the Data object was stored
using the dmz::DataConverterString::to_data function.
\param[in] Value Pointer to the Data object to convert to a String.
\return Returns String containing converted data. The String will be empty if
the Data object is not the correct type or is empty.

*/
dmz::String
dmz::DataConverterString::to_string (const Data *Value) {

   String result;

   if (Value) { result = to_string (*Value); }

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

\class dmz::DataConverterHandle
\ingroup Runtime
\brief Class for converting between an Handle and a Data object.
\details This class provides a simple way to package an Handle into a Data
object and then extract that Handle.

*/

struct dmz::DataConverterHandle::State {

   Handle handle;

   State (RuntimeContext *context) : handle (0)  {

      Definitions defs (context);
      handle = defs.create_named_handle ("handle");
   }
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

\brief Converts a Data object to an Handle.
\note This function assumes the Handle in the Data object was stored
using the dmz::DataConverterHandle::to_data function.
\param[in] Value Data object containing the Handle as an attribute.

*/
dmz::Handle
dmz::DataConverterHandle::to_handle (const Data &Value) {

   Handle result (0);

   Value.lookup_uint32 (_state.handle, 0, result);

   return result;
}


/*!

\brief Converts a Data object pointer to an Handle.
\note This function assumes the Handle in the Data object was stored
using the dmz::DataConverterHandle::to_data function.
\param[in] Value Pointer to the Data object containing the Handle as an attribute.

*/
dmz::Handle
dmz::DataConverterHandle::to_handle (const Data *Value) {

   Handle result (0);

   if (Value) { result = to_handle (*Value); }

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

