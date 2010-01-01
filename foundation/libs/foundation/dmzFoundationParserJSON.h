#ifndef DMZ_FOUNDATION_PARSER_JSON_DOT_H
#define DMZ_FOUNDATION_PARSER_JSON_DOT_H

#include <dmzFoundationExport.h>
#include <dmzFoundationParser.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class InterpreterJSON {

      public:
         virtual ~InterpreterJSON () {;}

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
         InterpreterJSON () {;}

      private:
         InterpreterJSON (const InterpreterJSON &);
         InterpreterJSON &operator= (const InterpreterJSON &);
   };

   class DMZ_FOUNDATION_LINK_SYMBOL ParserJSON : public Parser {

      public:
         ParserJSON ();
         ~ParserJSON ();

         void set_interpreter (InterpreterJSON *interpreter);

         // Parser Interface
         virtual void reset ();

         virtual Boolean parse_buffer (
            const char *buffer,
            const Int32 Length,
            const Boolean EndOfStream);

         virtual String get_error ();

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         ParserJSON (const ParserJSON &);
         ParserJSON &operator= (const ParserJSON &);
   };
};

#endif // DMZ_FOUNDATION_PARSER_JSON_DOT_H
