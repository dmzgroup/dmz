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

         PathContainer searchPath;
         const String FileName = config_to_string ("file", *rc);
         const String PathName = config_to_string ("path", *rc);
         PathContainer *path = _context->pathTable.lookup (PathName);
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

\brief Store a resource config.
\details Existing resource can not be overwritten. They must be removed first before
a new resource config with the same name may be stored.
\param[in] ResourceName String containing the name of the resource Config.
\param[in] Resource Config containing the resource data to store.
\return Returns dmz::True if the resource config was stored. Will return dmz::False
if there is already a resource config with the same name.

*/
dmz::Boolean
dmz::Resources::store_resource_config (
      const String &ResourceName,
      const Config &Resource) {

   Boolean result (False);

   if (_context) {

      Config *rc = new Config (Resource);

      result = _context->rcTable.store (ResourceName, rc);

      if (!result) { delete rc; rc = 0; }
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

\brief Removes a resource config.
\param[in] ResourceName String containing the name of the resource Config.
\return Returns dmz::True if the named resource Config was removed.

*/
dmz::Boolean
dmz::Resources::remove_resource_config (const String &ResourceName) {

   Boolean result (False);

   if (_context) {

      Config *rc = _context->rcTable.remove (ResourceName);

      if (rc) { delete rc; rc = 0; result = True; }
   }

   return result;
}


/*!

\brief Store a search path.
\details Existing search paths can not be overwritten. They must be removed first before
a new search path with the same name may be stored.
\param[in] SearchPathName String containing the name of the search path.
\param[in] SearchPath PathContainer containing the search paths to store.
\return Returns dmz::True if the search path was stored. Will return dmz::False
if there is already a search path with the same name.

*/
dmz::Boolean
dmz::Resources::store_search_path (
      const String &SearchPathName,
      const PathContainer &SearchPath) {

   Boolean result (False);

   if (_context) {

      PathContainer *pc = new PathContainer (SearchPath);

      result = _context->pathTable.store (SearchPathName, pc);

      if (!result) { delete pc; pc = 0; }
   }

   return result;
}


/*!

\brief Lookup a search path.
\param[in] SearchPathName String containing the name of the search path.
\param[out] searchPath PathContainer used to return the found search path.
\return Returns dmz::True if the named search path was found.

*/
dmz::Boolean
dmz::Resources::lookup_search_path (
      const String &SearchPathName,
      PathContainer &searchPath) const {

   Boolean result (False);

   if (_context) {

      PathContainer *pc = _context->pathTable.lookup (SearchPathName);

      if (pc) { searchPath = *pc; result = True; }
   }

   return result;
}


/*!

\brief Removes a search path.
\param[in] SearchPathName String containing the name of the search path.
\return Returns dmz::True if the named search path was removed.

*/
dmz::Boolean
dmz::Resources::remove_search_path (const String &SearchPathName) {

   Boolean result (False);

   if (_context) {

      PathContainer *pc = _context->pathTable.remove (SearchPathName);

      if (pc) { delete pc; pc = 0; result = True; }
   }

   return result;
}
