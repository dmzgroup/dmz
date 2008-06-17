/*!

\class dmz::AudioModulePortal
\ingroup Audio
\brief Audio portal interface.
\sa dmz::AudioModulePortalBasic

\fn dmz::AudioModulePortal *dmz::AudioModulePortal::cast (
const dmz::Plugin *PluginPtr,
const dmz::String &PluginName);

\fn dmz::AudioModulePortal::AudioModulePortal (const dmz::PluginInfo &Info);
\fn dmz::AudioModulePortal::~AudioModulePortal ();
\fn dmz::String dmz::AudioModulePortal::get_audio_portal_name () const;
\fn dmz::Handle dmz::AudioModulePortal::get_audio_portal_handle () const;
\fn dmz::Boolean dmz::AudioModulePortal::is_master_portal () const
\fn void dmz::AudioModulePortal::set_view (const dmz::Vector &Pos, const dmz::Matrix &Ori)
\fn void dmz::AudioModulePortal::get_view (dmz::Vector &pos, dmz::Matrix &ori) const
\fn void dmz::AudioModulePortal::set_mute_state (const dmz::Boolean Value)
\fn dmz::Boolean dmz::AudioModulePortal::get_mute_state () const

*/
