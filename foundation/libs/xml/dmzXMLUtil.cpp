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

/*!

\var dmz::XMLStripGlobal
\ingroup Foundation
\brief Specifies that the global scope should be stripped when converting a Config tree
to XML.
\details Defined in dmzXMLUtil.h
\sa dmz::format_config_to_xml

*/

/*!

\var dmz::XMLPrettyPrint
\ingroup Foundation
\brief Specifies that the generated XML should be pretty printed.
\details Defined in dmzXMLUtil.h
\sa dmz::format_config_to_xml

*/


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

         if (C == '<') { result << "&lt;"; }
         else if (C == '>') { result << "&gt;"; }
         else if (C == '&') { result << "&amp;"; }
         else if (C == '"') { result << "&quot;"; }
         else if (C == '\'') { result << "&apos;"; }
         else if (C == char (9)) { result << "&#x0009;"; } // tab
         else if (C == char (10)) { result << "&#x000A;"; } // LF
         else if (C == char (13)) { result << "&#x000D;"; } // CR
         else if (C < char (32)) {;} // Ignore invalid characters
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

static inline void
local_endl (const dmz::Int32 Indent, dmz::Stream &stream) {

   if (Indent >= 0) { stream << dmz::endl; }
}

static void
local_write_config (
      const dmz::Config &Data,
      const dmz::Int32 Indent,
      dmz::Stream &stream) {

   dmz::String padding ("");
   if (Indent >= 0) { padding.repeat (" ", Indent); }
   dmz::ConfigIterator it;

   const dmz::String Name (Data.get_name ());

   const dmz::Boolean HasChildren (Data.has_children ());

   if (!Name) {

      dmz::String value;
      Data.get_value (value);

      if (value) {

         if (Data.is_formatted ()) {

            stream << "<![CDATA[" << value << "]]>";
            local_endl (Indent, stream);
         }
         else {

            dmz::trim_ascii_white_space (value);
            stream << padding << value;
            local_endl (Indent, stream);
         }
      }
   }
   else {

      stream << padding << "<" << Data.get_name ();

      local_write_attributes (Data, stream);

      if (HasChildren) {

         stream << ">";
         local_endl (Indent, stream);

         dmz::Config nextData;

         for (
               dmz::Boolean result = Data.get_first_config (it, nextData);
               result;
               result = Data.get_next_config (it, nextData)) {

            local_write_config (nextData, (Indent >= 0) ? Indent + 3 : Indent, stream);
         }

         stream << padding << "</" << Data.get_name () << ">";
         local_endl (Indent, stream);
      }
      else { stream << "/>"; local_endl (Indent, stream); }
   }
}

} // namespace


/*!

\ingroup Foundation
\brief Converts an XML String to a config context tree.
\details Defined in dmzXMLUtil.h.
\param[in] Value String containing the XML  to parse.
\param[out] data Config object to store parsed XML data.
\param[in] log Pointer to Log for streaming log messages.
\return Returns dmz::True if the XML was successfully parsed.
\sa dmz::Config \n dmz::ConfigContext

*/
dmz::Boolean
dmz::xml_string_to_config (const String &Value, Config &data, Log *log) {

   XMLParser parser;
   XMLInterpreterConfig interpreter (data);
   parser.set_interpreter (&interpreter);
   Boolean error (False);

   if (!parser.parse_buffer (Value.get_buffer (), Value.get_length (), True)) {

      error = True;

      if (log) {

         log->error << "Error parsing: " << endl << "\t"
            << Value << endl << "\t" << parser.get_error () << endl;
      }
   }

   return !error;
}


/*!

\ingroup Foundation
\brief Converts an XML file to a config context tree.
\details Defined in dmzXMLUtil.h.
\param[in] FileName String containing name of XML file to parse.
\param[out] data Config object to store parsed XML data.
\param[in] log Pointer to Log for streaming log messages.
\return Returns dmz::True if the XML file was successfully parsed.
\sa dmz::Config \n dmz::ConfigContext

*/
dmz::Boolean
dmz::xml_to_config (const String &FileName, Config &data, Log *log) {

   XMLParser parser;
   XMLInterpreterConfig interpreter (data);
   parser.set_interpreter (&interpreter);
   Boolean error (False);

   FILE *file = open_file (FileName, "rb");

   if (file) {

      String buffer;

      while (read_file (file, 1024, buffer) && !error) {

         const Int32 Length = buffer.get_length ();
         const char *cbuf = buffer.get_buffer ();

         if (!parser.parse_buffer (cbuf, Length, Length < 1024)) {

            error = True;

            if (log) {

               log->error << "In file: " << FileName << " : " << parser.get_error ()
                  << endl;
            }
         }
      }

      close_file (file);
   }
   else if (log) {

      log->error << "Unable to open file: " << FileName << endl;
   }

   return !error;
}


/*!

\ingroup Foundation
\brief converts an XML file to a Version

*/
dmz::Boolean
dmz::xml_to_version (const String &FileName, Version &value, Log *log) {

   Config data ("global");
   Boolean result = xml_to_config (FileName, data, log);

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
\param[in] Mode Mask specifying file generation mode.
\param[in] log Pointer to Log used for error reporting.
\sa dmz::XMLPrettyPrint\n dmz::XMLStripGlobal

*/
void
dmz::format_config_to_xml (
      const Config &Data,
      Stream &stream,
      const UInt32 Mode,
      Log *log) {

   if (Mode & XMLStripGlobal) {

      ConfigIterator it;
      Config data;

      Boolean result (Data.get_first_config (it, data));

      while (result) {

         local_write_config (data, (XMLPrettyPrint & Mode) ? 0 : -1, stream);
         result = Data.get_next_config (it, data);
      }
   }
   else { local_write_config (Data, (XMLPrettyPrint & Mode) ? 0 : -1, stream); }
}

