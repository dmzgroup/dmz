#ifndef DZM_XML_INTERPRETER_CONFIG_DOT_H
#define DZM_XML_INTERPRETER_CONFIG_DOT_H

#include <dmzTypesBase.h>
#include <dmzXMLExport.h>
#include <dmzXMLParser.h>

namespace dmz {

   class Config;

   class DMZ_XML_LINK_SYMBOL XMLInterpreterConfig : public XMLInterpreter {

      public:
         XMLInterpreterConfig (const Config &RootConfig);
         virtual ~XMLInterpreterConfig ();

         virtual Boolean interpret_start_element (
            const String &Name,
            const HashTableStringTemplate<String> &AttributeTable);

         virtual Boolean interpret_end_element (const String &Name);

         virtual Boolean interpret_character_data (const String &Data);

         virtual Boolean interpret_start_cdata_section ();
         virtual Boolean interpret_end_cdata_section ();

         virtual String get_error ();

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         XMLInterpreterConfig (const XMLInterpreter &Interpreter);
         XMLInterpreterConfig &operator= (const XMLInterpreter &Interpreter);
   };
};

#endif // DZM_XML_INTERPRETER_CONFIG_DOT_H
