#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfig.h>
#include "dmzRuntimeContext.h"
#include "dmzRuntimeContextResources.h"
#include <dmzRuntimeLog.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeResources.h>
#include <dmzSystemFile.h>

/*!

\class dmz::Resources
\ingroup Runtime
\brief Runtime Resource Map.
\details

*/

/*!

\brief Runtime context Constructor.
\param[in] context Pointer to the runtime context.

*/
dmz::Resources::Resources (RuntimeContext *context) : _context (0), _log (0) {

   _context = (context ? context->get_resources_context () : 0);
   if (_context) { _context->ref (); }
}


/*!

\brief Runtime context Constructor.
\param[in] context Pointer to the runtime context.
\param[in] log Pointer to the Log to user for error reporting.

*/
dmz::Resources::Resources (RuntimeContext *context, Log *log) : _context (0), _log (log) {

   _context = (context ? context->get_resources_context () : 0);
   if (_context) { _context->ref (); }
}


/*!

\brief PluginInfo Constructor.
\param[in] Info Reference to the PluginInfo.

*/
dmz::Resources::Resources (const PluginInfo &Info) : _context (0), _log (0) {

   RuntimeContext *context (Info.get_context ());
   _context = (context ? context->get_resources_context () : 0);
   if (_context) { _context->ref (); }
}


/*!

\brief PluginInfo Constructor.
\param[in] Info Reference to the PluginInfo.
\param[in] log Pointer to the Log to user for error reporting.

*/
dmz::Resources::Resources (const PluginInfo &Info, Log *log) : _context (0), _log (log) {

   RuntimeContext *context (Info.get_context ());
   _context = (context ? context->get_resources_context () : 0);
   if (_context) { _context->ref (); }
}


//! Destructor.
dmz::Resources::~Resources () { if (_context) { _context->unref (); _context = 0; } }


/*!

\brief Find resource.
\param[in] ResourceName String containing the name of the resource file.
\return Returns a String containing the found resource file. Will return an empty
string if the resource is not found.

*/
dmz::String
dmz::Resources::find_file (const String &ResourceName) const {

   String result;

   if (_context) {

      Config *rc = _context->rcTable.lookup (ResourceName);

      if (rc) {

         StringContainer searchPath;
         const String FileName = config_to_string ("file", *rc);
         const String PathName = config_to_string ("path", *rc);
         StringContainer *path = _context->pathTable.lookup (PathName);
         if (path) { searchPath = *path; }

         if (!dmz::find_file (searchPath, FileName, result) && _log) {

            _log->error << "Unable to find file: " << FileName << " for resource: "
               << ResourceName << endl;
         }
      }
      else if (_log) {

         _log->error << "Unable to find resource: " << ResourceName << endl;
      }
   }

   return result;
}


/*!

\brief Lookup a resource config.
\param[in] ResourceName String containing the name of the resource Config.
\param[out] resource Config used to return the found resource Config.
\return Returns dmz::True if the named resource Config was found.

*/
dmz::Boolean
dmz::Resources::lookup_resource_config (
      const String &ResourceName,
      Config &resource) const {

   Boolean result (False);

   if (_context) {

      Config *rc = _context->rcTable.lookup (ResourceName);

      if (rc) { resource = *rc; result = True; }
      else if (_log) {

         _log->error << "Failed to find resource config: " << ResourceName << endl;
      }
   }

   return result;
}


/*!

\brief Lookup a search path.
\param[in] SearchPathName String containing the name of the search path.
\param[out] searchPath StringContainer used to return the found search path.
\return Returns dmz::True if the named search path was found.

*/
dmz::Boolean
dmz::Resources::lookup_search_path (
      const String &SearchPathName,
      StringContainer &searchPath) const {

   Boolean result (False);

   if (_context) {

      StringContainer *pc = _context->pathTable.lookup (SearchPathName);

      if (pc) { searchPath = *pc; result = True; }
   }

   return result;
}

