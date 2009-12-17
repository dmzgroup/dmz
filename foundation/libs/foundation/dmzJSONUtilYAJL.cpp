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


static inline Boolean
local_flush (yajl_gen gen, Stream &stream) {

   Boolean result (True);

   const unsigned char *buf (0);
   unsigned int len (0);

   const yajl_gen_status Status = yajl_gen_get_buf (gen, &buf, &len);

   if (result && (Status == yajl_gen_status_ok) && buf && len) {

      stream.write_raw_data (buf, len);
   }

   yajl_gen_clear (gen);

   return result;
}


static inline Boolean
local_is_unique (const Config &Data, const String &Name) {

   Config tmp;

   return Data.lookup_config (Name, tmp) == False;
}


static inline Boolean
local_ok (const yajl_gen_status Status) {

   return Status == yajl_gen_status_ok;
}


static inline Boolean
local_write_string (yajl_gen gen, const String &Str, Log *log) {

   Int32 length (0);
   const unsigned char *Buf = (const unsigned char *)Str.get_buffer (length);
   return local_ok (yajl_gen_string (gen, Buf, (unsigned int)length));
}


static void *TableValue ((void *)1);

static Boolean
local_write_config (yajl_gen gen, Stream &stream, const Config &Data, Log *log) {

   Boolean result (True);

   result - local_ok (yajl_gen_map_open (gen));

   ConfigIterator it;
   String name;
   String value;

   while (Data.get_next_attribute (it, name, value) && result) {

      if (local_is_unique (Data, name)) {

         local_write_string (gen, name, log);

         if (json_is_number (value)) {

            result = local_ok (
               yajl_gen_number (
                  gen,
                  value.get_buffer (),
                  (unsigned int)value.get_length ()));
         }
         else {

            if (value.get_length ()) {

               result = local_ok (yajl_gen_string (
                  gen,
                  (const unsigned char *)value.get_buffer (),
                  (unsigned int)value.get_length ()));
            }
            else { result = local_ok (yajl_gen_null (gen)); }
         }
      }
      else if (log) {

         log->error << "Attribute: " << name << "=" << value
            << " conflicts with Config name. Attribute will be ignored." << endl;
      }
   }

   local_flush (gen, stream);

   HashTableString unique;

   it.reset ();
   Config next;

   while (Data.get_next_config (it, next) && result) {

      const String Name = next.get_name ();

      if (unique.lookup (Name) != TableValue) {

         local_write_string (gen, Name, log);

         unique.store (Name, TableValue);

         Config group;

         Boolean isArray (False);

         if (Data.lookup_all_config (Name, group)) {

            if (group.get_config_count () > 1) { isArray = True; }
            else if (next.is_in_array ()) {

               isArray = True;
               group.add_config (next);
            }
         }

         if (isArray) {

            result = local_ok (yajl_gen_array_open (gen));

            ConfigIterator git;
            Config obj;

            while (group.get_next_config (git, obj) && result) {

               result = local_write_config (gen, stream, obj, log);
            }

            result = local_ok (yajl_gen_array_close (gen));
         }
         else {

            result = local_write_config (gen, stream, next, log);
         }
      }
   }

   result = local_ok (yajl_gen_map_close (gen));

   local_flush (gen, stream);

   return result;
}

}; // namespace


/*!

\brief Writes a config context tree to a stream as JSON.
\ingroup Foundation
\details Defined in dmzJSONUtil.h.
\param[in] Data Config object containing config context to write as JSON.
\param[in] stream Stream to output JSON.
\param[in] Mode Mask specifying file generation mode.
\param[in] log Pointer to Log used for error reporting.
\return Returns dmz::True if data was successfully formatted.
\sa dmz::JSONPrettyPrint\n dmz::JSONStripGlobal

*/
dmz::Boolean
dmz::format_config_to_json (
      const Config &Data,
      Stream &stream,
      const UInt32 Mode,
      Log *log) {


   Boolean result (True);

   yajl_gen_config cg = { (Mode & JSONPrettyPrint ? 1 : 0), 0 };
   yajl_gen gen = yajl_gen_alloc (&cg, 0);

   if (gen) {

      if (Mode & JSONStripGlobal) {

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

         while (Data.get_next_config (it, data)) {

            merged.copy_attributes (data);
            merged.add_children (data);
         }

         result = local_write_config (gen, stream, merged, log);

         if (isArray) { yajl_gen_array_close (gen); }
      }
      else {

         yajl_gen_map_open (gen);
         result = local_write_config (gen, stream, Data, log);
         yajl_gen_map_close (gen);
      }

      local_flush (gen, stream);

      yajl_gen_free (gen);
   }

   return result;
}

