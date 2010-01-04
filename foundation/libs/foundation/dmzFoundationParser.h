#ifndef DMZ_FOUNDATION_PARSER_DOT_H
#define DMZ_FOUNDATION_PARSER_DOT_H

#include <dmzFoundationExport.h>
#include <dmzTypesString.h>

namespace dmz {

class Parser {

   public:
      virtual ~Parser () {;}

      virtual void reset () = 0;

      virtual Boolean parse_buffer (
         const char *buffer,
         const Int32 Length,
         const Boolean EndOfStream) = 0;

      virtual String get_error () = 0;

   protected:
      Parser () {;}

   private:
      Parser (const Parser &Parser);
      Parser &operator= (const Parser &Parser);
};

};

#endif // DMZ_FOUNDATION_PARSER_DOT_H
