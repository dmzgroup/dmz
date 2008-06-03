#include "dmzRuntimeContext.h"
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>

#include <qdb.h>

/*!
   
\file dmzRuntimeSession.h
\brief Contains functions for manipulating the session data stored in the runtime context.
      
*/ 

//! \addtogroup Runtime
//! @{

/*!

\brief Stores session data in the runtime context.
\details Defined in dmzRuntimeSession.h.
This function will over write any Config with the same name as \a data in the
session.
\param[in] context Pointer to the runtime context.
\param[in] data Config data to store in runtime context.

*/
void
dmz::set_session_config (RuntimeContext *context, Config &data) {

   if (context) { context->get_session_config ().overwrite_config (data); }
}


/*!

\brief Returns all session named session data.
\details Defined in dmzRuntimeSession.h.
\param[in] Name String containing session Config to retrieve.
\param[in] context Pointer to the runtime context.
\return Returns a Config object  containing all the children of the first level session
Config objects with the specified \a Name.

*/
dmz::Config
dmz::get_session_config (const String &Name, RuntimeContext *context) {

   Config result;

   if (context) {

      context->get_session_config ().lookup_all_config_merged (Name, result);
   } 

   return result;
}


/*!

\brief Retrieve session data based on the PluginInfo.
\details Defined in dmzRuntimeSession.h.
\note This function uses the runtime context stored in PluginInfo to retrieve the
session data.
\param[in] Info The PluginInfo.
\return Returns a Config object  containing all the children of the first level session
based on the Plugin name stored in PluginInfo.
\sa dmz::PluginInfo::get_name \n dmz::PluginInfo::get_context

*/
dmz::Config
dmz::get_session_config (const PluginInfo &Info) {

   return get_session_config (Info.get_name (), Info.get_context ());
}

//! @}
