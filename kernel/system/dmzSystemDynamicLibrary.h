#ifndef DMZ_SYSTEM_DYNAMIC_LIBRARY_DOT_H
#define DMZ_SYSTEM_DYNAMIC_LIBRARY_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class Stream;

   enum DynamicLibraryModeEnum {
      DynamicLibraryModeUnload, //!< Unload dynamic library when object destroyed.
      DynamicLibraryModeKeep, //!< Do not unload dynamic library when object destroyed.
   };

   class DMZ_KERNEL_LINK_SYMBOL DynamicLibrary {

      public:
         static void dump_loaded (Stream &out);

         DynamicLibrary (const String &LibName, const DynamicLibraryModeEnum Mode);
         ~DynamicLibrary ();

         void *get_function_ptr (const String &FunctionName);

         Boolean is_loaded ();
         String get_name ();
         String get_error ();

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         DynamicLibrary ();
         DynamicLibrary (const DynamicLibrary &);
         DynamicLibrary operator= (const DynamicLibrary &);
   };
};

#endif // DMZ_SYSTEM_DYNAMIC_LIBRARY_DOT_H
