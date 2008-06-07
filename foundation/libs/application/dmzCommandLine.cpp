#include <dmzCommandLine.h>

namespace {

   struct argStruct {

      dmz::String arg;
      argStruct *next;

      argStruct (const dmz::String &Arg) : arg (Arg), next (0) {;}
      ~argStruct () { if (next) { delete next; next = 0; } }

      argStruct *copy () const {

         argStruct *result = new argStruct (arg);
         if (result && next) { result->next = next->copy (); }
         return result;
      }

      argStruct *get_tail () { return next ? next->get_tail () : this; }
   };
};


/*!

\class dmz::CommandLineArgs
\ingroup Foundation
\brief Container for parsed command line argument.
\details A command line argument has a name and a list of parameters.
For example:
\verbatim
% application -f file1.xml file2.xml
\endverbatim
The name of the command line argument would be "f" and the two parameters in the list
would be "file1.xml" and "file2.xml".
\sa dmz::CommandLine

*/


struct dmz::CommandLineArgs::State {

   String name;
   Int32 count;
   argStruct *head;
   argStruct *tail;
   argStruct *current;

   State () : count (0), head (0), tail (0), current (0) {;}
   ~State () { if (head) { delete head; head = tail = current = 0; } }
};


//! Constructor.
dmz::CommandLineArgs::CommandLineArgs () : _state (*(new State)) {;}


//! Copy Constructor.
dmz::CommandLineArgs::CommandLineArgs (const CommandLineArgs &Args) :
      _state (*(new State)) { *this = Args; }


/*!

\brief Named constructor.
\param[in] Name String containing the name of the command line argument.

*/
dmz::CommandLineArgs::CommandLineArgs (const String &Name) : _state (*(new State)) {

   _state.name = Name;
}


//! Destructor
dmz::CommandLineArgs::~CommandLineArgs () { delete &_state; }


//! Assignment operator.
dmz::CommandLineArgs &
dmz::CommandLineArgs::operator= (const dmz::CommandLineArgs &Args) {

   if (_state.head) {

      delete _state.head; _state.head = _state.tail = _state.current = 0;
   }

   if (Args._state.head) { _state.head = Args._state.head->copy (); }
   if (_state.head) { _state.tail = _state.head->get_tail (); }

   _state.name = Args._state.name;
   _state.count = Args._state.count;

   return *this;
}


//! Sets command line argument's name.
void
dmz::CommandLineArgs::set_name (const String &Name) { _state.name = Name; }


//! Gets command line argument's name.
dmz::String
dmz::CommandLineArgs::get_name () const { return _state.name; }


//! Gets number of parameters in the list.
dmz::Int32
dmz::CommandLineArgs::get_count () const { return _state.count; }


//! Appends parameter to the argument list.
void
dmz::CommandLineArgs::append_arg (const String &Arg) {

   if (_state.tail) {

      _state.tail->next = new argStruct (Arg);
      if (_state.tail->next) { _state.tail = _state.tail->next; }
  }
  else { _state.head = _state.tail = new argStruct (Arg); }

  _state.count++;
}


/*!

\brief Gets first argument parameter stored in the list.
\param[out] arg String to store the first argument in the list.
\return Returns dmz::True if the first element was returned. Returns dmz::False
if the list is empty.

*/
dmz::Boolean
dmz::CommandLineArgs::get_first_arg (String &arg) const {

   _state.current = _state.head;
   return get_next_arg (arg);
}


/*!

\brief Gets next argument parameter stored in the list.
\param[out] arg String to store the next argument in the list.
\return Returns dmz::True if the next element was returned. Returns dmz::False
if the entire list has been traversed.

*/
dmz::Boolean
dmz::CommandLineArgs::get_next_arg (String &arg) const {

   Boolean result (False);

   if (_state.current) {

      arg = _state.current->arg;
      _state.current = _state.current->next;
      result = True;
   }

   return result;
}


namespace {

   struct optStruct {

      optStruct *next;
      dmz::CommandLineArgs args;

      optStruct () : next (0) {;}
      ~optStruct () { if (next) { delete next; next = 0; } }
   };
};


/*!

\class dmz::CommandLine
\brief Parser the command line.
\details Stores the parsed command line in dmz::CommandLineArgs.
\sa dmz::CommandLineArgs

*/

struct dmz::CommandLine::State {

   optStruct *head;
   optStruct *tail;
   optStruct *current;

   State () : head (0), tail (0), current (0) {;}
   ~State ()  { if (head) { delete head; head = tail = current = 0; } }
};


//! Default constructor.
dmz::CommandLine::CommandLine () : _state (*(new State)) {;}


/*!

\brief Command line constructor.
\details Uses dmz::CommandLine::parse()
\param[in] argc Number arguments in \a argv.
\param[in] argv Array of pointers to character arrays. Each array is an argument.

*/
dmz::CommandLine::CommandLine (int argc, char *argv[]) : _state (*(new State)) {

   parse (argc, argv);
}


dmz::CommandLine::~CommandLine () { delete &_state; }


/*!

\brief Parses the command line.
\details
\param[in] argc Number arguments in \a argv.
\param[in] argv Array of pointers to character arrays. Each array is an argument.
\return Returns dmz::True if the command line was parsed successfully.

*/
dmz::Boolean
dmz::CommandLine::parse (int argc, char *argv[]) {

   Boolean result (False);

   for (int ix = 1; ix < argc; ix++) {

      if (argv[ix]) {

         if (argv[ix][0] == '-') {

            optStruct *os = new optStruct;

            if (os) {

               os->args.set_name (&(argv[ix][1]));

               if (!_state.tail) { _state.head = _state.tail = os; }
               else { _state.tail->next = os; _state.tail = os; }
            }

            result = True;
         }
         else if (_state.tail) { _state.tail->args.append_arg (argv[ix]); }
      }
   }

   return result;
}

/*!

\brief Adds arguments to the command line container.
\param[in] Args CommandLineArgs to be added.

*/
void
dmz::CommandLine::add_args (const CommandLineArgs &Args) {

   optStruct *os (new optStruct);

   if (os) {

      os->args = Args;
      if (_state.tail) { _state.tail->next = os; _state.tail = os; }
      else { _state.tail = _state.head = os; }
   }
}


/*!

\brief Gets first command line argument.
\param[out] args CommandLineArgs object for storing first argument in the list.
\return Returns dmz::True if the argument is returned. Returns dmz::False if the
CommandLine is empty.

*/
dmz::Boolean
dmz::CommandLine::get_first_option (CommandLineArgs &args) const {

   _state.current = _state.head;
   return get_next_option (args);
}


/*!

\brief Gets next command line argument.
\param[out] args CommandLineArgs object for storing next argument in the list.
\return Returns dmz::True if the next argument is returned. Returns dmz::False if the
CommandLine has returned all arguments.

*/
dmz::Boolean
dmz::CommandLine::get_next_option (CommandLineArgs &args) const {

   Boolean result (False);

   if (_state.current) {

      args = _state.current->args;
      _state.current = _state.current->next;
      result = True;
   }

   return result;
}

