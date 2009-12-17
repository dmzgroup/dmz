/*!

\class dmz::JSONInterpreter
\ingroup Foundation
\brief JSON interpreter interface.
\details Interface used by dmz::JSONParser to process the parsed JSON.
\sa dmz::XMLParser

\fn dmz::JSONInterpreter::JSONInterpreter ()
\brief Constructor.

\fn dmz::JSONInterpreter::~JSONInterpreter ()
\brief Destructor

\fn dmz::Boolean dmz::JSONInterpreter::interpret_null ()
\brief Indicates a NULL JSON parameter.
\return Returns False on internal interpreter error.

\fn dmz::Boolean dmz::JSONInterpreter::interpret_boolean (const Boolean Value)
\brief Indicates a Boolean JSON parameter.
\param[in] Value Boolean containing the value of the parameter.
\return Returns False on internal interpreter error.

\fn dmz::Boolean dmz::JSONInterpreter::interpret_number (const String &Value)
\brief Indicates a number JSON parameter.
\param[in] Value String containing the value of the parameter.
\return Returns False on internal interpreter error.
\sa dmz::json_is_number()

\fn dmz::Boolean dmz::JSONInterpreter::interpret_string (const String &Value)
\brief Indicates a String JSON parameter.
\param[in] Value String containing the value of the parameter.
\return Returns False on internal interpreter error.

\fn dmz::Boolean dmz::JSONInterpreter::interpret_start_map ()
\brief Indicates the start of a JSON map.
\return Returns False on internal interpreter error.

\fn dmz::Boolean dmz::JSONInterpreter::interpret_map_key (const String &Name)
\brief Indicates a map key name.
\param[in] Name String containing the name of the map key.
\return Returns False on internal interpreter error.

\fn dmz::Boolean dmz::JSONInterpreter::interpret_end_map ()
\brief Indicates the end of a JSON map.
\return Returns False on internal interpreter error.

\fn dmz::Boolean dmz::JSONInterpreter::interpret_start_array ()
\brief Indicates the start of a JSON array.
\return Returns False on internal interpreter error.

\fn dmz::Boolean dmz::JSONInterpreter::interpret_end_array ()
\brief Indicates the end of a JSON array.
\return Returns False on internal interpreter error.

\fn dmz::String dmz::JSONInterpreter::get_error ()
\brief Returns a String describing the last interpreter error.

*/
