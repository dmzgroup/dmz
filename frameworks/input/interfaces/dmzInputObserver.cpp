/*!

\class dmz::InputObserver
\ingroup Input
\brief Observers input events.
\details A pure virtual interface for observing input events. This class is only an
interface. Derived classes should use the dmz::InputObserverUtil class.
\sa dmz::InputObserverUtil

\fn dmz::InputObserver::InputObserver (const PluginInfo &Info)
\brief Constructor.
\param[in] Info PluginInfo containing initialization data.

\fn dmz::InputObserver::~InputObserver ()
\brief Destructor.

\fn dmz::InputObserver::InputObserver *dmz::InputObserver::cast (const Plugin *PluginPtr)
\brief Casts Plugin pointer to an InputObserver pointer.
\details If the Plugin object implements the InputObserver interface, apointer to the
InputObserver interface of the Plugin is returned.
\return Resturns a pointer to the InputObserver. Returns NULL if the PluginPtr does not
implement the InputObserver interface.

\fn dmz::Boolean dmz::InputObserver::is_valid (
const Handle ObserverHandle,
RuntimeContext *context)
\brief Tests if runtime handle belongs to a valid input observer interface.
\param[in] ObserverHandle Unique runtime handle.
\param[in] context Pointer to the runtime context.
\return Returns dmz::True if the runtime handle is valid and the associated Plugin
supports the input observer interface.

\fn dmz::Handle dmz::InputObserver::get_input_observer_handle ()
\brief Gets input observers unique runtime handle.
\return Returns unique runtime handle.

\fn dmz::Handle dmz::InputObserver::get_input_observer_name ()
\brief Gets input observers name.
\return Returns input observer name.

\fn void dmz::InputObserver::store_input_module_channels (
const String &Name,
InputModule &module)
\brief Stores input channels module in observer.
\param[in] Name String containing name of module.
\param[in] module InputModule reference.

\fn void dmz::InputObserver::remove_input_module_channels (
const String &Name,
InputModule &module)
\brief Removes input channels module from observer.
\param[in] Name String containing name of module.
\param[in] module InputModule reference.

\fn void dmz::InputObserver::update_channel_state (
const Handle Channel,
const Boolean State)
\brief Update state of a subscribed channel.
\param[in] Channel Handle of the channel.
\param[in] State Boolean indicating current channel state. True indicates the channel
is active. False indicates the channel has been deactivated.

\fn void dmz::InputObserver::receive_axis_event (
const Handle Channel,
const InputEventAxis &Value)
\brief Receives an axis event from a controller.
\param[in] Channel Handle indicating the channel used to send the event.
\param[in] Value Controller axis event.

\fn void dmz::InputObserver::receive_button_event (
const Handle Channel,
const InputEventButton &Value)
\brief Receives a button event from a controller.
\param[in] Channel Handle indicating the channel used to send the event.
\param[in] Value Controller button event.

\fn void dmz::InputObserver::receive_switch_event (
const Handle Channel,
const InputEventSwitch &Value)
\brief Receives a switch event from a controller.
\param[in] Channel Handle indicating the channel used to send the event.
\param[in] Value Controller switch event.

\fn void dmz::InputObserver::receive_key_event (
const Handle Channel,
const InputEventKey &Value)
\brief Receives a key press event.
\param[in] Channel Handle indicating the channel used to send the event.
\param[in] Value Key press event.

\fn void dmz::InputObserver::receive_mouse_event (
const Handle Channel,
const InputEventMouse &Value)
\brief Receives a mouse event.
\param[in] Channel Handle indicating the channel used to send the event.
\param[in] Value Mouse event.

\fn void dmz::InputObserver::receive_data_event (
const Handle Channel,
const Handle Source,
const Data &Value)
\brief Receives a Data event.
\param[in] Channel Handle indicating the channel used to send the event.
\param[in] Source Handle indicating the source of the event.
\param[in] Value Data event.

*/
