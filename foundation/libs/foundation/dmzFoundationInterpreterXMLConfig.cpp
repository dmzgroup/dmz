#include <dmzFoundationInterpreterXMLConfig.h>
#include <dmzRuntimeConfig.h>
#include <dmzTypesDeleteListTemplate.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesStringUtil.h>

namespace {

   struct ValueStruct {

      const dmz::String Value;
      ValueStruct *next;

      ValueStruct (const dmz::String &TheValue) : Value (TheValue), next (0) {;}
      ~ValueStruct () { dmz::delete_list (next); }
   };

   struct DataStruct {

      dmz::Config data;
      DataStruct *next;

      DataStruct (const dmz::Config &Config) : data (Config), next (0) {;}
      ~DataStruct () { if (next) { delete next; next = 0; } }
   };

   static void local_set_attributes (
         dmz::Config &data,
         const dmz::HashTableStringTemplate<dmz::String> &Attr) {

      dmz::HashTableStringIterator it;

      dmz::String *ptr = Attr.get_first (it);

       while (ptr) {

          data.store_attribute (it.get_hash_key (), *ptr);
          ptr = Attr.get_next (it);
      }
   }
};


/*!

\class dmz::InterpreterXMLConfig
\ingroup Foundation
\brief Class converts XML to a config context tree.
\details The class is used by the ParserXML to process the parsed XML and
convert it into a config context tree.
\sa dmz::ParserXML \n dmz::Config \n  dmz::ConfigContext

*/

struct dmz::InterpreterXMLConfig::State {

   DataStruct *stack;
   Int32 valueSize;
   ValueStruct *valueStart;
   ValueStruct *valueEnd;
   String value;
   Boolean inCDATA;
   String error;
   String name;

   State (const Config &RootConfig) :
         stack (0),
         valueSize (0),
         valueStart (0),
         valueEnd (0),
         inCDATA (False) {

      stack = new DataStruct (RootConfig);
   }

   ~State () {

      if (stack) { delete stack; stack = 0; }
      if (valueStart) { delete valueStart; valueStart = 0; }
   }

   void append_value (const String &Value) {

      valueSize += Value.get_length ();
      ValueStruct *vs = new ValueStruct (Value);
      if (valueEnd) { valueEnd->next = vs; valueEnd = vs; }
      else { valueEnd = valueStart = vs; }
   }
 
   void flush_value () {

      if (stack && valueSize && valueStart) {

         value.set_size (valueSize + 1);

         ValueStruct *current (valueStart);

         while (current) {

            value.append (current->Value);
            current = current->next;
         }

         delete valueStart; valueStart = valueEnd = 0;
         valueSize = 0;

         if (inCDATA || !is_ascii_white_space (value)) {

            stack->data.append_value (value, inCDATA);
         }
      }

      value.flush ();
   }
};


/*!

\brief Constructor.
\param[in] RootConfig Config object containing the root of the config context tree.

*/
dmz::InterpreterXMLConfig::InterpreterXMLConfig (const Config &RootConfig) :
      _state (*(new State (RootConfig))) {;}


//! Destructor.
dmz::InterpreterXMLConfig::~InterpreterXMLConfig () { delete &_state; }


dmz::Boolean
dmz::InterpreterXMLConfig::interpret_start_element (
      const String &Name,
      const HashTableStringTemplate<String> &AttributeTable) {

   Boolean result (True);

   if (_state.stack) {

      _state.flush_value ();

      Config data (Name);

      _state.stack->data.add_config (data);

      DataStruct *next = new DataStruct (data);

      if (next) {

         next->next = _state.stack;
         _state.stack = next;
      }

      local_set_attributes (data, AttributeTable);
   }

   return result;
}


dmz::Boolean
dmz::InterpreterXMLConfig::interpret_end_element (const String &Name) {

   Boolean result (False);

   DataStruct *tmp = _state.stack;

   if (tmp) {

      if (tmp->data.get_name () == Name) {

         _state.flush_value ();
         _state.stack = _state.stack->next;
         tmp->next = 0;
         delete tmp; tmp = 0;

         result = True;
      }
      else {

         _state.error = "Internal error. Closing data tag name miss-match.";
      }
   }
   else {

      _state.error = "Internal error. Closing data tag miss-match.";
   }

   return result;
}


dmz::Boolean
dmz::InterpreterXMLConfig::interpret_character_data (const String &Data) {

   Boolean result (True);

   _state.append_value (Data);

   return result;
}


dmz::Boolean
dmz::InterpreterXMLConfig::interpret_start_cdata_section () {

   _state.flush_value ();
   _state.inCDATA = True;
   return True;
}


dmz::Boolean
dmz::InterpreterXMLConfig::interpret_end_cdata_section () {

   _state.flush_value ();
   _state.inCDATA = False;
   return True;
}


dmz::String
dmz::InterpreterXMLConfig::get_error () { return _state.error; }
