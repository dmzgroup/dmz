#include <dmzFoundationSHA.h>
#include <qdb.h>

using namespace dmz;

static qdb out;

int
main (int argc, char *argv[]) {

   SHA sha;

   for (int ix = 1; ix < argc; ix++) {

      out << argv[ix] << " = " << sha_from_file (argv[ix]) << endl;
   }

   return 0;
}
