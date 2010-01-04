#include <dmzRuntimeConfig.h>
#include <dmzTypesDeleteListTemplate.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesStringUtil.h>
#include <dmzFoundationInterpreterJSONConfig.h>

using namespace dmz;

namespace {

struct Stack {

   Config data;
   String name;
   Boolean inArray;

   Stack *next;

   Stack (const Config &TheData) :
         data (TheData),
         inArray (False),
         next (0) {;}
};

};


/*!

\class dmz::InterpreterJSONConfig
\ingroup Foundation
\brief Class converts JSON to a config context tree.
\details The class is used by the ParserJSON to process the parsed JSON and
convert it into a config context tree.
\sa dmz::ParserJSON \n dmz::Config \n  dmz::ConfigContext

*/

struct dmz::InterpreterJSONConfig::State {

   String error;
   Stack *stack;

   State (const Config &RootConfig) : stack (0) {

      stack = new Stack (RootConfig);
      if (stack) { stack->name = "dmz"; }
   }

   ~State () {

      while (stack) { Stack *tmp = stack; stack = stack->next; delete tmp; tmp = 0; }
   }
};


/*!

\brief Constructor.
\param[in] RootConfig Config object containing the root of the config context tree.

*/
dmz::InterpreterJSONConfig::InterpreterJSONConfig (const Config &RootConfig) :
      _state (*(new State (RootConfig))) {;}


//! Destructor.
dmz::InterpreterJSONConfig::~InterpreterJSONConfig () { delete &_state; }

dmz::Boolean
dmz::InterpreterJSONConfig::interpret_null () {

   if (_state.stack) {

      if (_state.stack->inArray) {

         Config data (_state.stack->name);
         data.set_in_array (True);
         _state.stack->data.add_config (data);
      }
      else { _state.stack->data.store_attribute (_state.stack->name, ""); }
   }

   return True;
}


dmz::Boolean
dmz::InterpreterJSONConfig::interpret_boolean (const Boolean Value) {

   if (_state.stack) {

      if (_state.stack->inArray) {

         Config data (_state.stack->name);
         data.set_in_array (True);
         data.store_attribute ("value", Value ? "true" : "false");
         _state.stack->data.add_config (data);
      }
      else {

         _state.stack->data.store_attribute (
            _state.stack->name,
            Value ? "true" : "false");
      }
   }

   return True;
}


dmz::Boolean
dmz::InterpreterJSONConfig::interpret_number (const String &Value) {

   if (_state.stack) {

      if (_state.stack->inArray) {

         Config data (_state.stack->name);
         data.set_in_array (True);
         data.store_attribute ("value", Value);
         _state.stack->data.add_config (data);
      }
      else {

         _state.stack->data.store_attribute (_state.stack->name, Value);
      }
   }

   return True;
}


dmz::Boolean
dmz::InterpreterJSONConfig::interpret_string (const String &Value) {

   if (_state.stack) {

      if (_state.stack->inArray) {

         Config data (_state.stack->name);
         data.set_in_array (True);
         data.store_attribute ("value", Value);
         _state.stack->data.add_config (data);
      }
      else {

         _state.stack->data.store_attribute (_state.stack->name, Value);
      }
   }

   return True;
}


dmz::Boolean
dmz::InterpreterJSONConfig::interpret_start_map () {

   if (_state.stack) {

      Config data (_state.stack->name);
      data.set_in_array (_state.stack->inArray);
      Stack *next = new Stack (data);

      if (next) {

         _state.stack->data.add_config (data);
         next->next = _state.stack;
         _state.stack = next;
      }
   }

   return True;
}


dmz::Boolean
dmz::InterpreterJSONConfig::interpret_map_key (const String &Name) {

   if (_state.stack) { _state.stack->name = Name; }

   return True;
}


dmz::Boolean
dmz::InterpreterJSONConfig::interpret_end_map () {

   Stack *tmp = _state.stack;

   if (tmp) { _state.stack = tmp->next; delete tmp; tmp = 0; }

   return True;
}


dmz::Boolean
dmz::InterpreterJSONConfig::interpret_start_array () {

   if (_state.stack) { _state.stack->inArray = True; }

   return True;
}


dmz::Boolean
dmz::InterpreterJSONConfig::interpret_end_array () {

   if (_state.stack) { _state.stack->inArray = False; }

   return True;
}


dmz::String
dmz::InterpreterJSONConfig::get_error () { return _state.error; }
