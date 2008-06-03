#include <dmzTypesString.h>
#include <dmzTypesStringUtil.h>

/*!

\file dmzTypesStringUtil.h
\brief Contains simple ASCII specific String processing functions.

\addtogroup Types
@{

*/

/*!

\brief Returns dmz::True if character is ASCII whitespace (i.e. space, tab, or return)
\details Defined in dmzTypesStringUtil.h.

*/
dmz::Boolean
dmz::is_ascii_white_space (const char Value) {

   return
      (Value == ' ') ||
      (Value == '\t') ||
      (Value == char (10)) ||
      (Value == char (13)) ||
      (Value == char (26));
}


/*!

\brief Returns dmz::True if String is ASCII whitespace (i.e. space, tab, or return)
\details Defined in dmzTypesStringUtil.h.

*/
dmz::Boolean
dmz::is_ascii_white_space (const String &Value) {

   dmz::Boolean result (True);

   Int32 length (-1);
   const char *Ptr (Value.get_buffer (length));
   Int32 place (0);

   if (Ptr) {

      while (place < length) {

         if (!is_ascii_white_space (Ptr[place])) { place = length; result = False; }
         else { place++; }
      }
   }

   return result;
}


/*!

\brief Converts ASCII white space in string to \a Target 8 bit character.
\details Defined in dmzTypesStringUtil.h.

*/
void
dmz::convert_ascii_white_space (
      String &value,
      const char Target) {

   const Int32 Length (value.get_length ());

   for (Int32 ix = 0; ix < Length; ix++) {

      if (is_ascii_white_space (value.get_char (ix))) { value.set_char (ix, Target); }
   }
}


/*!

\brief Trims ASCII white space from front and back of the String.
\details Defined in dmzTypesStringUtil.h.
This function remove white space from the front and back of the string passed in.
For example the string " this is a string " would be converted to "this is a string".
\param[in,out] value String to have white space trimmed.

*/
void
dmz::trim_ascii_white_space (String &value) {

   Boolean done (False);
   Int32 length (value.get_length ());
   Int32 count (length - 1);

   while (!done) {

      if (count < 0) { done = True; }
      else if (!is_ascii_white_space (value.get_char (count))) { done = True; }
      else { count--; }
   }

   if (count < (length - 1)) { value.set_length (count + 1); }

   done = False;
   length = value.get_length ();
   count = 0;

   while (!done) {

      if (count >= length) { done = True; }
      else if (!is_ascii_white_space (value.get_char (count))) { done = True; }
      else { count++; }
   }

   if (count) { value.shift (-count); }
}

//! @}
