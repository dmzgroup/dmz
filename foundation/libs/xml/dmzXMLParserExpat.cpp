#include <dmzXMLParser.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <expat.h>

#include <stdio.h>

namespace {

   struct ParseStruct {

      dmz::Boolean error;
      dmz::XMLInterpreter *interpreter;
      XML_Parser parser;

      ParseStruct () : error (dmz::False), interpreter (0), parser (0) {;}
   };
};


static void XMLCALL
local_element_start (void *data, const char *el, const char **attr) {

   ParseStruct *ps = (ParseStruct *)data;

   if (ps && ps->interpreter) {

      const dmz::String Name (el);

      dmz::HashTableStringTemplate<dmz::String> table;

      if (attr) {

         dmz::Int32 count = 0;

         dmz::String attName;
         while (attr[count]) {

            attName = attr[count];
            dmz::String *value = new dmz::String (attr[count + 1]);

            if (!table.store (attName, value)) {

               fprintf (stderr, "Duplicate element: %s\n", attName.get_buffer ());
               delete value; value = 0;
            }

            count += 2;
         }
      }

      if (!ps->interpreter->interpret_start_element (Name, table)) {

         XML_StopParser (ps->parser, XML_FALSE);
         ps->error = dmz::True;
      }

      table.empty ();
   }
}


static void XMLCALL
local_element_end (void *data, const char *el) {

   ParseStruct *ps = (ParseStruct *)data;

   if (ps && ps->interpreter) {

      const dmz::String Name (el);

      if (!ps->interpreter->interpret_end_element (Name)) {

         XML_StopParser (ps->parser, XML_FALSE);
         ps->error = dmz::True;
      }
   }
}


static void XMLCALL
local_character_data (void *data, const XML_Char *charData, int len) {

   ParseStruct *ps = (ParseStruct *)data;

   if (ps && ps->interpreter) {

      const dmz::String Data (charData, len);

      if (!ps->interpreter->interpret_character_data (Data)) {

         XML_StopParser (ps->parser, XML_FALSE);
         ps->error = dmz::True;
      }
   }
}


static void XMLCALL
local_cdata_start (void *data) {

   ParseStruct *ps = (ParseStruct *)data;

   if (ps && ps->interpreter) {

      if (!ps->interpreter->interpret_start_cdata_section ()) {

         XML_StopParser (ps->parser, XML_FALSE);
         ps->error = dmz::True;
      }
   }
}


static void XMLCALL
local_cdata_end (void *data) {

   ParseStruct *ps = (ParseStruct *)data;

   if (ps && ps->interpreter) {

      if (!ps->interpreter->interpret_end_cdata_section ()) {

         XML_StopParser (ps->parser, XML_FALSE);
         ps->error = dmz::True;
      }
   }
}

/*!

\class dmz::XMLParser
\ingroup Foundation
\brief XML Parser.
\details This class parses an XML stream and passes the result to a dmz::XMLInterpreter.
\sa dmz::XMLInterpreter

*/

struct dmz::XMLParser::State {

   ParseStruct ps;
   Boolean firstParse;
   String error;

   State () : firstParse (True) {;}

   void set_up () {

      firstParse = False;
      if (ps.parser) { XML_ParserFree (ps.parser); ps.parser = 0; }

      ps.parser = XML_ParserCreate (0);

      if (ps.parser) {

         XML_SetUserData (ps.parser, (void *)&ps);
         XML_SetElementHandler (ps.parser, local_element_start, local_element_end);
         XML_SetCharacterDataHandler (ps.parser, local_character_data);
         XML_SetCdataSectionHandler (ps.parser, local_cdata_start, local_cdata_end);
      }
   }

   void clean_up () {

      firstParse = True;
      if (ps.parser) { XML_ParserFree (ps.parser); ps.parser = 0; }
   }
};


dmz::XMLParser::XMLParser () : _state (*(new State)) {;}


dmz::XMLParser::~XMLParser () {

   _state.clean_up ();
   delete &_state;
}


/*!

\brief Sets current XMLInterpreter.
\param[in] interpreter Pointer to the XMLInterpreter to use to interpret the parsed XML.

*/
void
dmz::XMLParser::set_interpreter (XMLInterpreter *interpreter) {

   _state.ps.interpreter = interpreter;
}


//! Resets the parser.
void
dmz::XMLParser::reset () { _state.firstParse = True; _state.error.flush (); }


/*!

\brief Parses the buffer.
\details The XML data may be parsed in pieces. The \a Buffer does not need to contain
all the XML data. This way an XML file may be read in chunks and parsed.
\param[in] Buffer Character array container XML data.
\param[in] Length Number of bytes of data in the \a Buffer.
\param[in] EndOfStream When set to dmz::True, it indicates that the buffer contains
the end of the XML data.
\return Returns dmz::True if the XML data was successfully parsed.

*/
dmz::Boolean
dmz::XMLParser::parse_buffer (
      const char *Buffer,
      const Int32 Length,
      const Boolean EndOfStream) {

   Boolean result (False);

   if (_state.firstParse) { _state.set_up (); }

   const XML_Status Status = XML_Parse (_state.ps.parser, Buffer, Length, EndOfStream);

   if (Status == XML_STATUS_OK) { result = True; }
   else {

      _state.error.flush () << XML_GetCurrentLineNumber (_state.ps.parser) << ": ";

      String errorMsg;

      if (_state.ps.error && _state.ps.interpreter) {

         _state.error << _state.ps.interpreter->get_error ();
      }
      else if (Status == XML_STATUS_ERROR) {

         _state.error << XML_ErrorString (XML_GetErrorCode (_state.ps.parser));
      }
      else { _state.error << "Unknown Error"; }
   }

   if (EndOfStream || !result) { _state.clean_up (); }

   return result;
}


//! Gets last parsing error.
dmz::String
dmz::XMLParser::get_error () { return _state.error; }

