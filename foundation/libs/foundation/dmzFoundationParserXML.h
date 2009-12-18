#ifndef DMZ_FOUNDATION_PARSER_XML_DOT_H
#define DMZ_FOUNDATION_PARSER_XML_DOT_H

#include <dmzFoundationExport.h>
#include <dmzFoundationParser.h>
#include <dmzTypesString.h>

namespace dmz {

   template<class T> class HashTableStringTemplate;

   class InterpreterXML {

      public:
         virtual ~InterpreterXML () {;}

         virtual Boolean interpret_start_element (
            const String &Name,
            const HashTableStringTemplate<String> &AttributeTable) = 0;

         virtual Boolean interpret_end_element (const String &Name) = 0;

         virtual Boolean interpret_character_data (const String &Data) = 0;

         virtual Boolean interpret_start_cdata_section () = 0;
         virtual Boolean interpret_end_cdata_section () = 0;

         virtual String get_error () = 0;

      protected:
         InterpreterXML () {;}

      private:
         InterpreterXML (const InterpreterXML &);
         InterpreterXML &operator= (const InterpreterXML &);
   };

   class DMZ_FOUNDATION_LINK_SYMBOL ParserXML : public Parser {

      public:
         ParserXML ();
         ~ParserXML ();

         void set_interpreter (InterpreterXML *interpreter);

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
         ParserXML (const ParserXML &Parser);
         ParserXML &operator= (const ParserXML &Parser);
   };
};

#endif // DMZ_FOUNDATION_PARSER_XML_DOT_H
