#include <dmzTypesStringContainer.h>
#include <dmzTypesStringSub.h>
#include <dmzTypesHashTableStringTemplate.h>

/*!

\class dmz::StringSub
\ingroup Types
\brief Converts Strings that contain variables.
\details A variable is defined as $(\b Tag) where \b Tag corresponds to a \b Tag
store in the class by dmz::SubString::Store.
\code
StringSub sub;
sub.store ("foo", "new value");
String str ("This is a $(foo).");
str = sub.convert (str);
_log.out << str << endl; // Will output: "This is a new value"
\endcode
Variables maybe nested. They may also contain variables in their value. The following are
all valid:
\code
String s1 = "$(foo-$(moo))";
sub.store ("foo", "$(goo)"); // $(foo) will convert to the value of $(goo)
sub.store ("goo", "some value");
\endcode
The '\' character is used to escape control strings such as "$(" and ")".

*/

struct dmz::StringSub::State {

   Boolean firstCall;
   StringContainer list;
   HashTableStringTemplate<String> table;

   State () : firstCall (True) {;}
   ~State () { table.empty (); }
};

//! Constructor.
dmz::StringSub::StringSub () : _state (*(new State)) {;}


//! Destructor.
dmz::StringSub::~StringSub () { delete &_state; }


/*!

\brief Stores a variables value.
\details If the variable is already defined, it will be overwritten.
\param[in] Tag String containing the variables name.
\param[in] Value String containing the variables value.

*/
void
dmz::StringSub::store (const String &Tag, const String &Value) {

   String *ptr = _state.table.lookup (Tag);

   if (ptr) { *ptr = Value; }
   else {

      ptr = new String (Value);

      if (ptr && !_state.table.store (Tag, ptr)) { delete ptr; ptr = 0; }
   }
}

/*!

\brief Looks up a variables value.
\param[in] Tag String containing the variables name.
\return Returns a String containing the variables value.

*/
dmz::String
dmz::StringSub::lookup (const String &Tag) const {

   String result;

   String *ptr = _state.table.lookup (Tag);

   if (ptr) { result = *ptr; }

   return result;
}


/*!

\brief Removes a variable.
\param[in] Tag String containing name of variable to remove.

*/
void
dmz::StringSub::remove (const String &Tag) {

   String *ptr = _state.table.remove (Tag);

   if (ptr) { delete ptr; ptr = 0; }
}


namespace {

static const dmz::Int32 NormalMode = 0;
static const dmz::Int32 CloseMode = 1;

static const char Escape = '\\';
static const char Start = '$';
static const char Open = '(';
static const char Close = ')';

};


/*!

\brief Converts a string containing variables.
\details A string containing no variables will be returned unchanged. If a string contains
the escape character '\', it will be stripped from the returned value.
\param[in] Value String containing the value to be converted.
\return Returns a String containing the converted value.

*/
dmz::String
dmz::StringSub::convert (const String &Value) const {

   const Boolean FirstCall = _state.firstCall;
   _state.firstCall = False;

   String result;
   String scratch;

   Int32 place = 0;
   Int32 mode = NormalMode;
   Int32 depth = 0;
   Boolean doNotProcessNext = False;

   const Int32 Length = Value.get_length ();
   const char *Buffer = Value.get_buffer ();

   while (place < Length) {

      const char Next = Buffer[place];

      if (NormalMode == mode) {

         if (doNotProcessNext) { result += Next; doNotProcessNext = False; }
         else if (Next == Escape) { doNotProcessNext = True; }
         else if (Next == Start) {

            place++;
            if (place < Length) {

               if (Buffer[place] == Open) {

                  mode = CloseMode;
                  depth = 1;
               }
               else { result += Next; place--; }
            }
            else { result += Next; place--; }
         }
         else { result += Next; }
      }
      else if (CloseMode == mode) {

         if (doNotProcessNext) {

            scratch += Next;
            doNotProcessNext = False;
         }
         else if (Next == Start) {

            place++;

            if ((place < Length) && (Buffer[place] == Open)) {

               scratch += Next;
               scratch += Buffer[place];
               depth++;
            }
            else { place--; }
         }
         else if (Next == Close) {

            depth--;

            if (depth <= 0) {

               scratch = convert (scratch);
               const String *Ptr = _state.table.lookup (scratch);

               if (Ptr) {

                  if (!_state.list.contains (scratch)) {

                     _state.list.add (scratch);
                    
                     result += convert (*Ptr);
                  }
               }

               scratch.flush ();
               mode = NormalMode;
            }
            else { scratch += Next; }
         }
         else { scratch += Next; }
      }

      place++;
   }

   if (scratch) {

      result += "$(";
      result += convert (scratch);
   }

   if (FirstCall) {

      _state.list.clear ();
      _state.firstCall = FirstCall;
   }

   return result;
}

