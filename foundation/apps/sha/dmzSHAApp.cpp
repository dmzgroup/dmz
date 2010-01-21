#include <dmzFoundationSHA.h>
#include <dmzFoundationReaderWriterFile.h>
#include <dmzSystemFile.h>
#include <qdb.h>

using namespace dmz;

static qdb out;

int
main (int argc, char *argv[]) {

   SHA sha;

   for (int ix = 1; ix < argc; ix++) {

      ReaderFile read;

      if (read.open_file (argv[ix])) {

         sha.init (SHA1);

         char buffer[64];
         Int32 size = read.read_file (buffer, 64);

         while (size > 0) { 

            sha.add_data (buffer, size);
            size = read.read_file (buffer, 64);
         }

         out << argv[ix] << " = " << sha.finish () << endl;
      }
   }

   return 0;
}
