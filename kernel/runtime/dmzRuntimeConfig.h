#ifndef DMZ_RUNTIME_CONFIG_DOT_H
#define DMZ_RUNTIME_CONFIG_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesBooleanOperator.h>
#include <dmzTypesString.h>

namespace dmz {

   class ConfigAttributesContext;
   class ConfigContext;
   class Stream;

   class DMZ_KERNEL_LINK_SYMBOL ConfigIterator {

      public:
         ConfigIterator ();
         ~ConfigIterator ();

         struct State;
         State &state; //!< Internal state.

      private:
         ConfigIterator (const ConfigIterator &);
         ConfigIterator &operator= (const ConfigIterator &);
   };

   class DMZ_KERNEL_LINK_SYMBOL Config {

      public:
         Config ();
         explicit Config (const String &Name);
         Config (const Config &Data);
         Config (ConfigContext *context);
         ~Config ();

         Config &operator= (const Config &Data);
         Boolean operator== (const Config &Data) const;
         Boolean operator!= (const Config &Data) const;
         Boolean operator! () const;
         DMZ_BOOLEAN_OPERATOR;

         String get_name () const;
         Boolean is_formatted () const;
         Boolean has_children () const;
         Boolean is_empty () const;

         Boolean get_first_attribute (
            ConfigIterator &it,
            String &name,
            String &value) const;

         Boolean get_next_attribute (
            ConfigIterator &it,
            String &name,
            String &value) const;

         Boolean store_attribute (const String &Name, const String &Value);
         Boolean lookup_attribute (const String &Name, String &value) const;

         Boolean set_value (const String &Value);
         Boolean append_value (const String &Value, const Boolean IsFormatted);
         Boolean get_value (String &value) const;

         Boolean get_first_config (ConfigIterator &it, Config &data) const;
         Boolean get_last_config (ConfigIterator &it, Config &data) const;
         Boolean get_next_config (ConfigIterator &it, Config &data) const;
         Boolean get_prev_config (ConfigIterator &it, Config &data) const;

         Boolean overwrite_config (const String &Scope, const Config &Data);

         Boolean overwrite_config (const Config &Data) {

            const String Null;
            return overwrite_config (Null, Data);
         }

         Boolean add_config (const String &Scope, const Config &Data);

         Boolean add_config (const Config &Data) {

            const String Null;
            return add_config (Null, Data);
         }

         Boolean add_children (const Config &Data);

         Boolean lookup_config (const String &Name, Config &data) const;
         Boolean lookup_all_config (const String &Name, Config &data) const;
         Boolean lookup_all_config_merged (const String &Name, Config &data) const;

         // For internal use
         void set_config_context (ConfigContext *context);
         ConfigContext *get_config_context () const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };

   DMZ_KERNEL_LINK_SYMBOL Boolean pop_last_config_scope_element (
         const String Name,
         String &value,
         String &remainder);
};

   
DMZ_KERNEL_LINK_SYMBOL dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::Config &Data);

#endif // DMZ_RUNTIME_CONFIG_DOT_H
