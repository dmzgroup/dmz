#ifndef DMZ_JSON_INTERPRETER_CONFIG_DOT_H
#define DMZ_JSON_INTERPRETER_CONFIG_DOT_H

#include <dmzTypesBase.h>
#include <dmzJSONExport.h>
#include <dmzJSONParser.h>

namespace dmz {

   class Config;

   class DMZ_JSON_LINK_SYMBOL JSONInterpreterConfig : public JSONInterpreter {

      public:
         JSONInterpreterConfig (const Config &RootConfig);
         virtual ~JSONInterpreterConfig ();

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
         JSONInterpreterConfig (const JSONInterpreter &Interpreter);
         JSONInterpreterConfig &operator= (const JSONInterpreter &Interpreter);
   };
};

#endif // DMZ_JSON_INTERPRETER_CONFIG_DOT_H
