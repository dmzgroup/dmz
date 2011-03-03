#include <dmzRuntime.h>
#include <dmzRuntimeContainer.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeUUID.h>
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

   State (RuntimeContext *theContext) : context (theContext) {

      if (context) { context->ref (); }
   }

   ~State () { if (context) { context->unref (); context = 0; } }
};


//! Base constructor.
dmz::Runtime::Runtime () : _state (*(new State)) {;}


//! Base constructor.
dmz::Runtime::Runtime (RuntimeContext *context) : _state (*(new State (context))) {;}


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

\brief TimeSlice synchronizes the runtime context.
\details This function should be called once per frame. It is responsible for updating
simulation time, invoking timed events, and other time related functions found
in the Runtime.

*/
void
dmz::Runtime::update_time_slice () {

   if (_state.context) { _state.context->update_time_slice (); }
}


/*!

\brief Gets the unique runtime UUID.
\ingroup Runtime
\param[in] context Pointer to the runtime context.
\details A new UUID is allocated every time a runtime context is created. This UUID
may be used to identify the application while it is running.
\return Returns the unique UUID associated with the RuntimeContext.

*/
dmz::UUID
dmz::get_runtime_uuid (RuntimeContext *context) {

   UUID result;
   if (context) { result = context->uuid; }
   return result;
}


/*!

\brief Gets the unique runtime UUID.
\ingroup Runtime
\param[in] Info Reference to the PluginInfo.
\details A new UUID is allocated every time a runtime context is created. This UUID
may be used to identify the application while it is running.
\return Returns the unique UUID associated with the RuntimeContext.

*/
dmz::UUID
dmz::get_runtime_uuid (const PluginInfo &Info) {

   UUID result;
   RuntimeContext *context (Info.get_context ());
   if (context) { result = context->uuid; }
   return result;
}


//! \cond
struct dmz::RuntimeContainer::State {

   RuntimeContext *context;

   void set_context (RuntimeContext *theContext) {

      if (context != theContext) {

         if (context) { context->unref (); context = 0; }
         context = theContext;
         if (context) { context->ref (); }
      }
   }

   State (RuntimeContext *theContext) : context (0) { set_context (theContext); }
   ~State () { set_context (0); }
};
//! \endcond


/*!

\class dmz::RuntimeContainer
\ingroup Runtime
\brief Holds a reference to the RuntimeContext.
\details This class should be use to hold a reference to the RuntimeContext when
it is need for an extended period of time by a class that does not have access to
a PluginInfo class or the dmz::Plugin::get_plugin_runtime_context() function.

*/

/*!

\brief Constructor.
\param[in] context Pointer to the RuntimeContext.

*/
dmz::RuntimeContainer::RuntimeContainer (RuntimeContext *context) :
      _state (*(new State (context))) {;}


/*!

\brief Constructor.
\details The RuntimeContext is extracted from the PluginInfo.
\param[in] Info Reference to the PluginInfo..

*/
dmz::RuntimeContainer::RuntimeContainer (const PluginInfo &Info) :
      _state (*(new State (Info.get_context ()))) {;}


//! Destructor.
dmz::RuntimeContainer::~RuntimeContainer () { delete &_state; }


//! Releases the RuntimeContext.
void
dmz::RuntimeContainer::clear () { _state.set_context (0); }


//! Stores the RuntimeContext.
void
dmz::RuntimeContainer::set_context (RuntimeContext *context) {

   _state.set_context (context);
}


//! Gets the RuntimeContext current stored in the class.
dmz::RuntimeContext *
dmz::RuntimeContainer::get_context () const { return _state.context; }


