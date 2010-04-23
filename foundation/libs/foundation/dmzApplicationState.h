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

   class ApplicationStateInterface {

      public:
         static ApplicationStateInterface *get_interface (RuntimeContext *context);

         virtual String get_app_name () const = 0;
         
         virtual void set_default_directory (const String &DirName) = 0;
         virtual String get_default_directory () const  = 0;

         virtual void set_autosave_file (const String &FileName) = 0;
         virtual String get_autosave_file () const = 0;

         virtual void push_mode (const ApplicationModeEnum Mode) = 0;
         virtual ApplicationModeEnum get_mode () const = 0;
         virtual ApplicationModeEnum pop_mode () = 0;

      protected:
         ApplicationStateInterface (RuntimeContext *context);
         virtual ~ApplicationStateInterface ();

      private:
         ApplicationStateInterface ();
         ApplicationStateInterface (const ApplicationStateInterface &);
         ApplicationStateInterface &operator= (const ApplicationStateInterface &);

         RuntimeContext *__context;
   };

   class ApplicationState {

      public:
         ApplicationState (const PluginInfo &Info);
         ApplicationState (RuntimeContext *context);
         ~ApplicationState ();

         String get_app_name () const;

         void set_default_directory (const String &DirName);
         String get_default_directory () const;

         void set_autosave_file (const String &FileName);
         String get_autosave_file () const;

         void push_mode (const ApplicationModeEnum Mode);
         ApplicationModeEnum get_mode () const;
         ApplicationModeEnum pop_mode ();
         Boolean is_mode_normal ();

      private:
         ApplicationState ();
         ApplicationState (const ApplicationState &);
         ApplicationState &operator= (const ApplicationState &);

         ApplicationStateInterface *__ptr;
   };
};


/*!

\brief Returns interface to the dmz::ApplicationState.
\param[in] context Pointer to runtime context.
\return Returns a pointer to the ApplicationState. Will return NULL if a
ApplicationState has not been created.

*/
inline dmz::ApplicationStateInterface *
dmz::ApplicationStateInterface::get_interface (RuntimeContext *context) {

   return (ApplicationStateInterface *)lookup_rtti_named_interface (
      ApplicationStateInterfaceName,
      context);
}



//! Constructor.
inline
dmz::ApplicationStateInterface::ApplicationStateInterface (RuntimeContext *context) :
      __context (context) {

   store_rtti_named_interface (ApplicationStateInterfaceName, __context, (void *)this);
}


//! Destructor.
inline
dmz::ApplicationStateInterface::~ApplicationStateInterface () {

   remove_rtti_named_interface (ApplicationStateInterfaceName, __context, (void *)this);
   __context = 0;
}


inline
dmz::ApplicationState::ApplicationState (const PluginInfo &Info) :
      __ptr (ApplicationStateInterface::get_interface (Info.get_context ())) {

}


inline
dmz::ApplicationState::ApplicationState (RuntimeContext *context) :
      __ptr (ApplicationStateInterface::get_interface (context)) {

}


inline
dmz::ApplicationState::~ApplicationState () { __ptr = 0; }


inline dmz::String
dmz::ApplicationState::get_app_name () const {

   return __ptr ? __ptr->get_app_name () : "dmz";
}


inline void
dmz::ApplicationState::set_default_directory (const String &DirName) {

   if (__ptr) { __ptr->set_default_directory (DirName); }
}


inline dmz::String
dmz::ApplicationState::get_default_directory () const {

   return __ptr ? __ptr->get_default_directory () : "";
}


inline void
dmz::ApplicationState::set_autosave_file (const String &FileName) {

   if (__ptr) { __ptr->set_autosave_file (FileName); }
}


inline dmz::String
dmz::ApplicationState::get_autosave_file () const {

   return __ptr ? __ptr->get_autosave_file () : "";
}


inline void
dmz::ApplicationState::push_mode (const ApplicationModeEnum Mode) {

   if (__ptr) { __ptr->push_mode (Mode); }
}


inline dmz::ApplicationModeEnum
dmz::ApplicationState::get_mode () const {

   return __ptr ? __ptr->get_mode () : ApplicationModeNormal;
}


inline dmz::ApplicationModeEnum
dmz::ApplicationState::pop_mode () {

   return __ptr ? __ptr->pop_mode () : ApplicationModeNormal;
}


inline dmz::Boolean
dmz::ApplicationState::is_mode_normal () {

   return __ptr ? __ptr->get_mode () == ApplicationModeNormal : True;
}

#endif // DMZ_APPLICATION_STATE_DOT_H
