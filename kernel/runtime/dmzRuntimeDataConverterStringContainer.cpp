#include <dmzRuntimeDataConverterStringContainer.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginInfo.h>
#include "dmzRuntimeDataConverterCommon.h"
/*!

\class dmz::DataConverterStringContainer
\ingroup Runtime
\brief Class for converting between a String and a Data object.
\note This class does not serialize a Data object into a String. It provides
a simple way to package a string into a Data object and then extract that String.

*/

struct dmz::DataConverterStringContainer::State : public DataConverterCommonStruct {

   State (RuntimeContext *context)  {

      set_handle (base_type_enum_to_string (BaseTypeString), context);
   }
};


/*!

\brief Constructor.
\param[in] Info reference to PluginInfo.

*/
dmz::DataConverterStringContainer::DataConverterStringContainer (const PluginInfo &Info) :
      _state (*(new State (Info.get_context ()))) {;}


/*!

\brief Constructor.
\param[in] context Pointer to the runtime context.

*/
dmz::DataConverterStringContainer::DataConverterStringContainer (RuntimeContext *context) :
      _state (*(new State (context))) {;}


//! Destructor.
dmz::DataConverterStringContainer::~DataConverterStringContainer () { delete &_state; }


/*!

\brief Sets the attribute handle used to store and retrieve the value in the Data object.
\param[in] HandleName StringContainer containing the name of the attribute handle.
\param[in] context Pointer to the runtime context.
\return Returns the Handle being used.

*/
dmz::Handle
dmz::DataConverterStringContainer::set_handle (
      const String &HandleName,
      RuntimeContext *context) {

   return _state.set_handle (HandleName, context);
}


/*!

\brief Converts Data object to a StringContainer.
\note This function assumes the StringContainer in the Data object was stored
using the dmz::DataConverterStringContainer::to_data function.
\param[in] Value Data object to convert to a StringContainer.
\return Returns StringContainer containing converted data. The StringContainer will be empty if
the Data object is not the correct type or is empty.

*/
dmz::StringContainer
dmz::DataConverterStringContainer::to_string_container (const Data &Value) {

   StringContainer result;

   const Int32 Count = Value.lookup_attribute_element_count (_state.handle);

   for (Int32 ix = 0; ix < Count; ix++) {

      String str;
      Value.lookup_string (_state.handle, ix, str);
      result.append (str);
   }

   return result;
}


/*!

\brief Converts Pointer to a Data object to a StringContainer.
\note This function assumes the StringContainer in the Data object was stored
using the dmz::DataConverterStringContainer::to_data function.
\param[in] Value Pointer to the Data object to convert to a StringContainer.
\return Returns StringContainer containing converted data. The StringContainer will be empty if
the Data object is not the correct type or is empty.

*/
dmz::StringContainer
dmz::DataConverterStringContainer::to_string_container (const Data *Value) {

   StringContainer result;

   if (Value) { result = to_string_container (*Value); }

   return result;
}


/*!

\brief Converts a StringContainer to a Data object.
\param[in] Value StringContainer containing value to store in returned Data object.
\return Returns a Data object containing the string in \a Value.

*/
dmz::Data
dmz::DataConverterStringContainer::to_data (const StringContainer &Value) {

   Data result;

   if (_state.handle) {

      Int32 count (0);
      StringContainerIterator it;
      String str;

      while (Value.get_next (it, str)) {

         result.store_string (_state.handle, count, str);
         count++;
      }
   }

   return result;
}


/*!

\brief Converts a StringContainer pointer to a Data object.
\param[in] Value StringContainer pointer containing value to store in returned Data object.
\return Returns a Data object containing the string in \a Value.

*/
dmz::Data
dmz::DataConverterStringContainer::to_data (const StringContainer *Value) {

   Data result;

   if (Value) { result = to_data (*Value); }

   return result;
}

