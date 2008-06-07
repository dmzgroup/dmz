/*!

\defgroup Input DMZ Input Framework

\class dmz::InputModule
\ingroup Input
\brief Input channels module interface.

\fn dmz::InputModule *dmz::InputModule::cast (const Plugin *PluginPtr, const String &PluginName)
\brief Casts Plugin pointer to an InputModule
\details If the Plugin object implements the InputModule interface, a pointer
to the InputModule interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin.
\param[in] PluginName String containing name of desired InputModule.
\return Returns a ointer to the InputModule. Returns NULL if the Plugin does not
implement the InputModule interface or the \a PluginName is not empty and not
equal to the Plugin's name.

\fn dmz::InputModule::InputModule (const PluginInfo &Info)
\brief Constructor.
\param[in] Info PluginInfo containing initialization data.

\fn dmz::InputModule::~InputModule ()
\brief Destructor.

\fn void dmz::InputModule::register_input_observer (
const Handle Channel,
const Mask &EventMask,
InputObserver &observer)
\brief Registers input observer.
\param[in] Channel Channel Handle.
\param[in] EventMask Mask containing the events desired for subscription.
\param[in] observer InputObserver to register.

\fn void dmz::InputModule::release_input_observer (
const Handle Channel,
const Mask &EventMask,
InputObserver &observer)
\brief Releases input observer.
\param[in] Channel Channel Handle.
\param[in] EventMask Mask containing the events desired for unsubscription.
\param[in] observer InputObserver to release.

\fn Boolean dmz::InputModule::create_channel (const Handle Channel)
\brief Create channel.
\param[in] Channel Named handle of channel to create.
\return Returns dmz::True if the channel is created.

\fn Boolean dmz::InputModule::set_channel_state (
const Handle Channel,
const Boolean Value)
\brief Sets channel state.
\param[in] Channel Channel Handle.
\param[in] Value dmz::True turns on channel, dmz::False turns it off.
\return Returns dmz::True if the channel state was successfully changed.

\fn Boolean dmz::InputModule::get_channel_state (const Handle Channel) const
\brief Gets channel state.
\param[in] Channel Channel Handle.
\return Returns dmz::True if the channel is on and dmz::False if if the channel if off.

\fn void dmz::InputModule::send_axis_event (const InputEventAxis &Event)
\brief Sends axis event.
\param[in] Event InputEventAxis to send.

\fn void dmz::InputModule::send_button_event (const InputEventButton &Event)
\brief Sends button event.
\param[in] Event InputEventButton to send.

\fn void dmz::InputModule::send_switch_event (const InputEventSwitch &Event)
\brief Sends switch event.
\param[in] Event InputEventSwicth to send.

\fn void dmz::InputModule::send_key_event (const InputEventKey &Event)
\brief Sends key event.
\param[in] Event InputEventKey to send.

\fn void dmz::InputModule::send_mouse_event (const InputEventMouse &Event)
\brief Sends mouse event.
\param[in] Event InputEventMouse to send.

\fn void dmz::InputModule::send_data_event (const Handle Source, const Data &Event)
\brief Sends data event.
\param[in] Source Handle of the source of the data event.
\param[in] Event Data to send.

*/
