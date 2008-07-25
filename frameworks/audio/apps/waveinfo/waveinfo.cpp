#include <dmzAudioWaveFile.h>
#include <dmzSystemStreamFile.h>

using namespace dmz;

static StreamFileOut out;

int
main (int argc, char *argv[]) {

   if (argc >= 2) {

      for (int ix = 1; ix < argc; ix++) {

         WaveFile wav (argv[ix]);

         if (wav.is_valid ()) {

            UInt32 size (0);
            wav.get_audio_buffer (size);

            out
               << "Opened wave file: " << argv[ix] << endl
               << "            size: " << size << endl
               << "          format: " << wav.get_audio_format () << endl
               << "        channels: " << wav.get_channel_count () << endl
               << "       frequency: " << wav.get_frequency () << endl
               << "             bps: " << wav.get_bits_per_sample () << endl;
           
         }
         else {

            out << "Unable to open wave file: " << argv[ix] << endl;
            out << "error: " << wav.get_error () << endl;
         }

         out << endl;
      }
   }

   return 0;
}
