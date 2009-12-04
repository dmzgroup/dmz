#ifndef DMZ_JSON_PARSER_DOT_H
#define DMZ_JSON_PARSER_DOT_H

#include <dmzTypesBase.h>
#include <dmzTypesString.h>
#include <dmzJSONExport.h>

namespace dmz {

   class JSONInterpreter {

      public:
         virtual ~JSONInterpreter () {;}

         virtual Boolean interpret_null () = 0;
         virtual Boolean interpret_boolean (const Boolean Value) = 0;
         virtual Boolean interpret_number (const String &Value) = 0;
         virtual Boolean interpret_string (const String &Value) = 0;
         virtual Boolean interpret_start_map () = 0;
         virtual Boolean interpret_map_key (const String &Name) = 0;
         virtual Boolean interpret_end_map () = 0;
         virtual Boolean interpret_start_array () = 0;
         virtual Boolean interpret_end_array () = 0;

         virtual String get_error () = 0;

      protected:
         JSONInterpreter () {;}

      private:
         JSONInterpreter (const JSONInterpreter &Interpreter);
         JSONInterpreter &operator= (const JSONInterpreter &Interpreter);
   };

   class DMZ_JSON_LINK_SYMBOL JSONParser {

      public:
         JSONParser ();
         ~JSONParser ();

         void set_interpreter (JSONInterpreter *interpreter);

         void reset ();

         Boolean parse_buffer (
            const char *buffer,
            const Int32 Length,
            const Boolean EndOfStream);

         String get_error ();

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         JSONParser (const JSONParser &Parser);
         JSONParser &operator= (const JSONParser &Parser);
   };
};

#endif // DMZ_JSON_PARSER_DOT_H
