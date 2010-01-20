#include <dmzFoundationParserJSON.h>
#include <dmzSystemFile.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <qdb.h>

#include <stdio.h>

using namespace dmz;

static qdb out;

namespace {

class interpreter : public dmz::InterpreterJSON {

   public:
      interpreter ();
      ~interpreter () {;}
      virtual Boolean interpret_null ();
      virtual Boolean interpret_boolean (const Boolean Value);
      virtual Boolean interpret_number (const String &Value);
      virtual Boolean interpret_string (const String &Value);
      virtual Boolean interpret_start_map ();
      virtual Boolean interpret_map_key (const String &Name);
      virtual Boolean interpret_end_map ();
      virtual Boolean interpret_start_array ();
      virtual Boolean interpret_end_array ();

      virtual String get_error ();

   protected:
      dmz::Int32 _count;
      dmz::String _error;
};

};

interpreter::interpreter () : _count (0) {

}

Boolean
interpreter::interpret_null () {

   out << "<NULL>" << endl;
   return True;
}


Boolean
interpreter::interpret_boolean (const Boolean Value) {

   out << "Boolean:" << (Value ? "True" : "False") << endl;
   return True;
}


Boolean
interpreter::interpret_number (const String &Value) {

   out << "Number:" << Value << endl;
   return True;
}


Boolean
interpreter::interpret_string (const String &Value) {

   out << "String:\"" << Value << "\"" << endl;
   return True;
}


Boolean
interpreter::interpret_start_map () {

   out << "{" << endl;
   _count += 3;
   return True;
}


Boolean
interpreter::interpret_map_key (const String &Name) {

   String space;
   space.repeat (" ", _count);
   out << space << Name << " : ";
   return True;
}


Boolean
interpreter::interpret_end_map () {

   _count -= 3;
   String space;
   space.repeat (" ", _count);
   out << endl << space << "}" << endl;
   return True;
}


Boolean
interpreter::interpret_start_array () {

   out << "[" << endl;
   _count += 3;

   return True;
}


Boolean
interpreter::interpret_end_array () {

   _count -= 3;
   String space;
   space.repeat (" ", _count);
   out << endl << space << "]" << endl;
   return True;
}


String
interpreter::get_error () {

   return "No Error";
}

int
main (int argc, char *argv[]) {

   if (argc == 2) {

      FILE *fd = dmz::open_file (argv[1], "rb");

      if (fd) {

         dmz::ParserJSON parser;
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
