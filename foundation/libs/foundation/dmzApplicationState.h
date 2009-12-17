#ifndef DMZ_APPLICATION_STATE_DOT_H
#define DMZ_APPLICATION_STATE_DOT_H

#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeRTTINamed.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   //! ApplicationState interface name.
   const char ApplicationStateInterfaceName[] = "ApplicationStateInterface";

   enum ApplicationModeEnum {

      ApplicationModeNormal,
      ApplicationModeSaving,
      ApplicationModeLoading,
      ApplicationModeUndoing,
      ApplicationModeUnknown,
   };

   class ApplicationState {

      public:
         static ApplicationState *get_interface (RuntimeContext *context);

         virtual void set_default_directory (const String &DirName) = 0;
         virtual String get_default_directory () const  = 0;

         virtual void set_autosave_file (const String &FileName) = 0;
         virtual String get_autosave_file () const = 0;

         virtual void push_mode (const ApplicationModeEnum Mode) = 0;
         virtual ApplicationModeEnum get_mode () const = 0;
         virtual ApplicationModeEnum pop_mode () = 0;

      protected:
         ApplicationState (RuntimeContext *context);
         virtual ~ApplicationState ();

      private:
         ApplicationState ();
         ApplicationState (const ApplicationState &);
         ApplicationState &operator= (const ApplicationState &);

         RuntimeContext *__context;
   };

   class ApplicationStateWrapper {

      public:
         ApplicationStateWrapper (const PluginInfo &Info);
         ApplicationStateWrapper (RuntimeContext *context);
         ~ApplicationStateWrapper ();

         void set_default_directory (const String &DirName);
         String get_default_directory () const;

         void set_autosave_file (const String &FileName);
         String get_autosave_file () const;

         void push_mode (const ApplicationModeEnum Mode);
         ApplicationModeEnum get_mode () const;
         ApplicationModeEnum pop_mode ();
         Boolean is_mode_normal ();

      private:
         ApplicationStateWrapper ();
         ApplicationStateWrapper (const ApplicationStateWrapper &);
         ApplicationStateWrapper &operator= (const ApplicationStateWrapper &);

         ApplicationState *__ptr;
   };
};


/*!

\brief Returns interface to the dmz::ApplicationState.
\param[in] context Pointer to runtime context.
\return Returns a pointer to the ApplicationState. Will return NULL if a
ApplicationState has not been created.

*/
inline dmz::ApplicationState *
dmz::ApplicationState::get_interface (RuntimeContext *context) {

   return (ApplicationState *)lookup_rtti_named_interface (
      ApplicationStateInterfaceName,
      context);
}



//! Constructor.
inline
dmz::ApplicationState::ApplicationState (RuntimeContext *context) : __context (context) {

   store_rtti_named_interface (ApplicationStateInterfaceName, __context, (void *)this);
}


//! Destructor.
inline
dmz::ApplicationState::~ApplicationState () {

   remove_rtti_named_interface (ApplicationStateInterfaceName, __context, (void *)this);
   __context = 0;
}


inline
dmz::ApplicationStateWrapper::ApplicationStateWrapper (const PluginInfo &Info) :
      __ptr (ApplicationState::get_interface (Info.get_context ())) {

}


inline
dmz::ApplicationStateWrapper::ApplicationStateWrapper (RuntimeContext *context) :
      __ptr (ApplicationState::get_interface (context)) {

}


inline
dmz::ApplicationStateWrapper::~ApplicationStateWrapper () { __ptr = 0; }


inline void
dmz::ApplicationStateWrapper::set_default_directory (const String &DirName) {

   if (__ptr) { __ptr->set_default_directory (DirName); }
}


inline dmz::String
dmz::ApplicationStateWrapper::get_default_directory () const {

   return __ptr ? __ptr->get_default_directory () : "";
}


inline void
dmz::ApplicationStateWrapper::set_autosave_file (const String &FileName) {

   if (__ptr) { __ptr->set_autosave_file (FileName); }
}


inline dmz::String
dmz::ApplicationStateWrapper::get_autosave_file () const {

   return __ptr ? __ptr->get_autosave_file () : "";
}


inline void
dmz::ApplicationStateWrapper::push_mode (const ApplicationModeEnum Mode) {

   if (__ptr) { __ptr->push_mode (Mode); }
}


inline dmz::ApplicationModeEnum
dmz::ApplicationStateWrapper::get_mode () const {

   return __ptr ? __ptr->get_mode () : ApplicationModeNormal;
}


inline dmz::ApplicationModeEnum
dmz::ApplicationStateWrapper::pop_mode () {

   return __ptr ? __ptr->pop_mode () : ApplicationModeNormal;
}


inline dmz::Boolean
dmz::ApplicationStateWrapper::is_mode_normal () {

   return __ptr ? __ptr->get_mode () == ApplicationModeNormal : True;
}

#endif // DMZ_APPLICATION_STATE_DOT_H
