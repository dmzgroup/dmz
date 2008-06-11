/*!

\defgroup Audio DMZ Audio Framework

\class dmz::AudioModule
\ingroup Audio
\brief Audio module Interface.

\fn dmz::AudioModule *dmz::AudioModule::cast (
const dmz::Plugin *PluginPtr,
const dmz::String &PluginName)
\brief Casts Plugin pointer to an AudioModule.
\details If the Plugin object implements the AudioModule interface, a pointer to
the AudioModule interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired AudioModule
\return Returns pointer to the AudioModule. Return NULL if the PluginPtr does not
implement the AudioModule interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::AudioModule::AudioModule (const dmz::PluginInfo &Info)
\brief Constructor.

\fn dmz::AudioModule::~AudioModule ()
\brief Destructor.

\fn dmz::Handle dmz::AudioModule::create_audio_handle (const dmz::String &FileName)

\fn dmz::Boolean dmz::AudioModule::destroy_audio_handle (const dmz::Handle AudioHandle)

\fn dmz::Handle dmz::AudioModule::play_sound (
const dmz::Handle AudioHandle, 
const dmz::SoundAttributes &Attributes)

\fn dmz::Boolean dmz::AudioModule::update_sound (
const dmz::Handle InstanceHandle, 
const dmz::SoundAttributes &Attributes)

\fn dmz::Boolean dmz::AudioModule::lookup_sound (
const dmz::Handle InstanceHandle, 
dmz::SoundAttributes &attributes)

\fn dmz::Boolean dmz::AudioModule::stop_sound (const dmz::Handle InstanceHandle)

\fn dmz::Boolean dmz::AudioModule::set_mute_all_state (const dmz::Boolean Mute)

\fn dmz::Boolean dmz::AudioModule::get_mute_all_state (dmz::Boolean &mute)

\fn dmz::Handle dmz::AudioModule::create_listener (const dmz::String &Name)

\fn dmz::Handle dmz::AudioModule::lookup_listener (const dmz::String &Name)

\fn dmz::Boolean dmz::AudioModule::set_listener (
const dmz::Handle Handle,
const dmz::Vector &Position,
const dmz::Matrix &Orientation)

\fn dmz::Boolean dmz::AudioModule::get_listener (
const dmz::Handle Handle,
dmz::Vector &position,
dmz::Matrix &orientation)

\fn dmz::Boolean dmz::AudioModule::destroy_listener (const dmz::Handle Handle)

*/
