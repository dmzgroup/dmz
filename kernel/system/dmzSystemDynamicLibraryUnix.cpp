#include <dmzSystemDynamicLibrary.h>
#include <dlfcn.h>

#if defined(__APPLE__) || defined(MACOSX)
#include <TargetConditionals.h>
#   ifndef TARGET_OS_IPHONE
#      include <CoreServices/CoreServices.h>
#   endif
#endif


/*!

\file dmzSystemDynamicLibrary.h
\brief Contains class for loading and manipulating dynamic libraries.

\enum dmz::DynamicLibraryModeEnum
\ingroup System
\brief Library clean up enumeration.
\details Defined in dmzSystemDynamicLibrary.h.

\class dmz::DynamicLibrary
\ingroup System
\brief Loads a dynamic library.
\details DynamicLibrary loads a dynamic library and provides a method for returning a
pointer to functions contained within the dynamic library.

*/

struct dmz::DynamicLibrary::State {

   const DynamicLibraryModeEnum LibMode;
   String name;
   void *handle;
   String error;
   State (const DynamicLibraryModeEnum Mode) : LibMode (Mode), handle (0) {;}
};


/*!

\brief Dynamic Library Constructor.
\details On a Unix platforms (Linux and MacOS) the \a LibName is processed as follows:
- Attempt to load library using \a LibName unmodified.
- Attempt to load library using \a LibName with the ".plugin" suffix.
- Attempt to load library using \a LibName with  the "lib" prefix and the ".so" suffix.
- Attempt to load library using \a LibName with the ".so" suffix.
- (MacOS \b ONLY) Attempt to load library using \a LibName with "lib" prefix
and ".dylib" suffix.

On Win32 platforms the \a LibName is processed as follows:
- Attempt to load library using \a LibName unmodified.
- Attempt to load library using \a LibName with the ".dll" suffix added to the end.

\param[in] LibName String containing name of Dynamic Library to load.
\param[in] LibMode dmz::DynamicLibraryModeEnum specifies how Dynamic Library should be
handled when destructor is called.

*/
dmz::DynamicLibrary::DynamicLibrary (
      const String &LibName,
      const DynamicLibraryModeEnum LibMode) : _state (*(new State (LibMode))) {

#if defined(__APPLE__) || defined(MACOSX)
   const int Mode (RTLD_LOCAL | RTLD_LAZY);
#else
   const int Mode (RTLD_GLOBAL | RTLD_LAZY);
#endif

   _state.handle = dlopen (LibName.get_buffer (), Mode);
   _state.name = LibName;

#if defined(__APPLE__) || defined(MACOSX)
#   ifndef TARGET_OS_IPHONE
   if (!_state.handle) {

      char *error = dlerror ();
      if (error) { _state.error << error << "\n"; }

      CFURLRef pluginRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());

      CFStringRef macPath =
         CFURLCopyFileSystemPath(pluginRef, kCFURLPOSIXPathStyle);

      String soName;
      soName << CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding());

      soName << "/Contents/Frameworks/";
      soName << LibName << ".plugin";

      _state.handle = dlopen (soName.get_buffer (), Mode);
      _state.name = soName;
   }
#   endif
#endif

   if (!_state.handle) {

      char *error = dlerror ();
      if (error) { _state.error << error << "\n"; }

      String pluginName (LibName);
      pluginName << ".plugin";
      _state.handle = dlopen (pluginName.get_buffer (), Mode);
      _state.name = pluginName;
   }

   if (!_state.handle) {

      char *error = dlerror ();
      if (error) { _state.error << error << "\n"; }

      String soName ("lib");
      soName << LibName << ".so";
      _state.handle = dlopen (soName.get_buffer (), Mode);
      _state.name = soName;
   }

   if (!_state.handle) {

      char *error = dlerror ();
      if (error) { _state.error << error << "\n"; }

      String soName (LibName);
      soName << ".so";
      _state.handle = dlopen (soName.get_buffer (), Mode);
      _state.name = soName;
   }

#if defined(__APPLE__) || defined(MACOSX)
#   ifndef TARGET_OS_IPHONE
   if (!_state.handle) {

      char *error = dlerror ();
      if (error) { _state.error << error << "\n"; }

      String dylibName ("lib");
      dylibName << LibName << ".dylib";
      _state.handle = dlopen (dylibName.get_buffer (), Mode);
      _state.name = dylibName;
   }
#   endif
#endif

   if (!_state.handle) {

      char *error = dlerror ();

      if (error) { _state.error << error; _state.name.empty (); }
   }
   else { _state.error.empty (); }
}


/*!

\brief Destructor.
\details If the dmz::DynamicLibraryModeEnum was set to dmz::DynamicLibraryModeUnload
then the library is unloaded at destruction. If it was set to
dmz::DynamicLibraryModeKeep then the library is left loaded at destruction time.

*/
dmz::DynamicLibrary::~DynamicLibrary () {

   if (_state.handle && (_state.LibMode == DynamicLibraryModeUnload)) {

      dlclose (_state.handle); _state.handle = 0;
   }

   delete &_state;
}


/*!

\brief Returns address to function contained in the dynamic library.
\param[in] FunctionName String containing name of the function.
\return Returns a void pointer which is the address of the function. Returns NULL if
the function is not found.

*/
void *
dmz::DynamicLibrary::get_function_ptr (const String &FunctionName) {

   void *functionPtr (0);

   if (_state.handle) {

      functionPtr = dlsym (_state.handle, FunctionName.get_buffer ());
   }
   else {

      _state.error.flush () << "Could not find function: " << FunctionName
         << " in library: " << (!_state.name ? "Unknown" : _state.name);
   }

   return functionPtr;
}


//! Returns dmz::True if dynamic library is successfully loaded.
dmz::Boolean
dmz::DynamicLibrary::is_loaded () { return _state.handle != 0; }


//! Returns String containing name of loaded dynamic library.
dmz::String
dmz::DynamicLibrary::get_name () { return _state.name; }


//! Returns String containing last error.
dmz::String
dmz::DynamicLibrary::get_error () { return _state.error; }

