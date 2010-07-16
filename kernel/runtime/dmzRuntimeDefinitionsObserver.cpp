#include "dmzRuntimeContext.h"
#include "dmzRuntimeContextDefinitions.h"
#include <dmzRuntimeDefinitionsObserver.h>
#include <dmzRuntimeHandle.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::DefinitionsObserver
\ingroup Runtime
\brief Observers runtime definitions.
\sa dmz::Definitions

*/

struct dmz::DefinitionsObserver::State {

   const Handle ObsHandle;
   const String Name;
   Log log;
   RuntimeHandle *handlePtr;
   UInt32 mask;
   UInt32 warnMask;

   RuntimeContext *context;
   RuntimeContextDefinitions *defs;

   State (const Handle &TheHandle, const String &TheName, RuntimeContext *theContext) :
         ObsHandle (TheHandle),
         Name (TheName),
         log (TheName + ".DefinitionsObserver", theContext),
         handlePtr (0),
         mask (0),
         warnMask (0),
         context (theContext),
         defs (0) {

      if (context) {

         context->ref ();

         defs = context->get_definitions_context ();

         if (defs) { defs->ref (); }

         if (!ObsHandle) {

            handlePtr = new RuntimeHandle (TheName + ".DefinitionsObserver", context);
         }
      }
   }

   ~State () {

      if (defs) { defs->unref (); defs = 0; }
      if (context) { context->unref (); context = 0; }
   }
};


//! Constructor.
dmz::DefinitionsObserver::DefinitionsObserver (RuntimeContext *context) :
      __state (*(new State (0, "<Anonymous Definitions Observer>", context))) {

}


//! Constructor.
dmz::DefinitionsObserver::DefinitionsObserver (const PluginInfo &Info) :
      __state (*(new State (Info.get_handle (), Info.get_name (), Info.get_context ()))) {

}


//! Destructor.
dmz::DefinitionsObserver::~DefinitionsObserver () {

   set_definitions_observer_callback_mask (0);
   delete &__state;
}


//! Gets observer's handle.
dmz::Handle
dmz::DefinitionsObserver::get_definitions_observer_handle () const {

   return __state.handlePtr ?
      __state.handlePtr->get_runtime_handle () :
      __state.ObsHandle;
}


//! Gets observer's name.
dmz::String
dmz::DefinitionsObserver::get_definitions_observer_name () const {

   return __state.Name;
}


//! Returns a mask of active callbacks.
dmz::UInt32
dmz::DefinitionsObserver::get_definitions_observer_callback_mask () {

   return __state.mask;
}


/*!

\brief Activates callbacks.
\details The callback activation mask should be composed of the following masks:
dmz::RuntimeNamedHandleMask, dmz::RuntimeStateMask, dmz::RuntimeObjectTypeMask, and/or
dmz::RuntimeEventTypeMask.
\param[in] TheMask A mask specifying which callbacks to activate.
\return Returns a mask of all callbacks that were activated.

*/
dmz::UInt32
dmz::DefinitionsObserver::set_definitions_observer_callback_mask (const UInt32 TheMask) {

   if (__state.context && __state.context->is_main_thread () && __state.defs) {

      const Handle ObsHandle = get_definitions_observer_handle ();

      if (RuntimeNamedHandleMask & TheMask) { 

         if ((RuntimeNamedHandleMask & __state.mask) == 0) {

            if (__state.defs->handleObsTable.store (ObsHandle, this)) {

               __state.mask |= RuntimeNamedHandleMask;

               HashTableHandleIterator it;
               String *ptr (0);

               while (__state.defs->namedHandleNameTable.get_next (it, ptr)) {

                  define_named_handle (it.get_hash_key (), *ptr);
               }
            }
         }
      }
      else if (RuntimeNamedHandleMask & __state.mask) {

         __state.mask &= ~RuntimeNamedHandleMask;
         __state.defs->handleObsTable.remove (ObsHandle);
      }

      if (RuntimeStateMask & TheMask) {

         if ((RuntimeStateMask & __state.mask) == 0) {

            if (__state.defs->maskObsTable.store (ObsHandle, this)) {

               __state.mask |= RuntimeStateMask;

               HashTableStringIterator it;
               Mask *ptr (0);

               while (__state.defs->maskTable.get_next (it, ptr)) {

                  define_state (*ptr, it.get_hash_key ());
               }
            }
         }
      }
      else if (RuntimeStateMask & __state.mask) {

         __state.mask &= ~RuntimeStateMask;
         __state.defs->maskObsTable.remove (ObsHandle);
      }

      if (RuntimeObjectTypeMask & TheMask) {

         if ((RuntimeObjectTypeMask & __state.mask) == 0) {

            if (__state.defs->objectObsTable.store (ObsHandle, this)) {

               __state.mask |= RuntimeObjectTypeMask;

               HashTableHandleIterator it;
               ObjectType *ptr (0);

               while (__state.defs->objectHandleTable.get_next (it, ptr)) {

                  define_object_type (*ptr);
               }
            }
         }
      }
      else if (RuntimeObjectTypeMask & __state.mask) {

         __state.mask &= ~RuntimeObjectTypeMask;
         __state.defs->objectObsTable.remove (ObsHandle);
      }

      if (RuntimeEventTypeMask & TheMask) {

         if ((RuntimeEventTypeMask & __state.mask) == 0) {

            if (__state.defs->eventObsTable.store (ObsHandle, this)) {

               __state.mask |= RuntimeEventTypeMask;

               HashTableHandleIterator it;
               EventType *ptr (0);

               while (__state.defs->eventHandleTable.get_next (it, ptr)) {

                  define_event_type (*ptr);
               }
            }
         }
      }
      else if (RuntimeEventTypeMask & __state.mask) {

         __state.mask &= ~RuntimeEventTypeMask;
         __state.defs->eventObsTable.remove (ObsHandle);
      }

      if (RuntimeMessageMask & TheMask) {

         if ((RuntimeMessageMask & __state.mask) == 0) {

            if (__state.defs->messageObsTable.store (ObsHandle, this)) {

               __state.mask |= RuntimeMessageMask;

               HashTableHandleIterator it;
               Message *ptr (0);

               while (__state.defs->messageHandleTable.get_next (it, ptr)) {

                  define_message (*ptr);
               }
            }
         }
      }
      else if (RuntimeMessageMask & __state.mask) {

         __state.mask &= ~RuntimeMessageMask;
         __state.defs->messageObsTable.remove (ObsHandle);
      }
   }

   return __state.mask;
}


/*!

\brief Callback for named handle definition.
\details Activated by dmz::RuntimeNamedHandleMask.
\param[in] TheHandle Value of the named Handle.
\param[in] Name String containing the name of the Handle.

*/
void
dmz::DefinitionsObserver::define_named_handle (const Handle, const String &) {

   if ((RuntimeNamedHandleMask & __state.warnMask) == 0) {

      __state.warnMask |= RuntimeNamedHandleMask;
      __state.log.warn << "Base define_named_handle called."
         << " Function should have been overridden?" << endl;
   }
}


/*!

\brief Callback for state definition.
\details Activated by dmz::RuntimeStateMask.
\param[in] TheState Mask containing state being defined.
\param[in] Name String containing the name of the state.

*/
void
dmz::DefinitionsObserver::define_state (const Mask &, const String &) {

   if ((RuntimeStateMask & __state.warnMask) == 0) {

      __state.warnMask |= RuntimeStateMask;
      __state.log.warn << "Base define_state called."
         << " Function should have been overridden?" << endl;
   }
}


/*!

\brief Callback for dmz::ObjectType definition.
\details Activated by dmz::RuntimeObjectTypeMask.
\param[in] Type ObjectType being defined.

*/
void
dmz::DefinitionsObserver::define_object_type (const ObjectType &) {

   if ((RuntimeObjectTypeMask & __state.warnMask) == 0) {

      __state.warnMask |= RuntimeObjectTypeMask;
      __state.log.warn << "Base define_object_type called."
         << " Function should have been overridden?" << endl;
   }
}


/*!

\brief Callback for dmz::EventType definition.
\details Activated by dmz::RuntimeEventTypeMask.
\param[in] Type EventType being defined.

*/
void
dmz::DefinitionsObserver::define_event_type (const EventType &) {

   if ((RuntimeEventTypeMask & __state.warnMask) == 0) {

      __state.warnMask |= RuntimeEventTypeMask;
      __state.log.warn << "Base define_event_type called."
         << " Function should have been overridden?" << endl;
   }
}


/*!

\brief Callback for dmz::Message definition.
\details Activated by dmz::RuntimeMessageMask.
\param[in] Type Message being defined.

*/
void
dmz::DefinitionsObserver::define_message (const Message &) {

   if ((RuntimeMessageMask & __state.warnMask) == 0) {

      __state.warnMask |= RuntimeMessageMask;
      __state.log.warn << "Base define_message called."
         << " Function should have been overridden?" << endl;
   }
}
