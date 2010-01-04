/*!

\class dmz::InterpreterXML
\ingroup Foundation
\brief XML interpreter interface.
\details Interface used by dmz::ParserXML to process the parsed XML.
\sa dmz::ParserXML

\fn dmz::Boolean dmz::InterpreterXML::interpret_start_element (
const String &Name,
const HashTableStringTemplate<String> &AttributeTable)
\brief Indicates the start of an element.
\param[in] Name String containing name of the XML element.
\param[in] AttributeTable Hash table containing all of the elements attributes.

\fn dmz::Boolean dmz::InterpreterXML::interpret_end_element (const String &Name)
\brief Indicates the end of an element.
\param[in] Name String containing the name of the closed element.

\fn dmz::Boolean dmz::InterpreterXML::interpret_character_data (const String &Data)
\brief Passes in character data enclosed in an element.
\note The character data may not come in one call but may be broken in to smaller pieces
that are passed in one at a time.

\fn dmz::Boolean dmz::InterpreterXML::interpret_start_cdata_section ()
\brief Indicates the start of a CDATA section.

\fn dmz::Boolean dmz::InterpreterXML::interpret_end_cdata_section ()
\brief Indicates the end of a CDATA section.

\fn dmz::String dmz::InterpreterXML::get_error ()
\brief Gets last error.
\return Returns a dmz::String containing a description of the last error encountered.
Will be an empty string if no error was encountered.

*/
