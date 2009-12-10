#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeLog.h>
#include <dmzSystemStream.h>
#include <dmzTypesStringUtil.h>
#include <dmzJSONUtil.h>
#include <dmzTypesHashTableString.h>

#include <yajl_gen.h>

using namespace dmz;

namespace {

void
local_escape_string (const dmz::String &Value, dmz::String &result) {

   const dmz::Int32 Length (Value.get_length ());
   const dmz::Int32 NewSize (Length * 2);
   if (NewSize < result.get_size ()) { result.set_buffer (0, NewSize); }

   const char *Buffer (Value.get_buffer ());

   if (Buffer && Length) {

      char charArray[2] = { '\0', '\0' };

      for (dmz::Int32 ix = 0; ix < Length; ix++) {

         const char C = Buffer[ix];

         if (C == '"') { result << "\\\""; }
         else if (C == '\'') { result << "\\'"; }
         else if (C == '\\') { result << "\\\\"; }
         else { charArray[0] = C; result << charArray; }
      }
   }
   else { result << ""; }
}

static Boolean
local_ok (const yajl_gen_status Status) {

   return Status == yajl_gen_status_ok;
}

static Boolean
local_write_string (yajl_gen gen, const String &Str, Log *log) {

   Int32 length (0);
   const unsigned char *Buf = (const unsigned char *)Str.get_buffer (length);
   return local_ok (yajl_gen_string (gen, Buf, (unsigned int)length));
}

static const void *TableValue ((void *)1);

static Boolean
local_write_config (yajl_gen gen, const Config &Data, Log *log) {

   Boolean result (True);

   yajl_gen_map_open (gen);

   ConfigIterator it;
   String name;
   String value;

   while (Data.get_next_attribute (it, name, value)) {

      Config tmp;

      if (!Data.lookup_config (name, tmp)) {

      }
      else if (log) {

         log->error << "" << endl;
      }
   }

   yajl_gen_map_close (gen);

   return result;
}

}; // namespace


/*!

\ingroup Foundation
\brief Writes a config context tree to a stream as JSON.
\details Defined in dmzJSONUtil.h.
\param[in] Data Config object containing config context to write as JSON.
\param[in] stream Stream to write JSON.
\param[in] StripGlobal Strips root of config context tree so it is not included in
the JSON.

*/
dmz::Boolean
dmz::format_config_to_json (
      const Config &Data,
      Stream &stream,
      Log *log,
      const Boolean StripGlobal) {

   Boolean result (True);

   yajl_gen_config cg = { 1, 0 };
   yajl_gen gen = yajl_gen_alloc (&cg, 0);

   if (gen) {

      if (StripGlobal) {

         Boolean isArray (False);

         ConfigIterator it;
         Config data;
         Config merged;

         if (Data.get_first_config (it, data)) {

            isArray = data.is_in_array ();
            Config tmp (data.get_name ());
            merged = tmp;
         }

         it.reset ();

         if (isArray) { yajl_gen_array_open (gen); }
         else { yajl_gen_map_open (gen); }

         while (Data.get_next_config (it, data)) {

            merged.copy_attributes (data);
            merged.add_children (data);
         }

         if (isArray == False) { local_write_string (gen, merged.get_name (), log); }

         result = local_write_config (gen, merged, log);

         if (isArray) { yajl_gen_array_close (gen); }
         else { yajl_gen_map_close (gen); }
      }
      else {

         yajl_gen_map_open (gen);
         result = local_write_config (gen, Data, log);
         yajl_gen_map_close (gen);
      }

      const unsigned char *buf (0);
      unsigned int len (0);

      const yajl_gen_status Status = yajl_gen_get_buf (gen, &buf, &len);
      yajl_gen_clear (gen);

      if (result && (Status == yajl_gen_status_ok) && buf && len) {

         stream.write_raw_data (buf, len);
      }

      yajl_gen_free (gen);
   }

   return result;
}

