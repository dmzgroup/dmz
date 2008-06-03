#include "dmzRuntimeContext.h"
#include "dmzRuntimeContextRTTI.h"
#include "dmzRuntimeContextRTTI.h"
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzRuntimeRTTINamed.h>

/*!

\file dmzRuntimeRTTI.h
\brief Provides RTTI services.
\details Contains functions that will map a unique handle to a interface. This mapping
allows the interface to be retrieved based on that unique handle.

*/

/*!

\file dmzRuntimeRTTINamed.h
\brief Provides Named RTTI services.
\details Contains functions that will map a unique name to a interface. This mapping
allows the interface to be retrieved based on that unique name.

*/

//! \addtogroup Runtime
//! @{


/*!

\brief Stores a named interface.
\details Defined in dmzRuntimeRTTINamed.h.
\param[in] InterfaceName String containing name of interface.
\param[in] context Pointer to the runtime context.
\param[in] ptr Void pointer to the interface.
\return Returns dmz::True if the interface was successfully stored with the specified
name.

*/
dmz::Boolean
dmz::store_rtti_named_interface (
      const String &InterfaceName,
      RuntimeContext *context,
      void *ptr) {

   Boolean result (False);

   if (context) {

      RuntimeContextRTTI *rtti = context->get_rtti_context ();
      if (rtti) { result = rtti->store_named_interface (InterfaceName, ptr); }
   }

   return result;
}


/*!

\brief Retrieves a named interface.
\details Defined in dmzRuntimeRTTINamed.h.
\param[in] InterfaceName String containing name of interface.
\param[in] context Pointer to the runtime context.
\return Returns a void pointer to the named interface. Will return a NULL pointer
if the named interface is not found.

*/
void *
dmz::lookup_rtti_named_interface (
      const String &InterfaceName,
      RuntimeContext *context) {

   void *result (0);

   if (context) {

      RuntimeContextRTTI *rtti = context->get_rtti_context ();
      if (rtti) { result = rtti->lookup_named_interface (InterfaceName); }
   }

   return result;
}


/*!

\brief Removes a named interface.
\details Defined in dmzRuntimeRTTINamed.h.
\param[in] InterfaceName String containing name of interface to remove.
\param[in] context Pointer to the runtime context.
\param[in] ptr Void pointer to interface to be removed.
\return Returns a void pointer to the named interface removed. Will return a NULL pointer
if the named interface is not found.

*/
void *
dmz::remove_rtti_named_interface (
      const String &InterfaceName,
      RuntimeContext *context,
      void *ptr) {

   void *result (0);

   if (context) {

      RuntimeContextRTTI *rtti = context->get_rtti_context ();
      if (rtti) {

         if (rtti->lookup_named_interface (InterfaceName) == ptr) {

            result = rtti->remove_named_interface (InterfaceName);
         }
      }
   }

   return result;
}


/*!

\brief Stores an interface with a unique handle.
\details Defined in dmzRuntimeRTTI.h.
\param[in] InterfaceName String containing name of interface.
\param[in] InstanceHandle Unique interface handle.
\param[in] context Pointer to the runtime context.
\param[in] ptr Void pointer to the interface.
\return Returns dmz::True if the interface was successfully stored with the specified
handle and name.

*/
dmz::Boolean
dmz::store_rtti_interface (
      const String &InterfaceName,
      const Handle InstanceHandle,
      RuntimeContext *context,
      void *ptr) {

   Boolean result (False);

   if (context) {

      RuntimeContextRTTI *rtti = context->get_rtti_context ();
      if (rtti) { result = rtti->store_interface (InterfaceName, InstanceHandle, ptr); }
   }

   return result;
}


/*!

\brief Stores an interface with a unique handle using the PluginInfo.
\details Defined in dmzRuntimeRTTI.h.
\param[in] InterfaceName String containing name of interface.
\param[in] Info PluginInfo containing unique handle and runtime context.
\param[in] ptr Void pointer to the interface.
\return Returns dmz::True if the interface was successfully stored with the specified
handle and name.

*/
dmz::Boolean
dmz::store_rtti_interface (
      const String &InterfaceName,
      const PluginInfo &Info,
      void *ptr) {

   return store_rtti_interface (
      InterfaceName,
      Info.get_handle (),
      Info.get_context (),
      ptr);
}


/*!

\brief Retrieves an interface.
\details Defined in dmzRuntimeRTTI.h.
\param[in] InterfaceName String containing name of interface.
\param[in] InstanceHandle Unique interface handle.
\param[in] context Pointer to the runtime context.
\return Returns a void pointer to the named interface. Will return a NULL pointer
if the named interface is not found.

*/
void *
dmz::lookup_rtti_interface (
      const String &InterfaceName,
      const Handle InstanceHandle,
      RuntimeContext *context) {

   void *result (0);

   if (context) {

      RuntimeContextRTTI *rtti = context->get_rtti_context ();
      if (rtti) { result = rtti->lookup_interface (InterfaceName, InstanceHandle); }
   }

   return result;
}


/*!

\brief Retrieves an interface.
\details Defined in dmzRuntimeRTTI.h. Uses the Plugin handle and runtime context to
retrieve interface pointer.
\param[in] InterfaceName String containing name of interface.
\param[in] InstanceName String containing the name of the instance.
\param[in] PluginPtr Pointer to a Plugin.
\return Returns a void pointer to the named interface. Will return a NULL pointer
if the named interface is not found or the \a InstanceName is not empty and InstanceName
not equal the name of the \a PluginPtr.

*/
void *
dmz::lookup_rtti_interface (
      const String &InterfaceName,
      const String &InstanceName,
      const Plugin *PluginPtr) {

   void *result (0);

   if (PluginPtr) {

      if (!InstanceName || (PluginPtr->get_plugin_name () == InstanceName)) {

         result = lookup_rtti_interface (
            InterfaceName,
            PluginPtr->get_plugin_handle (),
            PluginPtr->get_plugin_runtime_context ());
      }
   }

   return result;
}


/*!

\brief Validates that a handle is registered with one or more interfaces.
\details Defined in dmzRuntimeRTTI.h.
\param[in] InstanceHandle Unique handle to validate.
\param[in] context Pointer to runtime context.
\return Returns dmz::True if InstanceHandle is associated with one or more interfaces.

*/
dmz::Boolean
dmz::is_valid_rtti_handle (const Handle InstanceHandle, RuntimeContext *context) {

   Boolean result (False);

   if (context) {

      RuntimeContextRTTI *rtti = context->get_rtti_context ();
      if (rtti) { result = rtti->is_valid (InstanceHandle); }
   }

   return result;
}


/*!

\brief Tests if a Plugin has an exported interface.
\details Defined in dmzRuntimeRTTI.h.
\param[in] PluginPtr Pointer of Plugin to test for exported interfaces.
\return Returns dmz::True if \a PluginPtr exports one or more interfaces.

*/
dmz::Boolean
dmz::has_rtti_interface (const Plugin *PluginPtr) {

   return is_valid_rtti_handle (
      PluginPtr ? PluginPtr->get_plugin_handle () : 0,
      PluginPtr ? PluginPtr->get_plugin_runtime_context () : 0);
}


/*!

\brief Removes an interface with a unique handle.
\details Defined in dmzRuntimeRTTI.h.
\param[in] InterfaceName String containing name of interface to remove.
\param[in] InstanceHandle Unique interface handle.
\param[in] context Pointer to the runtime context.
\return Returns a void pointer to the interface removed. Will return a NULL pointer
if the named interface is not found.

*/
void *
dmz::remove_rtti_interface (
      const String &InterfaceName,
      const Handle InstanceHandle,
      RuntimeContext *context) {

   void *result (0);

   if (context) {

      RuntimeContextRTTI *rtti = context->get_rtti_context ();
      if (rtti) { result = rtti->remove_interface (InterfaceName, InstanceHandle); }
   }

   return result;
}


/*!

\brief Removes an interface with a unique handle.
\details Defined in dmzRuntimeRTTI.h.
\param[in] InterfaceName String containing name of interface to remove.
\param[in] Info PluginInfo containing unique handle and runtime context.
\return Returns a void pointer to the interface removed. Will return a NULL pointer
if the named interface is not found.

*/
void *
dmz::remove_rtti_interface (
      const String &InterfaceName,
      const PluginInfo &Info) {

   return remove_rtti_interface (InterfaceName, Info.get_handle (), Info.get_context ());
}

//! @}
