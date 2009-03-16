#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeVersion.h>
#include <dmzSystemFile.h>
#include <dmzSystemStream.h>
#include <dmzTypesStringUtil.h>
#include <dmzXMLInterpreterConfig.h>
#include <dmzXMLParser.h>
#include <dmzXMLUtil.h>

namespace {

static void
local_escape_string (const dmz::String &Value, dmz::String &result) {

   const dmz::Int32 Length (Value.get_length ());
   const dmz::Int32 NewSize (Length * 2);
   if (NewSize < result.get_size ()) { result.set_buffer (0, NewSize); }

   const char *Buffer (Value.get_buffer ());

   if (Buffer && Length) {

      char charArray[2] = { '\0', '\0' };

      for (dmz::Int32 ix = 0; ix < Length; ix++) {

         const char C = Buffer[ix];

         if(C == '<') { result << "&lt;"; }
         else if(C == '>') { result << "&gt;"; }
         else if(C == '&') { result << "&amp;"; }
         else if(C == '"') { result << "&quot;"; }
         else if(C == '\'') { result << "&apos;"; }
         else { charArray[0] = C; result << charArray; }
      }
   }
   else { result << ""; }
}


static void
local_write_attributes (const dmz::Config &Data, dmz::Stream &stream) {

   dmz::ConfigIterator it;
   dmz::String name, value, newValue;

   for (
         dmz::Boolean result = Data.get_first_attribute (it, name, value);
         result;
         result = Data.get_next_attribute (it, name, value)) {

      newValue.set_length (0);
      local_escape_string (value, newValue);

      stream << " " << name << "=\"" << newValue << "\"";
   }
}


static void
local_write_config (
      const dmz::Config &Data,
      const dmz::Int32 Indent,
      dmz::Stream &stream) {

   dmz::String padding;
   padding.repeat (" ", Indent);
   dmz::ConfigIterator it;

   const dmz::String Name (Data.get_name ());

   const dmz::Boolean HasChildren (Data.has_children ());

   if (!Name) {

      dmz::String value;
      Data.get_value (value);

      if (value) {

         if (Data.is_formatted ()) {

            stream << "<![CDATA[" << value << "]]>" << dmz::endl;
         }
         else {

            dmz::trim_ascii_white_space (value);
            stream << padding << value << dmz::endl;
         }
      }
   }
   else {

      stream << padding << "<" << Data.get_name ();

      local_write_attributes (Data, stream);

      if (HasChildren) {

         stream << ">" << dmz::endl;

         dmz::Config nextData;

         for (
               dmz::Boolean result = Data.get_first_config (it, nextData);
               result;
               result = Data.get_next_config (it, nextData)) {

            local_write_config (nextData, Indent + 3, stream);
         }

         stream << padding << "</" << Data.get_name () << ">" << dmz::endl;
      }
      else { stream << "/>" << dmz::endl; }
   }
}

} // namespace


/*!

\ingroup Foundation
\brief Converts an XML file to a config context tree.
\details Defined in dmzXMLUtil.h.
\param[in] File String containing name of XML file to parse.
\param[out] data Config object to store parsed XML data.
\param[in] log Pointer to Log for streaming log messages.
\return Returns dmz::True if the XML file was successfully parsed.
\sa dmz::Config \n dmz::ConfigContext

*/
dmz::Boolean
dmz::xml_to_config (const String &File, Config &data, Log *log) {

   XMLParser parser;
   XMLInterpreterConfig interpreter (data);
   parser.set_interpreter (&interpreter);
   Boolean error (False);

   FILE *file = open_file (File, "rb");

   if (file) {

      String buffer;

      while (read_file (file, 1024, buffer) && !error) {

         const Int32 Length = buffer.get_length ();
         const char *cbuf = buffer.get_buffer ();

         if (!parser.parse_buffer (cbuf, Length, Length < 1024)) {

            error = True;

            if (log) {

               log->error << "In file: " << File << " : " << parser.get_error ()
                  << endl;
            }
         }
      }

      close_file (file);
   }
   else if (log) {

      log->error << "Unable to open file: " << file << endl;
   }

   return !error;
}


/*!

\ingroup Foundation
\brief converts an XML file to a Version

*/
dmz::Boolean
dmz::xml_to_version (const String &File, Version &value, Log *log) {

   Config data ("global");
   Boolean result = xml_to_config (File, data, log);

   if (result) {

      Version tmp (data); 
      value = tmp;
   }

   return result;
}


/*!

\ingroup Foundation
\brief Write a standard XML 1.0 head to a stream.
\details Defined in dmzXMLUtil.h.
\param[in] stream Stream to write XML header.

*/
void
dmz::write_xml_header (Stream &stream) {

   stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
}


/*!

\ingroup Foundation
\brief Writes a config context tree to a stream as XML.
\details Defined in dmzXMLUtil.h.
\param[in] Data Config object containing config context to write as XML.
\param[in] stream Stream to write XML.
\param[in] StripGlobal Strips root of config context tree so it is not included in
the XML.

*/
void
dmz::format_config_to_xml (
      const Config &Data,
      Stream &stream,
      const Boolean StripGlobal) {

   if (StripGlobal) {

      ConfigIterator it;
      Config data;

      Boolean result (Data.get_first_config (it, data));

      while (result) {

         local_write_config (data, 0, stream);
         result = Data.get_next_config (it, data);
      }
   }
   else { local_write_config (Data, 0, stream); }
}

