#ifndef DMZ_XML_PARSER_DOT_H
#define DMZ_XML_PARSER_DOT_H

#include <dmzTypesString.h>
#include <dmzXMLExport.h>

namespace dmz {

   template<class T> class HashTableStringTemplate;

   class XMLInterpreter {

      public:
         virtual ~XMLInterpreter () {;}

         virtual Boolean interpret_start_element (
            const String &Name,
            const HashTableStringTemplate<String> &AttributeTable) = 0;

         virtual Boolean interpret_end_element (const String &Name) = 0;

         virtual Boolean interpret_character_data (const String &Data) = 0;

         virtual Boolean interpret_start_cdata_section () = 0;
         virtual Boolean interpret_end_cdata_section () = 0;

         virtual String get_error () = 0;

      protected:
         XMLInterpreter () {;}

      private:
         XMLInterpreter (const XMLInterpreter &Interpreter);
         XMLInterpreter &operator= (const XMLInterpreter &Interpreter);
   };

   class DMZ_XML_LINK_SYMBOL XMLParser {

      public:
         XMLParser ();
         ~XMLParser ();

         void set_interpreter (XMLInterpreter *interpreter);

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
         XMLParser (const XMLParser &Parser);
         XMLParser &operator= (const XMLParser &Parser);
   };
};

#endif // DMZ_XML_PARSER_DOT_H
