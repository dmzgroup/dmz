#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfig.h>
#include "dmzRuntimeContext.h"
#include "dmzRuntimeContextResources.h"
#include <dmzRuntimeLog.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeResourcesObserver.h>
#include <dmzRuntimeResourcesUpdate.h>
#include <dmzSystemFile.h>

/*!

\class dmz::ResourcesUpdate
\ingroup Runtime
\brief Runtime Resources Updater.
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
dmz::ResourcesUpdate::ResourcesUpdate (RuntimeContext *context, Log *log) :
      _context (0),
      _log (log) {

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
dmz::ResourcesUpdate::ResourcesUpdate (const PluginInfo &Info, Log *log) :
      _context (0),
      _log (log) {

   RuntimeContext *context (Info.get_context ());
   _context = (context ? context->get_resources_context () : 0);
   if (_context) { _context->ref (); }
}


//! Destructor.
dmz::ResourcesUpdate::~ResourcesUpdate () {

   if (_context) { _context->unref (); _context = 0; }
}


/*!

\brief Updates a resource's Config.
\param[in] ResourceName String containing the name of the resource Config.
\param[in] Data Config containing the resource's Config data.
\return Returns dmz::True if the resource config was updated.

*/
dmz::Boolean
dmz::ResourcesUpdate::update_resource (
      const String &ResourceName,
      const Config &Data) {

   Boolean result (False);

   if (_context) {

      ResourcesUpdateTypeEnum type = ResourcesCreated;

      Config *rc = _context->rcTable.remove (ResourceName);

      if (rc) {

         type = ResourcesUpdated;
         delete rc; rc = 0;
      }

      rc = new Config (Data);

      if (_context->rcTable.store (ResourceName, rc)) {

         HashTableHandleIterator it;
         ResourcesObserver *obs (0);

         while (_context->rcObsTable.get_next (it, obs)) {

            obs->update_resource (ResourceName, type);
         }

         result = True;
      }
      else { delete rc; rc = 0; }
   }

   return result;
}


/*!

\brief Removes resource definition.
\param[in] ResourceName String containing the name of the resource to remove.
\return Returns dmz::True if the resource was successfully removed.

*/
dmz::Boolean
dmz::ResourcesUpdate::remove_resource (const String &ResourceName) {

   Boolean result (False);

   if (_context) {

      Config *rc = _context->rcTable.remove (ResourceName);

      if (rc) {

         delete rc; rc = 0;

         HashTableHandleIterator it;
         ResourcesObserver *obs (0);

         while (_context->rcObsTable.get_next (it, obs)) {

            obs->update_resource (ResourceName, ResourcesRemoved);
         }

         result = True;
      }
   }

   return result;
}


/*!

\brief Updates a resource file.
\param[in] ResourceName String containing the name of the resource Config.
\param[in] FileName String containing the resource filename.
\return Returns dmz::True if the resource config was updated. Will return dmz::False
if there is no resource config found or FileName is not valid.

*/
dmz::Boolean
dmz::ResourcesUpdate::update_resource_file (
      const String &ResourceName,
      const String &FileName) {

   Boolean result (False);

   if (_context) {

      Config *rc = _context->rcTable.lookup (ResourceName);

      if (rc) {

         if (is_valid_path (FileName)) {
            
            result = rc->store_attribute ("file", FileName);

            HashTableHandleIterator it;
            ResourcesObserver *obs (0);

            while (_context->rcObsTable.get_next (it, obs)) {

               obs->update_resource (ResourceName, ResourcesUpdated);
            }
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


/*!

\brief Adds a search path.
\param[in] Name String containing the name of the search to add the path.
\param[in] Path String containing the path to add to the search list.
\return Returns dmz::True if the path was successfully added to the search.

*/
dmz::Boolean
dmz::ResourcesUpdate::add_search_path (const String &Name, const String &Path) {

   Boolean result (False);

   if (_context) {

      StringContainer *list = _context->pathTable.lookup (Name);

      if (!list) {

         list = new StringContainer;
         if (_context->pathTable.store (Name, list)) { delete list; list = 0; }
      }

      if (list) {

         list->add (Path);
         result = True;

         HashTableHandleIterator it;
         ResourcesObserver *obs (0);

         while (_context->pathObsTable.get_next (it, obs)) {

            obs->update_resources_path (Name, ResourcesUpdated);
         }
      }
   }

   return result;
}


/*!

\brief Updates a search list.
\details If the search already exists, it is overwritten by \p Paths. If the search
does not exist, it is created.
\param[in] Name String containing the name of the search.
\param[in] Paths StringContainer containing a list of the paths to use in the search.
\return Returns dmz::True if the search was successfully updated.

*/
dmz::Boolean
dmz::ResourcesUpdate::update_search_path (
      const String &Name,
      const StringContainer &Paths) {

   Boolean result (False);

   if (_context) {

      ResourcesUpdateTypeEnum type = ResourcesUpdated;

      StringContainer *list = _context->pathTable.remove (Name);

      if (list) { delete list; list = 0; }

      if (Paths.get_count () > 0) {

         list = new StringContainer (Paths);

         if (list && _context->pathTable.store (Name, list)) { result = True; }
         else if (list) { delete list; list = 0; }

      }
      else {

         result = True;
         type = ResourcesRemoved;
      }

      HashTableHandleIterator it;
      ResourcesObserver *obs (0);

      while (_context->pathObsTable.get_next (it, obs)) {

         obs->update_resources_path (Name, type);
      }
   }

   return result;
}

