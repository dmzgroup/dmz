#include <dmzFoundationReaderWriterZip.h>
#include <qdb.h>

using namespace dmz;

static qdb out;

int
main (int argc, char *argv[]) {

   for (int ix = 1; ix < argc; ix++) {

      if (is_zip_file (argv[ix])) { out << argv[ix] << " is a zip." << endl; }
      else { out << argv[ix] << " is NOT a zip." << endl; }
   }

   return 0;
}
