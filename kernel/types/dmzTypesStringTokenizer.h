#ifndef DMZ_TYPES_STRING_TOKENIZER_DOT_H
#define DMZ_TYPES_STRING_TOKENIZER_DOT_H

#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

/*!

\brief Simple String tokenizer that uses a single character as a delimiter.
\ingroup Types

*/
   class StringTokenizer {

      public:
         StringTokenizer (const dmz::String &Value, const char DelimiterValue = ' ');
         ~StringTokenizer () {;}
         void reset ();
         dmz::String get_next ();

      protected:
         const char _DelimiterValue; //!< Delimiter 8-bit character value.
         const String _Value; //!< String to tokenize.
         const Int32 _Length; //!< Length of String.
         const char *_Buffer; //!< Pointer to String buffer.
         Int32 _place; //!< Current place in buffer being tokenized.
   };
};


/*!

\brief Constructor.
\param[in] Value String to tokenize.
\param[in] DelimiterValue 8 bit character to use as the delimiter.

*/
inline
dmz::StringTokenizer::StringTokenizer (const String &Value, const char DelimiterValue) :
      _DelimiterValue (DelimiterValue),
      _Value (Value),
      _Length (Value.get_length ()),
      _Buffer (Value.get_buffer ()),
      _place (0) {;}


//! Sets the tokenizer back to the start of the string.
inline void
dmz::StringTokenizer::reset () { _place = 0; }


//! Returns next token
inline dmz::String
dmz::StringTokenizer::get_next () {

   dmz::String result;
   dmz::Int32 prev (_place);
   dmz::Boolean done (!_Buffer ? True : False);

   while (!done) {

      if (_place >= _Length) {

         result = _Value.get_sub (prev);
         done = True;
      }
      else if (_Buffer[_place] == _DelimiterValue) {

         if (_place == prev) { _place++; prev = _place; }
         else {

            result = _Value.get_sub (prev, _place - 1);
            _place++;
            done = True;
         }
      }
      else { _place++; }
   }

   return result;
}

#endif // DMZ_TYPES_STRING_TOKENIZER_DOT_H

