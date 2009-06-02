/*!

\class dmz::EventObserver
\ingroup Event
\brief Event observer interface.

\fn dmz::EventObserver *dmz::EventObserver::cast (
const Plugin *PluginPtr,
const String &PluginName)
\brief Casts Plugin pointer to an EventObserver pointer.
\details If the Plugin object implements the EventObserver interface,
a pointer to the EventObserver interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired EventObserver.
\return Returns pointer to the EventObserver. Return NULL if the PluginPtr does not
implement the EventObserver interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::Boolean dmz::EventObserver::is_valid (
const Handle ObserverHandle,
RuntimeContext *context)
\brief Tests if runtime handle belongs to a valid event observer interface.
\param[in] ObserverHandle Unique runtime handle.
\param[in] context Pointer to the runtime context.
\return Returns dmz::True if the runtime handle is valid and the Plugin supports the
event observer interface.

\fn dmz::EventObserver::EventObserver (const PluginInfo &Info)
\brief Constructor.
\param[in] Info PluginInfo containing initialization data.

\fn dmz::EventObserver::~EventObserver ()
\brief Destructor.

\fn dmz::String dmz::EventObserver::get_event_observer_name ()
\brief Gets event observer's name.
\return Returns unique runtime handle.

\fn dmz::Handle dmz::EventObserver::get_event_observer_handle ()
\brief Gets event observer's unique runtime handle.
\return Returns unique runtime handle.

\fn void dmz::EventObserver::store_event_module (const String &Name, EventModule &module)
\brief Stores event module in observer.
\param[in] Name String containing name of the module.
\param[in] module EventModule reference.

\fn void dmz::EventObserver::remove_event_module (const String &Name, EventModule &module)
\brief Removes event module in observer.
\param[in] Name String containing name of the module.
\param[in] module EventModule reference.

\fn void dmz::EventObserver::create_event (
const Handle EventHandle,
const EventType &Type,
const EventLocalityEnum Locality)
\brief Event creation callback.
\param[in] EventHandle Event's unique runtime handle,
\param[in] Type Event's EventType.
\param[in] Locality Event's locality.

\fn void dmz::EventObserver::close_event (
const Handle EventHandle,
const EventType &Type,
const EventLocalityEnum Locality)
\brief Event close callback.
\param[in] EventHandle Event's unique runtime handle,
\param[in] Type Event's EventType.
\param[in] Locality Event's locality.

*/
