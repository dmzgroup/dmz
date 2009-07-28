#include "dmzRuntimeContext.h"
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeHandle.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzSystemDynamicLibrary.h>
#include <dmzTypesHashTableUInt32.h>

/*!

\file dmzRuntimePluginInfo.h
\brief Contains dmz::PluginInfo class.

\class dmz::PluginInfo
\ingroup Runtime
\brief Contains initialization information for a dmz::Plugin.
\details The dmz::PluginInfo is typically passed as a parameter of the Plugin's factory
function.  The PluginInfo contains information such as the RuntimeContext,
the Plugin's unique handle, and the Plugins's name.
The PluginInfo is responsible for allocating and releasing the Plugin's unique handle.
\sa create_plugin_factory_function

\enum dmz::PluginDeleteModeEnum
\ingroup Runtime
\brief Plugin delete mode enumeration.
\details Defined in dmzRuntimePluginInfo.h.

*/

struct dmz::PluginInfo::State {

   RuntimeHandle *_handlePtr;
   const String Name;
   const String ClassName;
   const String FactoryName;
   const String ScopeName;
   const Handle PluginHandle;
   PluginDeleteModeEnum deleteMode;
   RuntimeContext *context;
   DynamicLibrary *lib;
   HashTableUInt32 levelTable;
   HashTableUInt32Iterator it;

   State (
         const String &TheName,
         const String TheClassName,
         const String TheFactoryName,
         const String TheScopeName,
         const PluginDeleteModeEnum TheDeleteMode,
         RuntimeContext *theContext,
         DynamicLibrary *theLib) :
         _handlePtr (TheName ? 0 : new RuntimeHandle ("Plugin", theContext)),
         Name (TheName ? TheName : "Unnamed Component"),
         ClassName (TheClassName),
         FactoryName (TheFactoryName),
         ScopeName (TheScopeName),
         PluginHandle (_handlePtr ?
            _handlePtr->get_runtime_handle () :
            Definitions (theContext).create_named_handle (TheName)),
         deleteMode (TheDeleteMode),
         context (theContext),
         lib (theLib) {

      if (context) { context->ref (); }
   }

   ~State () {

      levelTable.clear ();

      if (_handlePtr) { delete _handlePtr; _handlePtr = 0; }
      else {

         Definitions defs (context);
         defs.release_unique_name (Name);
      }

      if (context) { context->unref (); context = 0; }

      if (lib) { delete lib; lib = 0; }
   }
};


/*!

\brief Constructor.
\param[in] Name String containing the name of the Plugin.
\param[in] ClassName String containing the class name of the Plugin.
\param[in] FactoryName String containing the name of the factory function used to create
the Plugin.
\param[in] ScopeName String containing the name of the scope used to initialize the
Plugin.
\param[in] DeleteMode PluginDeletModeEnum specifying the delete mode to be used for the
Plugin.
\param[in] context Pointer to the runtime context.
\param[in] lib Pointer to the DynamicLibrary used to load the Plugin.

*/
dmz::PluginInfo::PluginInfo (
      const String &Name,
      const String &ClassName,
      const String &FactoryName,
      const String &ScopeName,
      const PluginDeleteModeEnum DeleteMode,
      RuntimeContext *context,
      DynamicLibrary *lib) :
      _state (*(new State (
         Name,
         ClassName,
         FactoryName,
         ScopeName,
         DeleteMode,
         context,
         lib))) {;} 


/*!

\brief Constructor.
\param[in] Name String containing the name of the Plugin.
\param[in] DeleteMode PluginDeletModeEnum specifying the delete mode to be used for the
Plugin.
\param[in] context Pointer to the runtime context.
\param[in] lib Pointer to the DynamicLibrary used to load the Plugin.

*/
dmz::PluginInfo::PluginInfo (
      const String &Name,
      const PluginDeleteModeEnum DeleteMode,
      RuntimeContext *context,
      DynamicLibrary *lib) :
      _state (*(new State (Name, "", "", "", DeleteMode, context, lib))) {;}


//! Destructor.
dmz::PluginInfo::~PluginInfo () { delete &_state; }


//! Returns String containing Plugin's instance name.
dmz::String
dmz::PluginInfo::get_name () const { return _state.Name; }


//! Returns Plugin's unique handle.
dmz::Handle
dmz::PluginInfo::get_handle () const { return _state.PluginHandle; }


//! Returns String containing Plugin's class name.
dmz::String
dmz::PluginInfo::get_class_name () const { return _state.ClassName; }


//! Returns String containing factory function used to create Plugin.
dmz::String
dmz::PluginInfo::get_factory_name () const { return _state.FactoryName; }


//! Returns String containing scope name used to initialize the Plugin/
dmz::String
dmz::PluginInfo::get_scope_name () const { return _state.ScopeName; }


//! Gets current delete mode.
dmz::PluginDeleteModeEnum
dmz::PluginInfo::get_delete_mode () const { return _state.deleteMode; }


//! Gets current runtime context.
dmz::RuntimeContext *
dmz::PluginInfo::get_context () const { return _state.context; }


//! Gets the current DynamicLibrary.
dmz::DynamicLibrary *
dmz::PluginInfo::get_dynamic_library () const { return _state.lib; }


//! Returns dmz::True if the plugin uses the specified level.
dmz::Boolean
dmz::PluginInfo::uses_level (const UInt32 Level) const {

   return _state.levelTable.lookup (Level) != 0;
}


//! Adds a level.
void
dmz::PluginInfo::add_level (const UInt32 Level) {

   if (Level) { _state.levelTable.store (Level, (void *)this); }
}


//! Returns the first level in the level list.
dmz::UInt32
dmz::PluginInfo::get_first_level () const {

   UInt32 result (1);
   _state.it.reset ();
   void *ptr (_state.levelTable.get_next (_state.it));
   if (ptr) { result = _state.it.get_hash_key (); }

   return result;
}


//! Returns the next level in the level list. Returns zero when list the end of
//! the list is reached.
dmz::UInt32
dmz::PluginInfo::get_next_level () const {

   UInt32 result (0);
   void *ptr (_state.levelTable.get_next (_state.it));
   if (ptr) { result = _state.it.get_hash_key (); }

   return result;
}

