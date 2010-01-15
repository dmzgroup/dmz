#include <dmzRuntimeDataConverterPathContainer.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginInfo.h>
#include "dmzRuntimeDataConverterCommon.h"
/*!

\class dmz::DataConverterPathContainer
\ingroup Runtime
\brief Class for converting between a String and a Data object.
\note This class does not serialize a Data object into a String. It provides
a simple way to package a string into a Data object and then extract that String.

*/

struct dmz::DataConverterPathContainer::State : public DataConverterCommonStruct {

   State (RuntimeContext *context)  {

      set_handle (base_type_enum_to_string (BaseTypeString), context);
   }
};


/*!

\brief Constructor.
\param[in] Info reference to PluginInfo.

*/
dmz::DataConverterPathContainer::DataConverterPathContainer (const PluginInfo &Info) :
      _state (*(new State (Info.get_context ()))) {;}


/*!

\brief Constructor.
\param[in] context Pointer to the runtime context.

*/
dmz::DataConverterPathContainer::DataConverterPathContainer (RuntimeContext *context) :
      _state (*(new State (context))) {;}


//! Destructor.
dmz::DataConverterPathContainer::~DataConverterPathContainer () { delete &_state; }


/*!

\brief Sets the attribute handle used to store and retrieve the value in the Data object.
\param[in] HandleName PathContainer containing the name of the attribute handle.
\param[in] context Pointer to the runtime context.
\return Returns the Handle being used.

*/
dmz::Handle
dmz::DataConverterPathContainer::set_handle (
      const String &HandleName,
      RuntimeContext *context) {

   return _state.set_handle (HandleName, context);
}


/*!

\brief Converts Data object to a PathContainer.
\note This function assumes the PathContainer in the Data object was stored
using the dmz::DataConverterPathContainer::to_data function.
\param[in] Value Data object to convert to a PathContainer.
\return Returns PathContainer containing converted data. The PathContainer will be empty if
the Data object is not the correct type or is empty.

*/
dmz::PathContainer
dmz::DataConverterPathContainer::to_string_container (const Data &Value) {

   PathContainer result;

   const Int32 Count = Value.lookup_attribute_element_count (_state.handle);

   for (Int32 ix = 0; ix < Count; ix++) {

      String str;
      Value.lookup_string (_state.handle, ix, str);
      result.add_path (str);
   }

   return result;
}


/*!

\brief Converts Pointer to a Data object to a PathContainer.
\note This function assumes the PathContainer in the Data object was stored
using the dmz::DataConverterPathContainer::to_data function.
\param[in] Value Pointer to the Data object to convert to a PathContainer.
\return Returns PathContainer containing converted data. The PathContainer will be empty if
the Data object is not the correct type or is empty.

*/
dmz::PathContainer
dmz::DataConverterPathContainer::to_string_container (const Data *Value) {

   PathContainer result;

   if (Value) { result = to_string_container (*Value); }

   return result;
}


/*!

\brief Converts a PathContainer to a Data object.
\param[in] Value PathContainer containing value to store in returned Data object.
\return Returns a Data object containing the string in \a Value.

*/
dmz::Data
dmz::DataConverterPathContainer::to_data (const PathContainer &Value) {

   Data result;

   if (_state.handle) {

      Int32 count (0);
      PathContainerIterator it;
      String str;

      while (Value.get_next (it, str)) {

         result.store_string (_state.handle, count, str);
         count++;
      }
   }

   return result;
}


/*!

\brief Converts a PathContainer pointer to a Data object.
\param[in] Value PathContainer pointer containing value to store in returned Data object.
\return Returns a Data object containing the string in \a Value.

*/
dmz::Data
dmz::DataConverterPathContainer::to_data (const PathContainer *Value) {

   Data result;

   if (Value) { result = to_data (*Value); }

   return result;
}

