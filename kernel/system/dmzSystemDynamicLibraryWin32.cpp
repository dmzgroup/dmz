#include <dmzSystemDynamicLibrary.h>
#include "dmzSystemLocalWin32.h"
#include <windows.h>

struct dmz::DynamicLibrary::State {

   const DynamicLibraryModeEnum LibMode;
   String name;
   HINSTANCE handle;
   String error;
   State (const DynamicLibraryModeEnum Mode) : LibMode (Mode), handle (0) {;}
};


dmz::DynamicLibrary::DynamicLibrary (
      const String &LibName,
      const DynamicLibraryModeEnum LibMode) : _state (*(new State (LibMode))) {

   _state.handle = LoadLibrary  (LibName.get_buffer ());
   _state.name = LibName;

   if (!_state.handle) {

      _state.error << get_last_win32_error () << "\n";

      String pluginName (LibName + ".dll");
      _state.handle = LoadLibrary (pluginName.get_buffer ());
      _state.name = pluginName;
   }

   if (!_state.handle) {

      _state.error << get_last_win32_error ();
      _state.name.empty ();
   }
   else { _state.error.empty (); }
}


dmz::DynamicLibrary::~DynamicLibrary () {

   if (_state.handle && (_state.LibMode == DynamicLibraryModeUnload)) {

      FreeLibrary (_state.handle); _state.handle = 0;
   }

   delete &_state;
}


void *
dmz::DynamicLibrary::get_function_ptr (const String &FunctionName) {

   void *functionPtr (0);

   if (_state.handle) {

      functionPtr = GetProcAddress (_state.handle, FunctionName.get_buffer ());
   }
   else {

      _state.error.flush () << "Could not find function: " << FunctionName
         << " in library: " << (!_state.name ? "Unknown" : _state.name);
   }

   return functionPtr;
}



dmz::Boolean
dmz::DynamicLibrary::is_loaded () { return _state.handle != 0; }


dmz::String
dmz::DynamicLibrary::get_name () { return _state.name; }


dmz::String
dmz::DynamicLibrary::get_error () { return _state.error; }

