#include <dmzSystemFile.h>
#include <dmzRuntimeConfigToBase.h>
#include <dmzRuntimeConfig.h>
#include "dmzRuntimeContext.h"
#include "dmzRuntimeContextResources.h"
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeResources.h>

dmz::Resources::Resources (RuntimeContext *context) : _context (0) {

   _context = (context ? context->get_resources_context () : 0);
   if (_context) { _context->ref (); }
}


dmz::Resources::Resources (const PluginInfo &Info) : _context (0) {

   RuntimeContext *context (Info.get_context ());
   _context = (context ? context->get_resources_context () : 0);
   if (_context) { _context->ref (); }
}


dmz::Resources::~Resources () { if (_context) { _context->unref (); _context = 0; } }


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

         dmz::find_file (searchPath, FileName, result);
      }
   }

   return result;
}


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


dmz::Boolean
dmz::Resources::lookup_resource_config (
      const String &ResourceName,
      Config &resource) const {

   Boolean result (False);

   if (_context) {

      Config *rc = _context->rcTable.lookup (ResourceName);

      if (rc) { resource = *rc; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::Resources::remove_resource_config (const String &ResourceName) {

   Boolean result (False);

   if (_context) {

      Config *rc = _context->rcTable.remove (ResourceName);

      if (rc) { delete rc; rc = 0; result = True; }
   }

   return result;
}


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


dmz::Boolean
dmz::Resources::remove_search_path (const String &SearchPathName) {

   Boolean result (False);

   if (_context) {

      PathContainer *pc = _context->pathTable.remove (SearchPathName);

      if (pc) { delete pc; pc = 0; result = True; }
   }

   return result;
}
