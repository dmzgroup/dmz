/*!

\defgroup Audio DMZ Audio Framework

\class dmz::AudioModule
\ingroup Audio
\brief Audio module Interface.
\details The audio module provides an abstract interface to the audio subsystem being
used. It is intended to play single shot temporal sounds as well as looped persistent
sounds. It is not intended to be used for streaming audio or audio input.
\sa dmz::AudioModuleFMOD

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
\brief Loads a sound file.
\param[in] FileName String containing the name of the audio file to load.
\return Returns a Handle to the loaded audio file that may be used to create playing
instances of the loaded sound. Returns zero if the file could not be loaded.

\fn dmz::Boolean dmz::AudioModule::destroy_audio_handle (const dmz::Handle AudioHandle)
\brief Unloads the loaded audio file.
\note If there are any existing instance of this audio file at the time the handle is
destroyed, the resource will not actually released until all the instance have been freed.
\param[in] AudioHandle Handle of the loaded audio file to be freed.
\return Returns dmz::True if the resource is successfully unreferenced. A result of
dmz::True does not necessarily mean the resource has been freed.

\fn dmz::Handle dmz::AudioModule::play_sound (
const dmz::Handle AudioHandle, 
const dmz::SoundAttributes &Attributes)
\brief Creates an instance of a sound file and plays it.
\param[in] AudioHandle Handle of the audio file to play.
\param[in] Attributes Specifies what attributes to apply to the sound instance such as
position and pitch of the sound and whether or not to loop the sound.
\return Returns a handle to the instance of the sound. This Handle may be used to update
or stop the audio instance while it is playing.
\note The instance if released as soon as the sound stops playing. If the sound is looped,
the sound must be stop in order for the instance to be freed.
\sa dmz::AudioModule::stop_sound() \n dmz::SoundAttributes

\fn dmz::Boolean dmz::AudioModule::update_sound (
const dmz::Handle InstanceHandle, 
const dmz::SoundAttributes &Attributes)
\brief Updates a sound instance's attributes.
\param[in] InstanceHandle Handle of the sound instance being updated.
\param[in] Attributes New attribute values of the sound instance.
\return Returns dmz::True if the sound instance was successfully updated.

\fn dmz::Boolean dmz::AudioModule::lookup_sound (
const dmz::Handle InstanceHandle, 
dmz::SoundAttributes &attributes)
\brief Looks up a sound instance's current attributes.
\param[in] InstanceHandle Handle of the sound instance to retrieve the attributes.
\param[out] attributes Used to return the sound instance's attributes.
\return Returns dmz::True if the attributes for the specified sound instance can be found.

\fn dmz::Boolean dmz::AudioModule::stop_sound (const dmz::Handle InstanceHandle)
\brief Stops an audio instance.
\details Function can be used with both looped and single play sounds.
\param[in] InstanceHandle Handle of the sound instance to stop.
\return Returns dmz::True if the sound instance is successfully stopped.

\fn dmz::Boolean dmz::AudioModule::set_mute_all_state (const dmz::Boolean Mute)
\brief Mutes and un-mutes all currently playing audio.
\param[in] Mute Specifies the mutes state.
\return Returns dmz::True if the new state was successfully set.

\fn dmz::Boolean dmz::AudioModule::get_mute_all_state (dmz::Boolean &mute)
\brief Gets current mute state.
\param[out] mute Value of the mute state.
\return Returns dmz::True if the mute state was successfully returned.

\fn dmz::Handle dmz::AudioModule::create_listener (const dmz::String &Name)
\brief Creates a listener.
\details A listener is an ear point into the audio environment.
\param[in] Name String containing the name of the listener.
\return Returns a handle to the listener. Returns zero if the listener is not created.
\note Currently only one listener is supported.

\fn dmz::Handle dmz::AudioModule::lookup_listener (const dmz::String &Name)
\brief Gets Handle for given listener name.
\param[in] Name String containing name of the listener.
\return Returns the handle of the named listener. Returns zero if no listener with
the give name is found.

\fn dmz::Boolean dmz::AudioModule::set_listener (
const dmz::Handle ListenerHandle,
const dmz::Vector &Position,
const dmz::Matrix &Orientation)
\brief Sets the position and orientation of the given listener.
\param[in] ListenerHandle Handle of the listener to update.
\param[in] Position Vector containing the new position of the listener.
\param[in] Orientation Matrix containing the new orientation of the listener.
\return Returns dmz::True if the listener was updated.

\fn dmz::Boolean dmz::AudioModule::get_listener (
const dmz::Handle ListenerHandle,
dmz::Vector &position,
dmz::Matrix &orientation)
\brief Looks up the given listener's position and orientation.
\param[in] ListenerHandle Handle of the listener to update.
\param[out] position Vector to store the current listener position.
\param[out] orientation Matrix to store the current listener orientation.
\return Returns dmz::True if the position and orientation of the given listener was
found.

\fn dmz::Boolean dmz::AudioModule::destroy_listener (const dmz::Handle ListenerHandle)
\brief Release the listener with the given Handle.
\param[in] ListenerHandle Handle of the listener to release.
\return Returns dmz::True if the specified listener was released.

*/
