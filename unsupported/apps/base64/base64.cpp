#include <dmzSystemFile.h>
#include <dmzSystemStream.h>
#include <dmzSystemStreamFile.h>
#include <dmzXMLBase64.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   Base64Decoder decoder (Base64Standard);

   FILE *file (open_file (argv[1], "rb"));

   if (file) {

      StreamFileOut out;

      decoder.set_stream (&out);

      String buffer;

      Int32 size (1);

      while (size) {

         size = read_file (file, 64, buffer);

         if (!decoder.decode (buffer.get_buffer (), size)) {

            size = 0;
            StreamFileErr err;
            err << "Error processing file: " << decoder.get_error_message () << endl;
         }
      }

      decoder.set_stream (0);
   }

   return 0;
}
