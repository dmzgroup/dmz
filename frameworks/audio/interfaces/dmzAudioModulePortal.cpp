/*!

\class dmz::AudioModulePortal
\ingroup Audio
\brief Audio portal interface.
\sa dmz::AudioModulePortalBasic

\fn dmz::AudioModulePortal *dmz::AudioModulePortal::cast (
const dmz::Plugin *PluginPtr,
const dmz::String &PluginName);
\brief Casts Plugin pointer to an AudioModulePortal.
\details If the Plugin object implements the AudioModulePortal interface, a pointer to
the AudioModulePortal interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired AudioModule
\return Returns pointer to the AudioModulePortal. Return NULL if the PluginPtr does not
implement the AudioModulePortal interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::AudioModulePortal::AudioModulePortal (const dmz::PluginInfo &Info);
\brief Construct.

\fn dmz::AudioModulePortal::~AudioModulePortal ();
\brief Destructor.

\fn dmz::String dmz::AudioModulePortal::get_audio_portal_name () const;
\brief Gets audio module portal's name.
\return Returns a String containing the audio module portal's name.

\fn dmz::Handle dmz::AudioModulePortal::get_audio_portal_handle () const;
\brief Gets audio module portal's Handle.
\return Returns the audio module portal's handle.

\fn dmz::Boolean dmz::AudioModulePortal::is_master_portal () const
\brief Indicates if portal is the master portal.
\return Returns dmz::True if the portal is the master audio portal.

\fn void dmz::AudioModulePortal::set_view (const dmz::Vector &Position, const dmz::Matrix &Orientation, const dmz::Vector &Velocity)
\brief Sets portal view.
\param[in] Position Vector containing the translational transform of the portal.
\param[in] Orientation Matrix containing the rotation transform of the portal.
\param[in] Velocity Vector containing the velocity of the portal.

\fn void dmz::AudioModulePortal::get_view (dmz::Vector &position, dmz::Matrix &orientation, dmz::Vector &velocity) const
\brief Gets portal view.
\param[out] position Vector to store the translational transform of the portal.
\param[out] orientation Matrix to store the rotation transform of the portal.
\param[out] velocity Vector to store the velocity of the portal.

\fn void dmz::AudioModulePortal::set_mute_state (const dmz::Boolean Value)
\brief Sets portal mute state.
\param[in] Value Boolean containing the mute state. If \b Value is dmz::True then
the portal is muted.

\fn dmz::Boolean dmz::AudioModulePortal::get_mute_state () const
\brief Gets current mute state.
\return Returns dmz::True if the portal is currently muted.

*/
