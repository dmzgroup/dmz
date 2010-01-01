#include <dmzFoundationInterpreterJSONConfig.h>
#include <dmzFoundationJSONUtil.h>
#include <dmzFoundationParserJSON.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeVersion.h>
#include <dmzSystemFile.h>
#include <dmzSystemStream.h>
#include <dmzTypesStringUtil.h>

namespace {

static inline dmz::Boolean
isdigit (const char Val) { return (Val >= '0') && (Val <= '9'); }

}

/*!

\brief Determines if a String contains a legally formatted JSON number.
\ingroup Foundation
\param[in] Value String containing the text to test.
\return Returns dmz::True if the String contains a valid JSON number.

*/
dmz::Boolean
dmz::json_is_number (const String &Value) {

   Boolean result (False);

   const Int32 Length (Value.get_length ());
   const char *Buf = Value.get_buffer ();

   if (Buf && Length) {

      result = True;

      Boolean done (False);
      Boolean leadingZero (False);
      Boolean foundDecimal (False);
      Boolean foundExp (False);
      Int32 digitCount (0);
      Int32 place (0);

      while (!done && result) {

         const char Val = Buf[place];

         if (isdigit (Val)) {

            if ((0 == digitCount) && (Val == '0')) {

               if (!foundDecimal) { leadingZero = True; }
               else if (foundDecimal && foundExp) { leadingZero = True; }
            }
            else if (leadingZero) { result = False; }

            digitCount++;
         }
         else if (Val == '-') {

            if (place != 0) { result = False; }
         }
         else if (Val == '.') {

            if (foundDecimal) { result = False; }
            else if (foundExp) { result = False; }
            else {

               if (digitCount == 0) { result = False; }
               else {

                  foundDecimal = True;
                  leadingZero = False;
                  digitCount = 0;
               }
            }
         }
         else if ((Val == 'e') || (Val == 'E')) {

            if (digitCount == 0) { result = False; }
            else if (!foundExp && ((place + 1) < Length)) {

               leadingZero = False;
               digitCount = 0;
               foundExp = True;
               const char Next = Buf[place + 1];

               if ((Next == '+') || (Next == '-')) {

                  place++;
                  if (place >= Length) { result = False; }
               }
            }
            else { result = False; }
         }
         else { result = False; }

         if (result && !done) {

            place++;

            if (place >= Length) {

               if (digitCount == 0) { result = False; }
               else { done = True; }
            }
         }
      }
   }

   return result;
}


/*!

\ingroup Foundation
\brief Converts an JSON String to a config context tree.
\details Defined in dmzFoundationJSONUtil.h.
\param[in] Value String containing the JSON  to parse.
\param[out] data Config object to store parsed JSON data.
\param[in] log Pointer to Log for streaming log messages.
\return Returns dmz::True if the JSON was successfully parsed.
\sa dmz::Config \n dmz::ConfigContext

*/
dmz::Boolean
dmz::json_string_to_config (const String &Value, Config &data, Log *log) {

   ParserJSON parser;
   InterpreterJSONConfig interpreter (data);
   parser.set_interpreter (&interpreter);
   Boolean error (False);

   if (!parser.parse_buffer (Value.get_buffer (), Value.get_length (), True)) {

      error = True;

      if (log) {

         log->error << "Error parsing: " << endl << "\t"
            << Value << endl << "\t" << parser.get_error () << endl;
      }
   }

   return !error;
}


/*!

\ingroup Foundation
\brief Converts an JSON file to a config context tree.
\details Defined in dmzFoundationJSONUtil.h.
\param[in] FileName String containing name of JSON file to parse.
\param[out] data Config object to store parsed JSON data.
\param[in] log Pointer to Log for streaming log messages.
\return Returns dmz::True if the JSON file was successfully parsed.
\sa dmz::Config \n dmz::ConfigContext

*/
dmz::Boolean
dmz::json_to_config (const String &FileName, Config &data, Log *log) {

   if (!data) { Config tmp ("global"); data = tmp; }

   ParserJSON parser;
   InterpreterJSONConfig interpreter (data);
   parser.set_interpreter (&interpreter);
   Boolean error (False);

   FILE *file = open_file (FileName, "rb");

   if (file) {

      String buffer;

      while (read_file (file, 1024, buffer) && !error) {

         const Int32 Length = buffer.get_length ();
         const char *cbuf = buffer.get_buffer ();

         if (!parser.parse_buffer (cbuf, Length, Length < 1024)) {

            error = True;

            if (log) {

               // JSON return multi line errors so start error message on own line.
               log->error << "In file: " << FileName << endl
                  << parser.get_error () << endl;
            }
         }
      }

      close_file (file);
   }
   else {

      error = True;

      if (log) { log->error << "Unable to open file: " << FileName << endl; }
   }

   return !error;
}


/*!

\ingroup Foundation
\brief converts an JSON file to a Version

*/
dmz::Boolean
dmz::json_to_version (const String &FileName, Version &value, Log *log) {

   Config data ("global");
   Boolean result = json_to_config (FileName, data, log);

   if (result) {

      Version tmp (data); 
      value = tmp;
   }

   return result;
}

