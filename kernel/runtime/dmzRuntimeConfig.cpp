#include <dmzRuntimeConfig.h>
#include "dmzRuntimeConfigContext.h"
#include <dmzSystemStream.h>
#include <dmzTypesStringTokenizer.h>

#include <stdio.h>

/*!

\file dmzRuntimeConfig.h
\brief Contains Config class and utility functions.

*/

static const char LocalScopeChar ('.');

static dmz::ConfigContext *
local_get_config_from_scope (
      const dmz::String &Scope,
      dmz::ConfigContext *context,
      dmz::Boolean CreateScope) {

   dmz::ConfigContext *result (context);

   dmz::StringTokenizer it (Scope, LocalScopeChar);

   if (result) {

      dmz::String sub = it.get_next ();
      dmz::Boolean done (sub ? dmz::False : dmz::True);

      while (!done) {

         dmz::ConfigContext::DataList *next = result->configTable.lookup (sub);

         if (next && next->tail) {

            next->lock.lock ();
               result = next->tail->context;
            next->lock.unlock ();
         }
         else if (CreateScope) {

            dmz::ConfigContext *newCt (new dmz::ConfigContext (sub));
            if (result->add_config (newCt)) { result = newCt; }
            else { result = 0; done = dmz::True; }
         }
         else { done = dmz::True; result = 0; }

         sub = it.get_next ();

         if (!sub) { done = dmz::True; }
      }
   }

   return result;
}


/*!

\ingroup Runtime
\brief Remove last element from a scoped Config name.
\details If the String "dmz.type.value" was passed in as \a Name. The string "value"
would be returned in \a value and the string "dmz.type" would be returned in \a remainder.
\param[in] Name String containing scoped config context name.
\param[out] value String for storing the last element of the config context name.
\param[out] remainder String for storing the remaining part of the config context name.
\return Returns dmz::True if the \a Name was successfully separated.

*/
dmz::Boolean
dmz::pop_last_config_scope_element (
      const String Name,
      String &value,
      String &remainder) {

   Boolean result (False);

   const Int32 Length (Name.get_length ());
   Boolean done (False);
   Int32 place = Length - 1;

   while (!done) {

      if (place < 0) { done = True; }
      else if (Name.get_char (place) == LocalScopeChar) {

         done = True;
      }
      else { place--; }
   }

   if (place >= 1) { value = Name.get_sub (0, place - 1); }

   if (place >= 0) { remainder = Name.get_sub (place + 1); result = True; }

   return result;
}


/*!

\class dmz::ConfigIterator
\ingroup Runtime
\brief Class used in Config iteration functions.
\details The ConfigIterator is used to iterate over attributes and sub Config object
stored in a Config object.
\sa dmz::Config::get_first_attribute \n
dmz::Config::get_next_attribute \n
dmz::Config::get_first_config \n
dmz::Config::get_next_config \n
dmz::Config::get_prev_config \n
dmz::Config::get_last_config

*/

struct dmz::ConfigIterator::State {

   HashTableStringIterator it;
};


//! Constructor.
dmz::ConfigIterator::ConfigIterator () : state (*(new State)) {;}


//! Destructor.
dmz::ConfigIterator::~ConfigIterator () { delete &state; }


//! Reset the iterator
void
dmz::ConfigIterator::reset () { state.it.reset (); }


struct dmz::Config::State {

   ConfigContext *context;
   State () : context (0) {;}
   State (ConfigContext *theContext) : context (theContext) {

      if (context) { context->ref (); }
   }
   ~State () { if (context) { context->unref (); context = 0; } }
};


/*!

\class dmz::Config
\ingroup Runtime
\brief Class containing hierarchical configuration data.
\details The Config class provides an interface for querying and manipulating
configuration data. Configuration data is stored in a hierarchical tree composed of
config contexts. Each config context in the tree may contain
attributes which are name-value pairs and a list of child config contexts.
Each config context has a name.
The name does not need to be unique. It is common for a config context to contain
a list of child config contexts that all have the same name.
It is also possible for a child config context
to have multiple parents. Config contexts are reference counted.
Allocation and deletion of config contexts should not be a concern for the user.
\note The system does not currently check for circular paths in the config context tree.
If one does occur, the config contexts contained in the circular path will never
be deleted.
\sa dmz::ConfigContext

*/


/*!

\brief Default constructor.
\note Does not create a config context.

*/
dmz::Config::Config () : _state (*(new State)) {;}


/*!

\brief Named constructor.
\details This constructor creates a config context with the name specified by the \a Name
parameter. The created config context has no initial parents, children, or attributes.
\note The name of config context should not contain the '.' (period) character.
\param[in] Name String containing the name of config context to create.

*/
dmz::Config::Config (const String &Name) : _state (*(new State)) {

   set_config_context (new ConfigContext (Name));
   // The set_config_context function increases the ref count by one.
   // Since a new ConfigContext starts with a count of one, after the set
   // function the ref count is two. The context needs to be unrefed once to
   // give it the correct count.
   if (_state.context) { _state.context->unref (); }
}


/*!

\brief Copy constructor.
\details This constructor gets the config context stored in \a Data and retains it
internally.
\param[in] Data Config to be copied.

*/
dmz::Config::Config (const Config &Data) : _state (*(new State)) {

   *this = Data;
}


/*!

\brief Context constructor.
\details This constructor take a pointer to a config context and store it internally.
\param[in] context Pointer to a config context.

*/
dmz::Config::Config (ConfigContext *context) :
      _state (*(new State (context))) {;}


//! Destructor.
dmz::Config::~Config () { delete &_state; }


//! Assignment operator.
dmz::Config &
dmz::Config::operator= (const Config &Data) {

   set_config_context (Data._state.context);
   return *this;
}


//! Relational "equal to" operator.
dmz::Boolean
dmz::Config::operator== (const Config &Data) const {

   return Data._state.context == _state.context;
}


//! Relational "not equal to" operator.
dmz::Boolean
dmz::Config::operator!= (const Config &Data) const {

   return Data._state.context != _state.context;
}


/*!

\brief Logical not operator.
\return Returns dmz::True if the Config object contains a valid pointer to a config
context.

*/
dmz::Boolean
dmz::Config::operator! () const { return _state.context == 0; }


/*!

\brief Gets name of config context.
\return Returns a String containing the name of the config context. Returns an empty
string if the Config does not contain a config context.

*/
dmz::String
dmz::Config::get_name () const {

   String result;
   if (_state.context) { result = _state.context->Name; }
   return result;
}


/*!

\brief Sets the formatted state of the config context.
\details A config context may contain formatted data. If the data is formatted, it is an
indication to any thing displaying or saving the data that white space and line
feeds should not be removed.
\param[in] IsFormatted Boolean indicating if the config context is formatted.

*/
void
dmz::Config::set_formatted (const Boolean IsFormatted) {

   if (_state.context && !_state.context->Name) {

      _state.context->isFormatted = IsFormatted;
   }
}


/*!

\brief Get the formatted state of the config context.
\details A config context may contain formatted data. If the data is formatted, it is an
indication to any thing displaying or saving the data that white space and line
feeds should not be removed.
\return Returns dmz::True if the config context is formatted.

*/
dmz::Boolean
dmz::Config::is_formatted () const {

   return _state.context ? _state.context->isFormatted : False;
}


//! Returns dmz::True if the config context has any children.
dmz::Boolean
dmz::Config::has_children () const {

   ConfigIterator it;
   Config cd;

   return get_first_config (it, cd);
}


/*!

\brief Determines if the config context is empty.
\return Returns dmz::True if the config context has no attributes and no children.
Also returns dmz::True if the config context is NULL.

*/
dmz::Boolean
dmz::Config::is_empty () const {

   Boolean result (True);

   if (_state.context) {

      if (_state.context->attrTable.get_count ()) { result = False; }
      else { result = !has_children (); }
   }

   return result;
}


/*!

\brief Determines if a Config objects have equal attributes.
\param[in] Data Config used to compare the attributes.
\return Returns dmz::True if \a Data has a equal number of attributes.
And that each of those attributes have the same name and value.
Will return dmz::False if \a Data has a different number of attributes. Or if
any of the attributes have a different name or value.

*/
dmz::Boolean
dmz::Config::are_attributes_equal (const Config &Data) const {

   Boolean result (False);

   if (_state.context && Data._state.context) {

      if (_state.context->attrTable.get_count () ==
          Data._state.context->attrTable.get_count ()) {

         Boolean areEqual (True);

         ConfigIterator it;
         String name, lhs, rhs;

         while (areEqual && get_next_attribute (it, name, lhs)) {

            if (Data.lookup_attribute (name, rhs)) {

               if (lhs != rhs) { areEqual = False; }
            }
            else { areEqual = False; }
         }

         result = areEqual;
      }
   }

   return result;
}


/*!

\brief Get first attribute stored in the config context.
\param[in] it ConfigIterator used to iterate over the attributes.
\param[out] name String to store the name of the first attribute.
\param[out] value String to store the value of the first attribute.
\return Returns dmz::True if the first name and value are stored in \a name and
\a value. Will return dmz::False if the config context contains no attributes.

*/
dmz::Boolean
dmz::Config::get_first_attribute (
      ConfigIterator &it,
      String &name,
      String &value) const {

   Boolean result (False);

   if (_state.context) {

      ConfigAttributeContext *ac = _state.context->attrTable.get_first (it.state.it);

      if (ac) {

         ac->lock.lock ();
            value = ac->value;
         ac->lock.unlock ();

         name = it.state.it.get_hash_key ();

         if (name == "") { result = get_next_attribute (it, name, value); }
         else { result = True; }
      }
   }

   return result;
}


/*!

\brief Get next attribute stored in the config context.
\param[in] it ConfigIterator used to iterate over the attributes.
\param[out] name String to store the name of the next attribute.
\param[out] value String to store the value of the next attribute.
\return Returns dmz::True if the next name and value were stored in \a name and
\a value. Returns dmz::False when there are no more name-value pairs to return.

*/
dmz::Boolean
dmz::Config::get_next_attribute (
      ConfigIterator &it,
      String &name,
      String &value) const {

   Boolean result (False);

   if (_state.context) {

      ConfigAttributeContext *ac = _state.context->attrTable.get_next (it.state.it);

      if (ac) {

         ac->lock.lock ();
            value = ac->value;
         ac->lock.unlock ();

         name = it.state.it.get_hash_key ();

         if (name == "") { result = get_next_attribute (it, name, value); }
         else { result = True; }
      }
   }

   return result;
}


/*!

\brief Store a name-value pair in the config context.
\details If an attribute with \a Name already exists, the value is over-written.
The \a Name parameter may be used to store an attribute in child config contexts.
For example the following code snippet:
\code
dmz::Config dataDMZ;

if (global.lookup_data ("dmz", dataDMZ)) {

   dmz::Config dataType

   if (dataDMZ.lookup_data ("type", dataType)) {

      const dmz::String Value ("This is the tag value");

      dataType.store_attribute ("tag", Value);
   }
}
\endcode
May also be written as:
\code
const dmz::String Value ("This is the tag value");

global.store_attribute ("dmz.type.tag", Value);
\endcode
Any child config context that is named in the scope of the attribute \a Name that does not
exist will be created.  For example, in the above code sample, if the child
config context named "type" does not exist, it will be created and added to the
config context named "dmz".
\param[in] Name String containing the name of the attribute.
\param[in] Value String containing the value of the attribute.
\return Returns dmz::True if the attribute was stored successfully.

*/
dmz::Boolean
dmz::Config::store_attribute (const String &Name, const String &Value) {

   Boolean result (False);

   if (_state.context && Name) {

      String dataName;
      String attrName;

      if (!pop_last_config_scope_element (Name, dataName, attrName)) { attrName = Name; }

      ConfigContext *context (0);

      if (dataName) {

         context = local_get_config_from_scope (dataName, _state.context, True);
      }
      else { context = _state.context; }

      if (context) {

         ConfigAttributeContext *ac (context->attrTable.lookup (attrName));

         if (!ac) {

            ac = new ConfigAttributeContext (Value);
            if (ac && context->attrTable.store (attrName, ac)) { result = True; }
            else if (ac) { delete ac; ac = 0; }
         }
         else {

            result = True;

            ac->lock.lock ();
               ac->value = Value;
            ac->lock.unlock ();
         }
      }
   }

   return result;
}


/*!

\brief Looks an attribute by its name.
\details The \a Name parameter may be used to look up an attribute in child config
contexts.  For example the following code snippet:
\code
dmz::Config dataDMZ;

if (global.lookup_data ("dmz", dataDMZ)) {

   dmz::Config dataType

   if (dataDMZ.lookup_data ("type", dataType)) {

      dmz::String value;

      if (dataType.lookup_attribute ("tag", value)) {

         // Do something with value.
      }
   }
}
\endcode
May also be written as:
\code
dmz::String value;

if (global.lookup_attribute ("dmz.type.tag", value)) {

   // Do something with value.
}
\endcode
\param[in] Name String containing name of the attribute to retrieve.
\param[out] value String used to store the value of the attribute.
\return Returns dmz::True if the attribute if found.

*/
dmz::Boolean
dmz::Config::lookup_attribute (const String &Name, String &value) const {

   Boolean result (False);

   if (_state.context) {

      String dataName;
      String attrName;

      if (!pop_last_config_scope_element (Name, dataName, attrName)) { attrName = Name; }

      Config cd;

      if (dataName) { lookup_config (dataName, cd); }
      else { cd = *this; }

      if (cd) {

         ConfigAttributeContext *ac = cd._state.context->attrTable.lookup (attrName);

         if (ac) {

            ac->lock.lock ();
               value = ac->value;
            ac->lock.unlock ();

            result = True;
         }
      }
   }

   return result;
}


/*!

\brief Stores a value in the config context.
\details Each config context may have a single value stored in it. This value is
essentially an unnamed attribute.
\param[in] Value String containing value to be stored in the config context.
\return Returns dmz::True if the value was stored.

*/
dmz::Boolean
dmz::Config::set_value (const String &Value) {

   Boolean result (False);

   if (_state.context && !_state.context->Name) {

      ConfigAttributeContext *ac (_state.context->attrTable.lookup (""));

      if (!ac) {

         ac = new ConfigAttributeContext (Value);
         if (ac && _state.context->attrTable.store ("", ac)) { result = True; }
         else if (ac) { delete ac; ac = 0; }
      }
      else {

         result = True;

         ac->lock.lock ();
            ac->value = Value;
         ac->lock.unlock ();
      }
   }

   return result;
}


/*!

\brief Appends a string to the current config context value.
\param[in] Value String containing value to be appended.
\param[in] IsFormatted If set to dmz::True the config context will be marked as
containing formatted data.
\return Returns dmz::True if the value was appended successfully.

*/
dmz::Boolean
dmz::Config::append_value (const String &Value, const Boolean IsFormatted) {

   Boolean result (False);

   if (_state.context) {

      if (!_state.context->Name) {

         if (IsFormatted) { _state.context->isFormatted = True; }

         ConfigAttributeContext *ac (_state.context->attrTable.lookup (""));

         if (!ac) {

            ac = new ConfigAttributeContext (Value);
            if (ac && _state.context->attrTable.store ("", ac)) { result = True; }
            else if (ac) { delete ac; ac = 0; }
         }
         else {

            result = True;

            ac->lock.lock ();
               ac->value << Value;
            ac->lock.unlock ();
         }
      }
      else {

         ConfigContext *cd (new ConfigContext (""));

         if (cd) {

            cd->isFormatted = IsFormatted;
            ConfigAttributeContext *ptr = new ConfigAttributeContext (Value);
            if (ptr && !cd->attrTable.store ("", ptr)) { delete ptr; ptr = 0; }
            _state.context->add_config (cd);
            result = True;
            cd->unref (); cd = 0;
         }
      }
   }

   return result;
}


/*!

\brief Gets the current value store in the config context.
\param[out] value String used to store the config context value.
\return Returns dmz::True if the config context value was stored in \a value.

*/
dmz::Boolean
dmz::Config::get_value (String &value) const {

   Boolean result (False);

   if (_state.context) {

      if (!_state.context->Name) {

         ConfigAttributeContext *at (_state.context->attrTable.lookup (""));

         if (at) {

            result = True;

            at->lock.lock ();
               value.flush () << at->value;
            at->lock.unlock ();
          }
      }
      else {

         ConfigContext::DataList *dl (_state.context->configTable.lookup (""));

         if (dl) {

            result = True;

            value.flush ();
            dl->lock.lock ();
               ConfigContext::DataStruct *ds (dl->head);
               while (ds) {

                  if (ds->handle && ds->context) {

                     ConfigAttributeContext *at (ds->context->attrTable.lookup (""));

                     if (at) {

                        at->lock.lock ();
                           value << at->value;
                        at->lock.unlock ();
                     }
                  }

                  ds = ds->next;
               }
            dl->lock.unlock ();
         }
      }
   }

   return result;
}


/*!

\brief Get first child config context stored in the config context.
\param[in] it ConfigIterator used to iterate over the child config contexts.
\param[out] data Config object used to store the first config context.
\return Returns dmz::True if the first child config context is stored in \a data.
Will return dmz::False if the config context contains no children.

*/
dmz::Boolean
dmz::Config::get_first_config (ConfigIterator &it, Config &data) const {

   Boolean result (False);

   if (_state.context) {

      ConfigContext::DataStruct *ds =
         _state.context->configOrderTable.get_first (it.state.it);

      if (ds) {

         if (ds->handle) { data.set_config_context (ds->context); result = True; }
         else { result = get_next_config (it, data); }
      }
   }

   return result;
}


/*!

\brief Get last child config context stored in the config context.
\param[in] it ConfigIterator used to iterate over the child config contexts.
\param[out] data Config object used to store the last config context.
\return Returns dmz::True if the last child config context is stored in \a data.
Will return dmz::False if the config context contains no children.

*/
dmz::Boolean
dmz::Config::get_last_config (ConfigIterator &it, Config &data) const {

   Boolean result (False);

   if (_state.context) {

      ConfigContext::DataStruct *ds =
         _state.context->configOrderTable.get_last (it.state.it);

      if (ds) {

         if (ds->handle) { data.set_config_context (ds->context); result = True; }
         else { result = get_prev_config (it, data); }
      }
   }

   return result;
}


/*!

\brief Get next child config context stored in the config context.
\param[in] it ConfigIterator used to iterate over the child config contexts.
\param[out] data Config object used to store the next child config context.
\return Returns dmz::True if the next child config context was stored in
\a data. Returns dmz::False when there are no more child config contexts to return.

*/
dmz::Boolean
dmz::Config::get_next_config (ConfigIterator &it, Config &data) const {

   Boolean result (False);

   if (_state.context) {

      ConfigContext::DataStruct *ds =
         _state.context->configOrderTable.get_next (it.state.it);

      while (ds && !ds->handle) {

         ds = _state.context->configOrderTable.get_next (it.state.it);
      }

      if (ds) { data.set_config_context (ds->context); result = True; }
   }

   return result;
}


/*!

\brief Get previous child config context stored in the config context.
\param[in] it ConfigIterator used to iterate over the child config contexts.
\param[out] data Config object used to store the previous child config context.
\return Returns dmz::True if the previous child config context was stored in
\a data. Returns dmz::False when there are no more child config contexts to return.

*/
dmz::Boolean
dmz::Config::get_prev_config (ConfigIterator &it, Config &data) const {

   Boolean result (False);

   if (_state.context) {

      ConfigContext::DataStruct *ds =
         _state.context->configOrderTable.get_prev (it.state.it);

      while (ds && !ds->handle) {

         ds = _state.context->configOrderTable.get_prev (it.state.it);
      }

      if (ds) { data.set_config_context (ds->context); result = True; }
   }

   return result;
}


/*!

\brief Overwrites config contexts found at the specified scope path.
\param[in] Scope String containing scope to use when overwriting.
\param[in] Data Config containing config context to use in the over write.
\return Returns dmz::True if config context was successfully overwritten.

*/
dmz::Boolean
dmz::Config::overwrite_config (const String &Scope, const Config &Data) {

   if (!Scope) {

      if (_state.context) { _state.context->remove_config (Data.get_name ()); }
   }
   else {

      Config list;

      lookup_all_config (Scope, list);

      ConfigIterator it;
      Config cd;

      for (
            Boolean found = list.get_first_config (it, cd);
            found;
            found = list.get_next_config (it, cd)) {

         ConfigContext *context (cd.get_config_context ());

         if (context) { context->remove_config (Data.get_name ()); }
      }
   }

   return add_config (Scope, Data);
}


/*!

\fn dmz::Config::overwrite_config (const Config &Data)
\brief Overwrite config contexts.
\details The is the same as calling overwrite_config ("", Data)
\return Returns dmz::True if the config contexts are over written.
\sa dmz::Config::overwrite_config (const String &Scope, const Config &Data)

*/


/*!

\brief Adds a config context.
\details The \a Scope parameter may be used to add a config context to child config
contexts. For example the following code snippet:
\code
dmz::Config dataDMZ;

if (global.lookup_data ("dmz", dataDMZ)) {

   dmz::Config dataType

   if (dataDMZ.lookup_data ("type", dataType)) {

      Config fooData ("foo");

      dataType.add_config (fooData);
   }
}
\endcode
May also be written as:
\code
Config fooData ("foo");

global.add_config ("dmz.type", fooData);
\endcode
\param[in] Scope String containing the scope at witch to add the config context.
\param[in] Data Config containing config context to add.
\return Returns dmz::True if the config context was successfully added.

*/
dmz::Boolean
dmz::Config::add_config (const String &Scope, const Config &Data) {

   Boolean result (False);

   if (_state.context && _state.context->Name) {

      ConfigContext *cd = Data.get_config_context ();
      ConfigContext *target = local_get_config_from_scope (Scope, _state.context, True);

      if (cd && target) { result = target->add_config (cd); }
   }

   return result;
}


/*!

\fn dmz::Config::add_config (const Config &Data)
\brief Adds a config context.
\param[in] Data Config containing config context to add.
\return Returns dmz::True if the config context was successfully added.

*/

/*!

\brief Adds the children from a config context.
\details This function is equivalent to iterating through \a Data's children and
adding them one at a time.
\param[in] Data Config containing config context of which its children will be added.
\return Returns dmz::True if the children were added.

*/
dmz::Boolean
dmz::Config::add_children (const Config &Data) {

   if (_state.context && _state.context->Name) {

      ConfigIterator it;
      Config cd;

      for (
            Boolean found = Data.get_first_config (it, cd);
            found;
            found = Data.get_next_config (it, cd)) { add_config (cd); }
   }

   return has_children ();
}


/*!

\brief Looks up a config context by name.
\details If there are multiple config context with the same same name, the last
config context that was stored with the given name is returned. The \a Name parameter
may be scoped like other functions. For example, the following is valid:
\code
dmz::Config result;
global.lookup_config ("dmz.types.foo", result);
\endcode
\param[in] Name String containing name of config context to lookup.
\param[out] data Config to store the found config context.
\return Returns dmz::True if the config context was found.

*/
dmz::Boolean
dmz::Config::lookup_config (const String &Name, Config &data) const {

   Boolean result (False);
   Config tmp;

   if (lookup_all_config (Name, tmp)) {

      result = True;

      ConfigIterator it;
      tmp.get_last_config (it, data);
   }

   return result;
}


/*!

\brief Looks up all config contexts with the given name.
\details All config context found with a matching name are stored as children of \a data.
\param[in] Name String containing name of config contexts to lookup.
\param[out] data Config to store the found config contexts.
\return Returns dmz::True if any config contexts were found.

*/
dmz::Boolean
dmz::Config::lookup_all_config (const String &Name, Config &data) const {

   Boolean result (False);
   StringTokenizer it (Name, LocalScopeChar);
   String sub = it.get_next ();
   Boolean done (!sub ? True : False);

   Config prev ("prev");
   prev.add_config (*this);
   Config current (sub);

   while (!done) {

      ConfigIterator tableIt;
      Config next;

      ConfigContext *curContext = current.get_config_context ();

      for (
            Boolean found = prev.get_first_config (tableIt, next);
            found;
            found = prev.get_next_config (tableIt, next)) {

         ConfigContext *nextContext = next.get_config_context ();

         if (nextContext && curContext) {

            ConfigContext::DataList *dl (nextContext->configTable.lookup (sub));

            if (dl) {

               dl->lock.lock ();
                  ConfigContext::DataStruct *ds = dl->head;
                  while (ds) {

                     if (ds->handle) {

                        curContext->add_config (ds->context);
                     }

                     ds = ds->next;
                  }
               dl->lock.unlock ();
            }
         }
      }

      if (current.is_empty ()) { done = True; }
      else {

         sub = it.get_next ();
         if (!sub) { data = current; done = True; }
         else { prev = current; Config next (sub); current = next; }
      }
   }

   if (data.is_empty ()) { data.set_config_context (0); }
   else { result = True; }

   return result;
}


/*!

\brief Looks up all config contexts with the given name and returns their children.
\details All children of config context found with a matching name are stored as
children of \a data. Also all attributes of the found config contexts are stored in
\a data.  If the same attributes are defined in multiple found config contexts, the
value of the last one found is used.
\param[in] Name String containing name of config contexts to lookup.
\param[out] data Config to store the children of found config contexts.
\return Returns dmz::True if any config contexts were found.

*/
dmz::Boolean
dmz::Config::lookup_all_config_merged (const String &Name, Config &data) const {

   Boolean result (False);

   Config foundData;

   if (lookup_all_config (Name, foundData)) {

      ConfigIterator it;
      Config tmp;

      Boolean found (foundData.get_first_config (it, tmp));

      result = found;

      if (result) {

         Config newName (tmp.get_name ());

         data = newName;
      }

      while (found) {

         ConfigIterator attrIt;

         String name, value;

         Boolean attrFound = tmp.get_first_attribute (attrIt, name, value);

         while (attrFound) {

            data.store_attribute (name, value);

            attrFound = tmp.get_next_attribute (attrIt, name, value);
         }

         data.add_children (tmp);

         found = foundData.get_next_config (it, tmp);
      }
   }

   return result;
}


//! For internal use.
void
dmz::Config::set_config_context (ConfigContext *context) {

   if (_state.context) { _state.context->unref (); }
   _state.context = context;
   if (_state.context) { _state.context->ref (); }
}


//! For internal use.
dmz::ConfigContext *
dmz::Config::get_config_context () const { return _state.context; }


static void
local_print_attribute (dmz::Stream &stream, const dmz::Config &At) {

   dmz::String buffer, name, value;
   dmz::ConfigIterator it;

   stream << At.get_name ();
   for (
         dmz::Boolean found = At.get_first_attribute (it, name, value);
         found;
         found = At.get_next_attribute (it, name, value)) {

      if (name) {

         if (!buffer) { buffer << ":"; }
         else { buffer << ", "; }
         buffer << "'" << name << "=" << value << "'";
      }
   }

   if (buffer) { stream << buffer; }
}


static void
local_print_config (
      dmz::Stream &stream,
      const dmz::Config &Data,
      const dmz::Int32 Indent) {

   dmz::String indent; indent.repeat (" ", Indent);

   stream << indent;
   local_print_attribute (stream, Data);
   stream << dmz::endl;

   dmz::Config current;
   dmz::ConfigIterator it;

   indent.flush ().repeat (" ", Indent + 3);

   dmz::Config ctable;

   for (
         dmz::Boolean found = Data.get_first_config (it, ctable);
         found;
         found = Data.get_next_config (it, ctable)) {

      local_print_config (stream, ctable, Indent + 3);
   }
}


/*!

\brief Writes a Config to a Stream.
\param[in] stream Stream to write to.
\param[in] Data Config containing config context to be written.

*/
dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::Config &Data) {

   local_print_config (stream, Data, 0);
   return stream;
}

