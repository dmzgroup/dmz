#include <dmzRuntime.h>
#include "dmzRuntimeContext.h"

/*!

\defgroup Runtime DMZ Kernel Runtime Layer

\class dmz::Runtime
\ingroup Runtime
\brief Creates a runtime context.
\details This class will typically only be instantiated once. It will create the
runtime context that is used by most other classes in the runtime. If a class needs a
dmz::RuntimeContext, it is usually not correct to construct a dmz::Runtime to acquire one,
but instead should be obtained from either a dmz::PluginInfo class or passed in from
an external source. Most all Plugins will not have need of this class.
\note dmz::Runtime::get_session_config and dmz::Runtime::set_session_config are not
intended for general session use.  \see dmzRuntimeSession.h.

*/

struct dmz::Runtime::State {

   RuntimeContext *context;
   State () : context (0) { context = new RuntimeContext; }
   ~State () { if (context) { context->unref (); context = 0; } }
};


//! Base constructor.
dmz::Runtime::Runtime () : _state (*(new State)) {;}


/*!

\brief Destructor.
\details Will unreference the runtime context.

*/
dmz::Runtime::~Runtime () { delete &_state; }


/*!

\brief Gets a pointer to the runtime context.
\return Returns a pointer to the runtime context.

*/
dmz::RuntimeContext *
dmz::Runtime::get_context () { return _state.context; }


/*!

\brief Stores the session Config in the runtime context.
\param[in] Session Config containing session data to store in the runtime context.
\note This function will overwrite any previous session data stored in the runtime
context. To add data to the session, use dmz::Runtime::get_session_data and add
additional session data to the returned Config object.
\sa dmzRuntimeSession.h.

*/
void
dmz::Runtime::set_session_config (const Config &Session) {

   if (_state.context) { _state.context->set_session_config (Session); }
}


/*!

\brief Retrieves the session Config from the runtime context.
\param[out] session Config that will contain the session data stored with the runtime
context.
\sa dmzRuntimeSession.h.

*/
void
dmz::Runtime::get_session_config (Config &session) {

   if (_state.context) { session = _state.context->get_session_config (); }
}


/*!

\brief Synchronizes the runtime context.
\details This function should be called once per frame. It is responsible for updating
simulation time, invoking timed events, and other time related functions found
in the Runtime.

*/
void
dmz::Runtime::sync () { if (_state.context) { _state.context->sync (); } }
