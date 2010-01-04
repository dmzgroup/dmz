#ifndef DMZ_FOUNDATION_INTERPRETER_JSON_CONFIG_DOT_H
#define DMZ_FOUNDATION_INTERPRETER_JSON_CONFIG_DOT_H

#include <dmzFoundationExport.h>
#include <dmzFoundationParserJSON.h>
#include <dmzTypesBase.h>

namespace dmz {

   class Config;

   class DMZ_FOUNDATION_LINK_SYMBOL InterpreterJSONConfig : public InterpreterJSON {

      public:
         InterpreterJSONConfig (const Config &RootConfig);
         virtual ~InterpreterJSONConfig ();

         virtual Boolean interpret_null ();
         virtual Boolean interpret_boolean (const Boolean Value);
         virtual Boolean interpret_number (const String &Value);
         virtual Boolean interpret_string (const String &Value);
         virtual Boolean interpret_start_map ();
         virtual Boolean interpret_map_key (const String &Name);
         virtual Boolean interpret_end_map ();
         virtual Boolean interpret_start_array ();
         virtual Boolean interpret_end_array ();

         virtual String get_error ();

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         InterpreterJSONConfig (const InterpreterJSONConfig &);
         InterpreterJSONConfig &operator= (const InterpreterJSONConfig &);
   };
};

#endif // DMZ_FOUNDATION_INTERPRETER_JSON_CONFIG_DOT_H
