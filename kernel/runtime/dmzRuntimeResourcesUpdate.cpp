#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfig.h>
#include "dmzRuntimeContext.h"
#include "dmzRuntimeContextResources.h"
#include <dmzRuntimeLog.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeResourcesUpdate.h>
#include <dmzSystemFile.h>

/*!

\class dmz::ResourcesUpdate
\ingroup Runtime
\brief Runtime Resource Updater.
\details

*/

/*!

\brief Runtime context Constructor.
\param[in] context Pointer to the runtime context.

*/
dmz::ResourcesUpdate::ResourcesUpdate (RuntimeContext *context) : _context (0), _log (0) {

   _context = (context ? context->get_resources_context () : 0);
   if (_context) { _context->ref (); }
}


/*!

\brief Runtime context Constructor.
\param[in] context Pointer to the runtime context.
\param[in] log Pointer to the Log to user for error reporting.

*/
dmz::ResourcesUpdate::ResourcesUpdate (RuntimeContext *context, Log *log) : _context (0), _log (log) {

   _context = (context ? context->get_resources_context () : 0);
   if (_context) { _context->ref (); }
}


/*!

\brief PluginInfo Constructor.
\param[in] Info Reference to the PluginInfo.

*/
dmz::ResourcesUpdate::ResourcesUpdate (const PluginInfo &Info) : _context (0), _log (0) {

   RuntimeContext *context (Info.get_context ());
   _context = (context ? context->get_resources_context () : 0);
   if (_context) { _context->ref (); }
}


/*!

\brief PluginInfo Constructor.
\param[in] Info Reference to the PluginInfo.
\param[in] log Pointer to the Log to user for error reporting.

*/
dmz::ResourcesUpdate::ResourcesUpdate (const PluginInfo &Info, Log *log) : _context (0), _log (log) {

   RuntimeContext *context (Info.get_context ());
   _context = (context ? context->get_resources_context () : 0);
   if (_context) { _context->ref (); }
}


//! Destructor.
dmz::ResourcesUpdate::~ResourcesUpdate () { if (_context) { _context->unref (); _context = 0; } }


/*!

\brief Updates a resource config.
\details Existing resource can not be overwritten. They must be removed first before
a new resource config with the same name may be stored.
\param[in] ResourceName String containing the name of the resource Config.
\param[in] FileName String containing the resource filename.
\return Returns dmz::True if the resource config was updated. Will return dmz::False
if there is no resource config found or FileName is not valid.

*/
dmz::Boolean
dmz::ResourcesUpdate::update_resource_config (
      const String &ResourceName,
      const String &FileName) {

   Boolean result (False);

   if (_context) {

      Config *rc = _context->rcTable.lookup (ResourceName);

      if (rc) {

         if (is_valid_path (FileName)) {
            
            result = rc->store_attribute ("file", FileName);
         }
         else if (_log) {

            _log->error << "Invalid file name: " << FileName << endl;
         }
      }
      else if (_log) {

         _log->error << "Unable to find resource: " << ResourceName << endl;
      }
   }

   return result;
}
