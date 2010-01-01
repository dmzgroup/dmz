#include <dmzFoundationParserXML.h>
#include <dmzTypesHashTableStringTemplate.h>

#include <stdio.h>

class interpreter : public dmz::InterpreterXML {

   public:
      interpreter ();
      ~interpreter () {;}

      dmz::Boolean interpret_start_element (
         const dmz::String &Name,
         const dmz::HashTableStringTemplate<dmz::String> &AttributeTable);

      dmz::Boolean interpret_end_element (const dmz::String &Name);

      dmz::Boolean interpret_character_data (const dmz::String &Data);

      dmz::Boolean interpret_start_cdata_section () { return dmz::True; }
      dmz::Boolean interpret_end_cdata_section () { return dmz::True; }

      dmz::String get_error ();

   protected:
      dmz::Int32 _count;
      dmz::String _error;
};

interpreter::interpreter () : _count (0) {

}


dmz::Boolean
interpreter::interpret_start_element (
      const dmz::String &Name,
      const dmz::HashTableStringTemplate<dmz::String> &AT) {

   dmz::String out;
   out.repeat (" ", _count);
   out << Name << ":";
   fprintf (stderr, "%s\n", out.get_buffer ());

   dmz::HashTableStringIterator it;

   for (dmz::String *ptr = AT.get_first (it); ptr; ptr = AT.get_next (it)) {

      out.flush ().repeat (" ", _count + 6) << it.get_hash_key () << " = " << *ptr;
      fprintf (stderr, "%s\n", out.get_buffer ());
   }

   _count += 3;

   return dmz::True;
}


dmz::Boolean
interpreter::interpret_end_element (const dmz::String &Name) {

   _count -= 3;

   dmz::String out;
   out.repeat (" ", _count);
   out << "\\" << Name;
   fprintf (stderr, "%s\n", out.get_buffer ());

   return dmz::True;
}


dmz::Boolean
interpreter::interpret_character_data (const dmz::String &Data) {

   dmz::String space;
   space.repeat (" ", _count + 6);
   fprintf (stderr, "%sCDATA = \"%s\"\n", space.get_buffer (), Data.get_buffer ());

   return dmz::True;
}


dmz::String
interpreter::get_error () { return "No Error"; }


int
main (int argc, char *argv[]) {

   if (argc == 2) {

      FILE *fd = fopen (argv[1], "rb");

      if (fd) {

         dmz::ParserXML parser;
         interpreter inter;
         parser.set_interpreter (&inter);

         char buffer[1056];
         dmz::Int32 size (0);

         do {

            size = fread (buffer, sizeof (char), 1056, fd);

            if (size && !parser.parse_buffer (buffer, size, size < 1056)) {

               size = 0;

               fprintf (stderr, "Error: %s\n", parser.get_error ().get_buffer ());
            }

         } while (size);

         fclose (fd);
      }
   }

   return 0;
}
