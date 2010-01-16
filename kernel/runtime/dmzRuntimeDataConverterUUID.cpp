#include <dmzRuntimeDataConverterUUID.h>
#include "dmzRuntimeDataConverterCommon.h"
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::DataConverterUUID
\ingroup Runtime
\brief Class for converting between a UUID and a Data object.
\note This class does not serialize a Data object into a UUID. It provides
a simple way to package a string into a Data object and then extract that UUID.

*/

struct dmz::DataConverterUUID::State : public DataConverterCommonStruct {

   State (RuntimeContext *context) {

      set_handle ("UUID", context);
   }
};


/*!

\brief Constructor.
\param[in] Info reference to PluginInfo.

*/
dmz::DataConverterUUID::DataConverterUUID (const PluginInfo &Info) :
      _state (*(new State (Info.get_context ()))) {;}


/*!

\brief Constructor.
\param[in] context Pointer to the runtime context.

*/
dmz::DataConverterUUID::DataConverterUUID (RuntimeContext *context) :
      _state (*(new State (context))) {;}


//! Destructor.
dmz::DataConverterUUID::~DataConverterUUID () { delete &_state; }


/*!

\brief Sets the attribute handle used to store and retrieve the value in the Data object.
\param[in] HandleName String containing the name of the attribute handle.
\param[in] context Pointer to the runtime context.
\return Returns the Handle being used.

*/
dmz::Handle
dmz::DataConverterUUID::set_handle (
      const String &HandleName,
      RuntimeContext *context) {

   return _state.set_handle (HandleName, context);
}


/*!

\brief Converts Data object to a UUID.
\note This function assumes the UUID in the Data object was stored
using the dmz::DataConverterUUID::to_data function.
\param[in] Value Data object to convert to a UUID.
\param[in] Element Index of attribute element.
\return Returns UUID containing converted data. The UUID will be invalid if
the Data object is not the correct type or is empty.

*/
dmz::UUID
dmz::DataConverterUUID::to_uuid (const Data &Value, const Int32 Element) {

   UUID result;

   Value.lookup_uuid (_state.handle, Element, result);

   return result;
}


/*!

\brief Converts Pointer to a Data object to a UUID.
\note This function assumes the UUID in the Data object was stored
using the dmz::DataConverterUUID::to_data function.
\param[in] Value Pointer to the Data object to convert to a UUID.
\param[in] Element Index of attribute element.
\return Returns UUID containing converted data. The UUID will be invalid if
the Data object is not the correct type or is empty.

*/
dmz::UUID
dmz::DataConverterUUID::to_uuid (const Data *Value, const Int32 Element) {

   UUID result;

   if (Value) { result = to_uuid (*Value, Element); }

   return result;
}


/*!

\brief Converts a UUID to a Data object.
\param[in] Value UUID containing value to store in returned Data object.
\return Returns a Data object containing the UUID in \a Value.

*/
dmz::Data
dmz::DataConverterUUID::to_data (const UUID &Value) {

   Data result (_state.context);

   if (_state.handle) { result.store_uuid (_state.handle, 0, Value); }

   return result;
}


/*!

\brief Writes a UUID to a Data object.
\param[in] Value UUID to store in the Data object.
\param[out] data Data object used to store the \a Value
\param[in] Element Index of attribute element.

*/
void
dmz::DataConverterUUID::write_data (
      const UUID &Value,
      Data &data,
      const Int32 Element) {

   if (_state.handle) { data.store_uuid (_state.handle, Element, Value); }
}

