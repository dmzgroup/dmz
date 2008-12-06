#ifndef DMZ_XML_BASE_64_DOT_H
#define DMZ_XML_BASE_64_DOT_H

#include <dmzTypesString.h>
#include <dmzXMLExport.h>

namespace dmz {

   class Stream;

   //! \brief Base64Encode and Base64Decode mode.
   //! \ingroup Foundation
   enum Base64FormatMode {
      //!< Uses '/' and '+' as the two extra character values.
      Base64Standard,
      //! Uses '-' and '_' as the two extra character values which are URL safe.
      Base64Web,
   };

   class DMZ_XML_LINK_SYMBOL Base64Encoder {

      public:
         Base64Encoder ();
         Base64Encoder (const Base64FormatMode Mode, const Int32 LineLength);
         ~Base64Encoder ();

         void set_format (const Base64FormatMode Mode);
         Base64FormatMode get_format ();

         void set_line_length (const Int32 LineLength);
         Int32 get_line_length ();

         void set_stream (Stream *stream);
         Stream *get_stream ();

         void reset ();
         void encode (const char *Buffer, const Int32 Length);

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         Base64Encoder (const Base64Encoder &);
         Base64Encoder &operator= (const Base64Encoder &);
   };

   class DMZ_XML_LINK_SYMBOL Base64Decoder {

      public:
         Base64Decoder ();
         Base64Decoder (const Base64FormatMode Mode);
         ~Base64Decoder ();

         void set_format (const Base64FormatMode Mode);
         Base64FormatMode get_format ();

         void set_stream (Stream *stream);
         Stream *get_stream ();

         void reset ();
         Boolean decode (const char *Buffer, const Int32 Length);

         String get_error_message ();

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         Base64Decoder (const Base64Decoder &);
         Base64Decoder &operator= (const Base64Decoder &);
   };

   DMZ_XML_LINK_SYMBOL String
   encode_base64 (const String &Value);

   DMZ_XML_LINK_SYMBOL String
   decode_base64 (const String &Value);

   DMZ_XML_LINK_SYMBOL Boolean
   decode_base64 (const String &Value, String &decoded);
};

#endif //  DMZ_XML_BASE_64_DOT_H
