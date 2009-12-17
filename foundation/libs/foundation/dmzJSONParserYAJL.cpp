#include <dmzJSONParser.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <yajl_parse.h>

#include <stdio.h>

using namespace dmz;

namespace {

typedef const unsigned char * JSONText;

struct ParseStruct {

   Boolean error;
   JSONInterpreter *interpreter;
   yajl_handle parser;

   ParseStruct () : error (False), interpreter (0), parser (0) {;}
};

static int
local_interpret_null (void *ctx) {

   int result (0);

   ParseStruct *ps = (ParseStruct *)ctx;

   if (ps && ps->interpreter) { result = ps->interpreter->interpret_null () ? 1 : 0; }

   return result;
}

static int
local_interpret_boolean (void * ctx, int boolVal) {

   int result (0);

   ParseStruct *ps = (ParseStruct *)ctx;

   if (ps && ps->interpreter) {

      result = ps->interpreter->interpret_boolean (boolVal ? True : False) ? 1: 0;
   }

   return result;
}


static int
local_interpret_number (void * ctx, const char * numberVal, unsigned int numberLen) {

   int result (0);

   ParseStruct *ps = (ParseStruct *)ctx;

   if (ps && ps->interpreter) {

      String value (numberVal, numberLen);
      result = ps->interpreter->interpret_number (value) ? 1: 0;
   }

   return result;
}


static int
local_interpret_string (
      void * ctx,
      const unsigned char * stringVal,
      unsigned int stringLen) {

   int result (0);

   ParseStruct *ps = (ParseStruct *)ctx;

   if (ps && ps->interpreter) {

      String value ((const char *)stringVal, stringLen);
      result = ps->interpreter->interpret_string (value) ? 1: 0;
   }

   return result;
}


static int
local_interpret_start_map (void * ctx) {

   int result (0);

   ParseStruct *ps = (ParseStruct *)ctx;

   if (ps && ps->interpreter) {

      result = ps->interpreter->interpret_start_map () ? 1: 0;
   }

   return result;
}


static int
local_interpret_map_key (void * ctx, const unsigned char * key, unsigned int stringLen) {

   int result (0);

   ParseStruct *ps = (ParseStruct *)ctx;

   if (ps && ps->interpreter) {

      String value ((const char *)key, stringLen);
      result = ps->interpreter->interpret_map_key (value) ? 1: 0;
   }

   return result;
}


static int
local_interpret_end_map (void * ctx) {

   int result (0);

   ParseStruct *ps = (ParseStruct *)ctx;

   if (ps && ps->interpreter) {

      result = ps->interpreter->interpret_end_map () ? 1: 0;
   }

   return result;
}


static int
local_interpret_start_array (void * ctx) {

   int result (0);

   ParseStruct *ps = (ParseStruct *)ctx;

   if (ps && ps->interpreter) {

      result = ps->interpreter->interpret_start_array () ? 1: 0;
   }

   return result;
}


static int
local_interpret_end_array (void * ctx) {

   int result (0);

   ParseStruct *ps = (ParseStruct *)ctx;

   if (ps && ps->interpreter) {

      result = ps->interpreter->interpret_end_array () ? 1: 0;
   }

   return result;
}

static yajl_callbacks cb = {
   local_interpret_null,
   local_interpret_boolean,
   NULL,
   NULL,
   local_interpret_number,
   local_interpret_string,
   local_interpret_start_map,
   local_interpret_map_key,
   local_interpret_end_map,
   local_interpret_start_array,
   local_interpret_end_array
};

};


/*!

\class dmz::JSONParser
\ingroup Foundation
\brief JSON Parser.
\details This class parses an JSON stream and passes the result to a dmz::JSONInterpreter.
\sa dmz::JSONInterpreter

*/

struct dmz::JSONParser::State {

   ParseStruct ps;
   Boolean firstParse;
   String error;

   State () : firstParse (True) {;}

   void set_up () {

      firstParse = False;
      if (ps.parser) { yajl_free (ps.parser); ps.parser = 0; }

      yajl_parser_config cg = { 1, 1 };

      ps.parser = yajl_alloc (&cb, &cg, 0, (void *)&ps);
   }

   void clean_up () {

      firstParse = True;
      if (ps.parser) { yajl_free (ps.parser); ps.parser = 0; }
   }

};


dmz::JSONParser::JSONParser () : _state (*(new State)) {;}


dmz::JSONParser::~JSONParser () {

   _state.clean_up ();
   delete &_state;
}


/*!

\brief Sets current JSONInterpreter.
\param[in] interpreter Pointer to the JSONInterpreter to use to interpret the parsed JSON.

*/
void
dmz::JSONParser::set_interpreter (JSONInterpreter *interpreter) {

   _state.ps.interpreter = interpreter;
}


//! Resets the parser.
void
dmz::JSONParser::reset () { _state.firstParse = True; _state.error.flush (); }


/*!

\brief Parses the buffer.
\details The JSON data may be parsed in pieces. The \a Buffer does not need to contain
all the JSON data. This way an JSON file may be read in chunks and parsed.
\param[in] Buffer Character array container JSON data.
\param[in] Length Number of bytes of data in the \a Buffer.
\param[in] EndOfStream When set to dmz::True, it indicates that the buffer contains
the end of the JSON data.
\return Returns dmz::True if the JSON data was successfully parsed.

*/
dmz::Boolean
dmz::JSONParser::parse_buffer (
      const char *Buffer,
      const Int32 Length,
      const Boolean EndOfStream) {

   Boolean result (False);

   const unsigned int ULength = (Length > 0) ? (const unsigned int) Length : 0;

   if (_state.firstParse) { _state.set_up (); }

   const yajl_status Status = (Buffer && Length) ?
      yajl_parse (_state.ps.parser, (JSONText)Buffer, ULength) : yajl_status_ok;

   if ((Status == yajl_status_ok) ||
         ((Status == yajl_status_insufficient_data) && !EndOfStream)) { result = True; }
   else {

      if (Status == yajl_status_error) {

         char *error =
            (char *)yajl_get_error (_state.ps.parser, 1, (JSONText)Buffer, ULength);

         if (error) {

            _state.error = error;
            yajl_free_error (_state.ps.parser, (unsigned char *)error);
         }
         else { _state.error = "Unknown JSON parser error."; }
      }
      else if (Status == yajl_status_client_canceled) {

         _state.error = "Internal client error.";
      }
   }

   if (result && EndOfStream) {

      const yajl_status Status = yajl_parse_complete (_state.ps.parser);

      if (Status != yajl_status_ok) {

         _state.error = "Incomplete JSON data.";
         result = False;
      }
   }

   return result;
}


//! Gets last parsing error.
dmz::String
dmz::JSONParser::get_error () { return _state.error; }

