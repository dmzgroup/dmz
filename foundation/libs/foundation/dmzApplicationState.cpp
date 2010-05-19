/*!

\enum dmz::ApplicationModeEnum
\ingroup Foundation
\brief Application mode enumerations.

\class dmz::ApplicationState
\ingroup Foundation
\brief Provides an interface to the application's state.

\fn dmz::ApplicationState::ApplicationState (const PluginInfo &Info)
\brief Constructor.
\param[in] Info Reference to the PluginInfo.

\fn dmz::ApplicationState::ApplicationState (RuntimeContext *context)
\brief Constructor.
\param[in] context Pointer to the runtime context.

\fn dmz::ApplicationState::~ApplicationState ()
\brief Destructor.

\fn dmz::String dmz::ApplicationState::get_app_name () const
\brief Gets the name of the application.
\return Returns a String containing the name of the Application.

\fn void dmz::ApplicationState::set_default_directory (const String &DirName)
\brief Sets default directory used for file open and save.
\param[in] DirName String containing the name of the default directory.

\fn dmz::String dmz::ApplicationState::get_default_directory () const
\brief Gets default directory used for file open and save.
\return Returns a String containing the name of the default directory.

\fn void dmz::ApplicationState::set_autosave_file (const String &FileName)
\brief Sets the name of the autosave file.
\param[in] FileName String containing the name of the file used in autosaves.

\fn dmz::String dmz::ApplicationState::get_autosave_file () const
\brief Gets the name of the autosave file.
\return Returns a String containing the name of the autosave file.

\fn void dmz::ApplicationState::push_mode (const ApplicationModeEnum Mode)
\brief Pushes the current application mode on to the stack.
\param[in] Mode Contains the value of the current application mode.

\fn dmz::ApplicationModeEnum dmz::ApplicationState::get_mode () const
\brief Gets the current application mode.
\return Returns the current application mode.

\fn dmz::ApplicationModeEnum dmz::ApplicationState::pop_mode ()
\brief Pops current application mode from the stack.
\return Returns the application mode after the current mode is popped off the stack.
It does \b not return the mode that was popped off the stack.

\fn dmz::Boolean dmz::ApplicationState::is_mode_normal ()
\brief Tests if application mode is normal.
\return Returns dmz::True if the application mode is normal.

*/
