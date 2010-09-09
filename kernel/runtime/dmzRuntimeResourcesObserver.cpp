#include "dmzRuntimeContext.h"
#include "dmzRuntimeContextResources.h"
#include <dmzRuntimeHandle.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeResourcesObserver.h>

/*!

\file dmzRuntimeResourcesObserver.h
\ingroup Runtime
\brief Contains ResourcesObserver class.

\enum dmz::ResourcesActivateModeEnum
\ingroup Runtime
\brief Resources callback activate mode.

\enum dmz::ResourcesUpdateTypeEnum
\ingroup Runtime
\brief Resources update callback type.

\class dmz::ResourcesObserver
\ingroup Runtime
\brief Observes changes in runtime Resources.

*/

struct dmz::ResourcesObserver::State {

   RuntimeHandle *handlePtr;
   const Handle ObsHandle;
   Log log;
   RuntimeContextResources *rc;
   UInt32 mask;
   UInt32 warnMask;

   State (const Handle TheObsHandle, const String &ObsName, RuntimeContext *context) :
         handlePtr (
            TheObsHandle == 0 ?
               new RuntimeHandle (ObsName + ".ResourcesObserver", context) :
               0),
         ObsHandle (handlePtr ? handlePtr->get_runtime_handle () : TheObsHandle),
         log (ObsName + "ResourcesObserver", context),
         rc (context ? context->get_resources_context () : 0),
         mask (0),
         warnMask (0) {

      if (rc) { rc->ref (); }
   }

   ~State () {

      if (rc) { rc->unref (); rc = 0; }

      if (handlePtr) { delete handlePtr; handlePtr = 0; }
   }
};


/*!

\brief Constructor.
\param[in] context Pointer to the RuntimeContext.

*/
dmz::ResourcesObserver::ResourcesObserver (RuntimeContext *context) :
      __state (*(new State (0, "<Anonymous Resources Observer>", context))) {;}


/*!

\brief Constructor.
\param[in] Info Reference to the PluginInfo.

*/
dmz::ResourcesObserver::ResourcesObserver (const PluginInfo &Info) :
      __state (*(new State (Info.get_handle (), Info.get_name (), Info.get_context ()))) {
}


//! Destructor.
dmz::ResourcesObserver::~ResourcesObserver () {

   set_resources_observer_callback_mask (ResourcesDumpNone, 0);
   delete &__state;
}


//! Returns a mask of active callbacks.
dmz::UInt32
dmz::ResourcesObserver::get_resources_observer_callback_mask () const {

   return __state.mask;
}


/*!

\brief Activates dmz::ResourcesObserver::update_resource() callback.
\param[in] Mode Specifies if all currently defined resources should be dumped to observer
upon activation.
\param[in] TheMask A mask specifying which callbacks to activate.
\return Returns a mask of all callbacks that were activated.

*/
dmz::UInt32
dmz::ResourcesObserver::set_resources_observer_callback_mask (
      const ResourcesActivateModeEnum Mode,
      const UInt32 TheMask) {

   if (__state.rc) {

      const Boolean Dump = (Mode == ResourcesDumpAll);

      const Handle ObsHandle = __state.ObsHandle;

      if (ResourcesPathMask & TheMask) {

         if ((ResourcesPathMask & __state.mask) == 0) {

            if (__state.rc->pathObsTable.store (ObsHandle, this)) {

               __state.mask |= ResourcesPathMask;

               if (Dump) {

                  HashTableStringIterator it;
                  StringContainer *ptr (0);

                  while (__state.rc->pathTable.get_next (it, ptr)) {

                     update_resources_path (it.get_hash_key (), ResourcesCreated);
                  }
               }
            }
         }
      }
      else if (ResourcesPathMask & __state.mask) {

         __state.mask &= ~ResourcesPathMask;
         __state.rc->pathObsTable.remove (ObsHandle);
      }

      if (ResourcesResourceMask & TheMask) {

         if ((ResourcesResourceMask & __state.mask) == 0) {

            if (__state.rc->rcObsTable.store (ObsHandle, this)) {

               __state.mask |= ResourcesResourceMask;

               if (Dump) {

                  HashTableStringIterator it;
                  Config *ptr (0);

                  while (__state.rc->rcTable.get_next (it, ptr)) {

                     update_resource (it.get_hash_key (), ResourcesCreated);
                  }
               }
            }
         }
      }
      else if (ResourcesResourceMask & __state.mask) {

         __state.mask &= ~ResourcesResourceMask;
         __state.rc->rcObsTable.remove (ObsHandle);
      }
   }

   return __state.mask;
}


/*!

\brief Dumps the current Resources to the observer.
\details The dmz::ResourcesObserver::update_resource() Mode parameter will be set to dmz::ResourcesDumped.

*/
void
dmz::ResourcesObserver::dump_current_resources () {

   if (__state.rc) {

      if (__state.mask & ResourcesPathMask) {

         HashTableStringIterator it;
         StringContainer *ptr (0);

         while (__state.rc->pathTable.get_next (it, ptr)) {

            update_resources_path (it.get_hash_key (), ResourcesDumped);
         }
      }

      if (__state.mask & ResourcesResourceMask) {

         HashTableStringIterator it;
         Config *ptr (0);

         while (__state.rc->rcTable.get_next (it, ptr)) {

            update_resource (it.get_hash_key (), ResourcesDumped);
         }
      }
   }
}


/*!

\brief Function invoked when runtime Resources paths are create, updated, or removed.
\param[in] Name String containing name of path being updated.
\param[in] Type Type of update.

*/
void
dmz::ResourcesObserver::update_resources_path (
      const String &Name,
      const ResourcesUpdateTypeEnum Type) {

   if ((ResourcesPathMask & __state.warnMask) == 0) {

      __state.warnMask |= ResourcesPathMask;
      __state.log.warn << "Base dmz::ResourcesObserver::update_resources_path called."
         << " Function should have been overridden?" << endl;
   }
}


/*!

\brief Function invoked when runtime Resources are create, updated, or removed.
\param[in] Name String containing name of resource being updated.
\param[in] Type Type of update.

*/
void
dmz::ResourcesObserver::update_resource (
      const String &Name,
      const ResourcesUpdateTypeEnum Type) {

   if ((ResourcesResourceMask & __state.warnMask) == 0) {

      __state.warnMask |= ResourcesResourceMask;
      __state.log.warn << "Base dmz::ResourcesObserver::update_resource called."
         << " Function should have been overridden?" << endl;
   }
}


