/*!

\class dmz::Parser
\ingroup Foundation
\brief Stream based parser interface.
\sa dmz::ParserXML \n dmz::ParserJSON

\fn dmz::Parser::Parser ()
\brief Constructor.

\fn dmz::Parser::~Parser ()
\brief Destructor.

\fn void dmz::Parser::reset ()
\brief Resets parser.

\fn dmz::Boolean dmz::Parser::parse_buffer (const char *buffer, const Int32 Length, const Boolean EndOfStream)
\brief Parsers buffer.
\param[in] buffer Data to be parsed.
\param[in] Length Size of the buffer.
\param[in] EndOfStream Boolean to indicate the end of the data stream.
\return Returns dmz::True if the buffer was parsed successfully.

\fn dmz::String dmz::Parser::get_error ()
\brief Returns last error.
\return Returns a String containing the last error.

*/
