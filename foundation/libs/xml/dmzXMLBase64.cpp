#include <dmzXMLBase64.h>
#include <dmzSystemStream.h>
#include <dmzSystemStreamString.h>
#include <dmzTypesBase.h>
#include <dmzTypesStringUtil.h>

#include <qdb.h>
static dmz::qdb cout;

using namespace dmz;

namespace {

// Win32 EOL == <CR><LF>
// Unix  EOL == <LF>
// MacOS EOL == <CR>

static const char LocalCR = char (13);
static const char LocalLF = char (10);
static const char LocalEndl[] = {char (13), char (10), '\0'};
static const char ErrorBit = char (0x80);

static const char *get_encode_table () {

   static Boolean init (False);
   static char etable[64];

   if (!init) {

      int ix (0);

      for (ix = 0; ix < 9; ix++) {

         etable[ix] = 'A' + ix;
         etable[ix + 9] = 'J' + ix;
         etable[26 + ix] = 'a' + ix;
         etable[26 + ix + 9] = 'j' + ix;
      }

      for (ix = 0; ix < 8; ix++) {

         etable[ix + 18] = 'S' + ix;
         etable[26 + ix + 18] = 's' + ix;
      }

      for (ix = 0; ix < 10; ix++) { etable[52 + ix] = '0' + ix; }

      etable[62] = '+';
      etable[63] = '/';

      init = True;
   }

   return etable;
}


static const char *get_encode_table_web () {

   static Boolean init (False);
   static char etable[64];

   if (!init) {

      int ix (0);

      for (ix = 0; ix < 9; ix++) {

         etable[ix] = 'A' + ix;
         etable[ix + 9] = 'J' + ix;
         etable[26 + ix] = 'a' + ix;
         etable[26 + ix + 9] = 'j' + ix;
      }

      for (ix = 0; ix < 8; ix++) {

         etable[ix + 18] = 'S' + ix;
         etable[26 + ix + 18] = 's' + ix;
      }

      for (ix = 0; ix < 10; ix++) { etable[52 + ix] = '0' + ix; }

      etable[62] = '-';
      etable[63] = '_';

      init = True;
   }

   return etable;
}


static const char *get_decode_table () {

   static Boolean init (False);
   static char dtable[256];

   if (!init) {

      int ix (0);

      for (ix = 0; ix < 255; ix++) { dtable[ix] = ErrorBit; }
      for (ix = 'A'; ix <= 'I'; ix++) { dtable[ix] = 0 + (ix - 'A'); }
      for (ix = 'J'; ix <= 'R'; ix++) { dtable[ix] = 9 + (ix - 'J'); }
      for (ix = 'S'; ix <= 'Z'; ix++) { dtable[ix] = 18 + (ix - 'S'); }
      for (ix = 'a'; ix <= 'i'; ix++) { dtable[ix] = 26 + (ix - 'a'); }
      for (ix = 'j'; ix <= 'r'; ix++) { dtable[ix] = 35 + (ix - 'j'); }
      for (ix = 's'; ix <= 'z'; ix++) { dtable[ix] = 44 + (ix - 's'); }
      for (ix = '0'; ix <= '9'; ix++) { dtable[ix] = 52 + (ix - '0'); }
      dtable['+'] = 62;
      dtable['/'] = 63;
      dtable['='] = 0;

      init = True;
   }

   return dtable;
}


static const char *get_decode_table_web () {

   static Boolean init (False);
   static char dtable[256];

   if (!init) {

      int ix (0);

      for (ix = 0; ix < 255; ix++) { dtable[ix] = ErrorBit; }
      for (ix = 'A'; ix <= 'I'; ix++) { dtable[ix] = 0 + (ix - 'A'); }
      for (ix = 'J'; ix <= 'R'; ix++) { dtable[ix] = 9 + (ix - 'J'); }
      for (ix = 'S'; ix <= 'Z'; ix++) { dtable[ix] = 18 + (ix - 'S'); }
      for (ix = 'a'; ix <= 'i'; ix++) { dtable[ix] = 26 + (ix - 'a'); }
      for (ix = 'j'; ix <= 'r'; ix++) { dtable[ix] = 35 + (ix - 'j'); }
      for (ix = 's'; ix <= 'z'; ix++) { dtable[ix] = 44 + (ix - 's'); }
      for (ix = '0'; ix <= '9'; ix++) { dtable[ix] = 52 + (ix - '0'); }
      dtable['-'] = 62;
      dtable['_'] = 63;
      dtable['='] = 0;

      init = True;
   }

   return dtable;
}


static inline void
local_encode (const char *etable, const UInt8 In[3], char out[5]) {

   out[0] = etable[In[0] >> 2];
   out[1] = etable[((In[0] & 3) << 4) | (In[1] >> 4)];
   out[2] = etable[((In[1] & 0xF) << 2) | (In[2] >> 6)];
   out[3] = etable[In[2] & 0x3F];
}


static inline void
local_decode (const UInt8 In[4], UInt8 out[4]) {

   out[0] = (In[0] << 2) | (In[1] >> 4);
   out[1] = (In[1] << 4) | (In[2] >> 2);
   out[2] = (In[2] << 6) | In[3];
}

}


/*!

\class dmz::Base64Encoder
\ingroup Foundation
\brief Stream based base64 encoder.
\details Will convert stream to either standard base64 or URL safe base64.
\sa dmz::Base64Decoder

*/

struct dmz::Base64Encoder::State {

   Base64FormatMode mode;
   Int32 maxLineLength;
   Int32 lineLength;
   Int32 place;
   UInt8 in[3];
   Stream *stream;
   const char *etable;

   void reset () { place = 0; lineLength = 0; in[0] = in[1] = in[2] = 0; }

   State (const Base64FormatMode Mode, const Int32 LineLength) :
         mode (Mode),
         maxLineLength (LineLength),
         lineLength (0),
         place (0),
         stream (0),
         etable (Mode == Base64Web ? get_encode_table_web () : get_encode_table ()) {

      reset ();
   }
};


/*!

\brief Base constructor.
\details Defaults to dmz::Base64Standard and a line length of zero.

*/
dmz::Base64Encoder::Base64Encoder () : _state (*(new State (Base64Standard, 0))) {

}


/*!

\brief Constructor.
\param[in] Mode dmz::Base64FormatMode to use for encoding.
\param[in] LineLength Length of line for out put. Must be a multiple of three. If set
to zero, no line feeds are inserted.

*/
dmz::Base64Encoder::Base64Encoder (const Base64FormatMode Mode, const Int32 LineLength) :
      _state (*(new State (Mode, LineLength))) {

}


//! Destructor.
dmz::Base64Encoder::~Base64Encoder () { delete &_state; }


/*!

\brief Sets format mode.
\note Calling this function while in the middle of a encode will change the encode map
and will most likely cause encode errors.
\param[in] Mode dmz::Base64FormatMode to use for encoding.

*/
void
dmz::Base64Encoder::set_format (const Base64FormatMode Mode) {

   _state.mode = Mode;
   _state.etable = (Mode == Base64Web ? get_encode_table_web () : get_encode_table ());
}


//! Gets format mode.
dmz::Base64FormatMode
dmz::Base64Encoder::get_format () { return _state.mode; }


//! Sets line wrap length. Must be a multiple of three. If set to zero, the encoder
//! will not insert line feeds into the output.
void
dmz::Base64Encoder::set_line_length (const Int32 LineLength) {

   _state.maxLineLength = LineLength;
}


//! Gets line wrap length.
dmz::Int32
dmz::Base64Encoder::get_line_length () { return _state.maxLineLength; }


//! Sets output stream.
void
dmz::Base64Encoder::set_stream (Stream *stream) { _state.stream = stream; }


//! Gets output stream pointer.
dmz::Stream *
dmz::Base64Encoder::get_stream () { return _state.stream; }


//! Resets the encoder.
void
dmz::Base64Encoder::reset () { _state.reset (); }


/*!

\brief Encodes buffer.
\details Encoded buffer is written to the output stream.
\note After the source has
been encoded, it is necessary to flush the encoder by calling encode with a NULL
buffer and a Length of zero. If the encoder is not flushed a few bytes of encoded
data may not be written out to the output stream.
\param[in] Buffer data buffer.
\param[in] Length Number of bytes in the data buffer.

*/
void
dmz::Base64Encoder::encode (const char *Buffer, const Int32 Length) {

   if (_state.etable && _state.stream) {

      if (Buffer && Length) {

         char out[5] = {'\0', '\0', '\0', '\0', '\0'};

         for (Int32 ix = 0; ix < Length; ix++) {

            _state.in[_state.place] = UInt8 (Buffer[ix]);

            if (_state.place == 2) {

               local_encode (_state.etable, _state.in, out);
               *(_state.stream) << out;

               if (_state.maxLineLength) {

                  _state.lineLength += 4;

                  if (_state.lineLength >= _state.maxLineLength) {

                     *(_state.stream) << LocalEndl;
                     _state.lineLength = 0;
                  }
               }

               _state.place = 0;
            }
            else { _state.place++; }
         }
      }
      else if (!Length) {

         if (_state.place) {

            char out[5] = {'\0', '\0', '\0', '\0', '\0'};
            local_encode (_state.etable, _state.in, out);
            if (_state.place == 1) { out[2] = out[3] = '='; }
            else if (_state.place == 2) { out[3] = '='; }
            *(_state.stream) << out;
         }

         if (_state.maxLineLength) { *(_state.stream) << LocalEndl; }
      }
   }
}


/*!

\class dmz::Base64Decoder
\ingroup Foundation
\brief Stream based base64 decoder.
\details Will convert either standard base64 or URL safe base64 back to original format.
\sa dmz::Base64Encoder

*/

struct dmz::Base64Decoder::State {

   Base64FormatMode mode;
   int place;
   char cdata[4];
   UInt8 in[4];
   Stream *stream;
   const char *dtable;
   String errorMessage;

   void reset () {

      place = 0;
      in[0] = in[1] = in[2] = in[3] = '\0';
      cdata[0] = cdata[1] = cdata[2] = cdata[3] = '\0';
      errorMessage.flush ();
   }

   State (const Base64FormatMode Mode) :
         mode (Mode),
         place (0),
         stream (0),
         dtable (Mode == Base64Web ? get_decode_table_web () : get_decode_table ()) {

      reset ();
   }
};


/*!

\brief Base constructor.
\details Defaults to dmz::Base64Standard mode.

*/
dmz::Base64Decoder::Base64Decoder () : _state (*(new State (Base64Standard))) {;}


//! Mode constructor.
dmz::Base64Decoder::Base64Decoder (const Base64FormatMode Mode) :
      _state (*(new State (Mode))) {;}


//! Destructor.
dmz::Base64Decoder::~Base64Decoder () { delete &_state; }


/*!

\brief Sets format mode.
\note Calling this function while in the middle of a decode will change the
decode and will most likely cause decode errors.
\param[in] Mode dmz::Base64FormatMode to use for decoding.

*/
void
dmz::Base64Decoder::set_format (const Base64FormatMode Mode) { _state.mode = Mode; }


//! Gets format mode.
dmz::Base64FormatMode
dmz::Base64Decoder::get_format () { return _state.mode; }


//! Sets output stream.
void
dmz::Base64Decoder::set_stream (Stream *stream) { _state.stream = stream; }


//! Gets current output stream.
dmz::Stream *
dmz::Base64Decoder::get_stream () { return _state.stream; }


//! Resets decoder.
void
dmz::Base64Decoder::reset () { _state.reset (); }


/*!

\brief Decodes buffer.
\details Decodes buffer and writes results to the output stream.
\param[in] Buffer data buffer.
\param[in] Length Number of bytes in the data buffer.
\return Returns dmz::True if the buffer was successfully decoded. If the decode fails,
an error message should be available from dmz::Base64Decoder::get_error_message().

*/
dmz::Boolean
dmz::Base64Decoder::decode (const char *Buffer, const Int32 Length) {

   Boolean result (True);

   if (_state.dtable && _state.stream) {

      if (Buffer && Length) {

         UInt8 out[4] = {'\0', '\0', '\0', '\0'};

         for (Int32 ix = 0; (ix < Length) && result; ix++) {

            const char Value (Buffer[ix]);

            if (!is_ascii_white_space (Value)) {

               const UInt8 In (_state.dtable[Value]);

               if ((In & ErrorBit) == 0x00) {

                  _state.cdata[_state.place] = Value;
                  _state.in[_state.place] = In;

                  if (_state.place == 3) {

                     local_decode (_state.in, out);

                     const Int32 Count =
                        (_state.cdata[2] == '=' ? 1 : (_state.cdata[3] == '=' ? 2 : 3));

                     _state.stream->write_raw_data (out, Count);

                     _state.place = 0;
                  }
                  else { _state.place++; }
               }
               else {

                  _state.errorMessage.flush () << "Illegal character: '" << Value
                     << "' [" << Int32 (Value) << "]";

                  result = False;
               }
            }
         }
      }
      else if (!Length && _state.place) {

         _state.errorMessage.flush () << "Undecoded values left in buffer.";

         result = False;
      }
   }

   return result;
}


//! Get last error message.
dmz::String
dmz::Base64Decoder::get_error_message () { return _state.errorMessage; }


/*!

\brief Base64 encodes a String.
\ingroup Foundation
\param[in] Value String containing value to be base64 encoded.
\return Returns String containing the base64 encoded result.

*/
dmz::String
dmz::encode_base64 (const String &Value) {

   String result;
   StreamString stream (result);
   Base64Encoder encoder;
   encoder.set_stream (&stream);
   Int32 size (0);
   const  char *buffer = Value.get_buffer (size);
   encoder.encode (buffer, size);
   encoder.encode (0, 0);

   return result;
}



/*!

\brief Base64 decodes a String.
\ingroup Foundation
\param[in] Value String containing value to be base64 decoded.
\return Returns String containing the base64 decoded result.

*/
dmz::String
dmz::decode_base64 (const String &Value) {

   String result (0, 0, ((Value.get_length () * 3) / 4) + 1);
   StreamString stream (result);
   Base64Decoder decoder;
   decoder.set_stream (&stream);
   Int32 size (0);
   const  char *buffer = Value.get_buffer (size);
   decoder.decode (buffer, size);

   return result;
}

