#ifndef DMZ_FOUNDATION_INTERPRETER_XML_CONFIG_DOT_H
#define DMZ_FOUNDATION_INTERPRETER_XML_CONFIG_DOT_H

#include <dmzFoundationExport.h>
#include <dmzTypesBase.h>
#include <dmzFoundationParserXML.h>

namespace dmz {

   class Config;

   class DMZ_FOUNDATION_LINK_SYMBOL InterpreterXMLConfig : public InterpreterXML {

      public:
         InterpreterXMLConfig (const Config &RootConfig);
         virtual ~InterpreterXMLConfig ();

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
         InterpreterXMLConfig (const InterpreterXMLConfig &);
         InterpreterXMLConfig &operator= (const InterpreterXMLConfig &);
   };
};

#endif // DMZ_FOUNDATION_INTERPRETER_XML_CONFIG_DOT_H
