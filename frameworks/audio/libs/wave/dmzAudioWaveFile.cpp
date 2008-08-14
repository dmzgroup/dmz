#include <dmzAudioWaveFile.h>
#include <dmzSystemFile.h>
#include <dmzSystemUnmarshal.h>

/*!

\class dmz::WaveFile
\ingroup Audio
\brief Loads and parses standard WAV files.

*/

struct dmz::WaveFile::State {

   String fileName;
   String error;
   UInt32 format;
   UInt32 channels;
   UInt32 frequency;
   UInt32 bps;
   UInt32 size;
   char *buffer;

   void clear () {

      fileName.empty ();
      error.empty ();
      format = 0;
      channels = 0;
      frequency = 0;
      bps = 0;
      size = 0;
      if (buffer) { delete []buffer; buffer = 0; }
   }

   State () :
         format (0),
         channels (0),
         frequency (0),
         bps (0),
         size (0),
         buffer (0) {;}

   ~State () { clear (); }
};


//! Default Constructor.
dmz::WaveFile::WaveFile () : _state (*(new State)) {;}


/*!

\brief File Constructor.
\param[in] FileName String containing the name of the wave file to load.

*/
dmz::WaveFile::WaveFile (const String &FileName) : _state (*(new State)) {

   load_file (FileName);
}


//! Destructor
dmz::WaveFile::~WaveFile () { delete &_state; }


/*!

\brief Gets loaded WAV file name.
\return Returns a string containing the name of the loaded WAV file. Returns
and empty string if no file was loaded.

*/
dmz::String 
dmz::WaveFile::get_file_name () const { return _state.fileName; }


/*!

\brief Determines if the WAV file is valid.
\return Returns dmz::True if the WAV file was successfully loaded.

*/
dmz::Boolean
dmz::WaveFile::is_valid () const { return _state.buffer != 0; }


/*!

\brief Clears the class.
\details All memory used to store the loaded WAV file is freed and the class instance
is reset.

*/
void
dmz::WaveFile::clear () { _state.clear (); } 


static const dmz::Int32 LocalHeaderSize = 12;
static const dmz::Int32 LocalFormatSize = 24;

/*!

\brief Loads WAV file.
\param[in] FileName String containing the name of the WAV file to load.
\return Returns dmz::True if the WAV file was successfully loaded.

*/
dmz::Boolean
dmz::WaveFile::load_file (const String &FileName) {

   _state.clear ();

   Boolean error (False);

   FILE *f = open_file (FileName, "rb");

   if (f) {

      _state.fileName = FileName;

      Boolean error (False);

      char header[LocalHeaderSize];
      const Int32 HeaderSize = read_file (f, LocalHeaderSize, header);

      if (HeaderSize == LocalHeaderSize) {

         Unmarshal out (ByteOrderLittleEndian);
         out.set_buffer (HeaderSize, header);

         String chunkID;
         out.get_next_fixed_string (4, chunkID);
         const UInt32 ChunkSize (out.get_next_uint32 ());
         String format;
         out.get_next_fixed_string (4, format);

         if (chunkID != "RIFF") {

            _state.error.flush () << "Unsupported file type: " << chunkID;
            error = true;
         }
         else if (format != "WAVE") {

            _state.error.flush () << "Unsupported format: '" << format << "'";
            error = true;
         }
      }
      else {

         _state.error.flush () << "Unable to read wave file header";
         error = True;
      }

      if (!error) {

         char format[LocalFormatSize];

         const Int32 FormatSize = read_file (f, LocalFormatSize, format);

         if (FormatSize == LocalFormatSize) {

            Unmarshal out (ByteOrderLittleEndian);
            out.set_buffer (FormatSize, format);

            String fmtID;
            out.get_next_fixed_string (4, fmtID);
            const Int32 ChunkSize (out.get_next_int32 ());
            _state.format = out.get_next_uint16 ();
            _state.channels = out.get_next_uint16 ();
            _state.frequency = out.get_next_uint32 ();
            const UInt32 ByteRate (out.get_next_uint32 ());
            const UInt16 BlockAlign (out.get_next_uint16 ());
            _state.bps = out.get_next_uint16 ();

            if (fmtID != "fmt ") {

               _state.error.flush () << "Unknown sub chunk id: " << fmtID;
               error = True;
            }

            // Subtract of the standard part of the fmt header.
            const Int32 Remainder (ChunkSize - 16);

            if (!error && (Remainder > 0)) {

               char buf = '\0';
               Int32 count (0);
               for (Int32 ix = 0; ix < Remainder; ix++) {
 
                  count += read_file (f, 1, &buf);
               }

               if (count != Remainder) {

                  _state.error.flush () << "Inconsistent extra data in fmt data header.";
                  error = True;
               }
            }
         }
         else {

            _state.error.flush () << "Unable to read wave file format header";
            error = True;
         }
      }

      // Find the data!
      if (!error) {

         Boolean done (False);

         while (!done && !error) {

            char subchunk [8];
            const Int32 SubChunkSize = read_file (f, 8, subchunk);

            if (SubChunkSize == 8) {

               Unmarshal out (ByteOrderLittleEndian);
               out.set_buffer (SubChunkSize, subchunk);

               String dataID;
               out.get_next_fixed_string (4, dataID);
               const Int32 DataSize (out.get_next_int32 ());

               if (dataID == "data") {

                  done = True;

                  if (DataSize > 0) {

                     _state.buffer = new char[DataSize];

                     if (_state.buffer) {

                        _state.size = read_file (f, DataSize, _state.buffer);

                        if (_state.size != DataSize) {

                           UInt32 tmp (_state.size);
                           _state.clear ();
                           _state.error.flush () << "Read data size does not match "
                              << "data size specified in the sub chunk header. " << tmp
                              << " " << DataSize;
                           error = True;
                        }
                     }
                     else {

                        _state.error.flush () << "Unable to allocate buffer of size: "
                           << DataSize;
                        error = True;
                     }
                  }
                  else {

                     _state.error.flush () << "Data size is less than zero.";
                     error = True;
                  }
               }
               else {

                  // Skip over unknown sub chunk

                  char buf = '\0';
                  Int32 count (0);
                  for (Int32 ix = 0; ix < DataSize; ix++) {
 
                     count += read_file (f, 1, &buf);
                  }

                  if (DataSize != count) {

                     _state.error.flush () << "Inconsistent sub chunk size of type: "
                        << dataID;
                     error = True;
                  }
               }
            }
            else {

               _state.error.flush () << "Failed Reading subchunk header while looking "
                  << " for data subchunk.";
               error = True;
            }
         }
      }

      close_file (f);
   }
   else {

      _state.error.flush () << "Unable to open wav file: " << FileName;
      error = True;
   }

   return error != True;
}


//! Returns the audio format enumeration as specified in the WAV standard.
dmz::UInt32
dmz::WaveFile::get_audio_format () const { return _state.format; }


//! Returns the number audio channels contained in the loaded WAV file.
dmz::UInt32
dmz::WaveFile::get_channel_count () const { return _state.channels; }


//! Returns the frequency of the loaded WAV file.
dmz::UInt32
dmz::WaveFile::get_frequency () const { return _state.frequency; }


//! Returns the number of bits per sample of the loaded WAV file.
dmz::UInt32
dmz::WaveFile::get_bits_per_sample () const { return _state.bps; }


/*!

\brief Gets the buffer containing the loaded WAV file.
\param[out] size Used to return the size of the buffer.
\return Returns a buffer containing the loaded wave file. Return NULL if the
WAV file was not loaded.

*/
char *
dmz::WaveFile::get_audio_buffer (UInt32 &size) {

   size = _state.size;
   return _state.buffer;
}


//! Returns any error that was encountered while loading and parsing the WAV file
dmz::String
dmz::WaveFile::get_error () const { return _state.error; }


