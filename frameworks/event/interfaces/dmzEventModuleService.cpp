/*!

\class dmz::EventModuleService
\ingroup Event
\brief Provides an interface for creating service events.

\fn dmz::EventModuleService *dmz::EventModuleService::cast (
const Plugin *PluginPtr,
const String &PluginName)
brief Casts Plugin pointer to an EventModuleService.
\details If the Plugin object implements the EventModuleService interface, a pointer to
the EventModuleService interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired EventModuleService.
\return Returns pointer to the EventModuleService. Returns NULL if the PluginPtr does not
implement the EventModuleService interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::EventModuleService::EventModuleService (const PluginInfo &Info)
\brief Constructor.
\param[in] Info PluginInfo containing initialization data.

\fn dmz::EventModuleService::~EventModuleService ()
\brief Destructor

\fn dmz::String dmz::EventModuleService::get_event_module_service_name () const
\brief Gets the service event module's name.

\fn dmz::Handle dmz::EventModuleService::get_event_module_service_handle () const
\brief Gets the service event module's Handle.

\fn dmz::Handle dmz::EventModuleService::create_open_service_request_event (
const Handle Source,
const Handle Target,
const EventLocalityEnum Locality)
\brief Creates an open service request event.
\param[in] Source Handle of the source object of the event.
\param[in] Target Handle of the target object of the event.
\param[in] Locality dmz::EventLocalityEnum specifying if the event was created locally
or remotely.
\return Returns the Handle of the created event. A zero Handle is returned if the event
creation fails.

\fn dmz::Handle dmz::EventModuleService::create_open_resupply_offer_event (
const Handle Source,
const Handle Target,
const EventLocalityEnum Locality)
\brief Creates an open service resupply offer event.
\param[in] Source Handle of the source object of the event.
\param[in] Target Handle of the target object of the event.
\param[in] Locality dmz::EventLocalityEnum specifying if the event was created locally
or remotely.
\return Returns the Handle of the created event. A zero Handle is returned if the event
creation fails.

\fn dmz::Handle dmz::EventModuleService::create_open_resupply_received_event (
const Handle Source,
const Handle Target,
const EventLocalityEnum Locality)
\brief Creates an open service resupply received event.
\param[in] Source Handle of the source object of the event.
\param[in] Target Handle of the target object of the event.
\param[in] Locality dmz::EventLocalityEnum specifying if the event was created locally
or remotely.
\return Returns the Handle of the created event. A zero Handle is returned if the event
creation fails.

\fn dmz::Boolean dmz::EventModuleService::close_service_event (const Handle EventHandle)
\brief Closes an open event.
\param[in] EventHandle Handle of the event to close
\return Returns dmz::True if the event was closed or scheduled to be closed.
\sa dmz::EventModule::close_event()

\fn dmz::Handle dmz::EventModuleService::add_supplies_to_event (
const Handle EventHandle,
const ObjectType &Type,
const UInt32 Quantity)
\brief Adds supplies to the events supply list.
\param[in] EventHandle Handle of the event.
\param[in] Type ObjectType of the supply.
\param[in] Quantity Number of supplies.
\return Returns the attribute Handle used to store the supplies.

\fn dmz::UInt32 dmz::EventModuleService::lookup_supply_count (const Handle EventHandle)
\brief Returns the number of supplies in the event's supply list.
\param[in] EventHandle Handle of the event.
\return Returns the number of supplies in the event's supply list.

\fn dmz::Handle dmz::EventModuleService::lookup_supply (
const Handle EventHandle,
const UInt32 Which,
ObjectType &type,
UInt32 &quantity)
\brief looks up a specific supply from the events supply list.
\param[in] EventHandle Handle of the event.
\param[in] Which specifies which supply in the event's list to look up.
\param[out] type Returned ObjectType of the supply.
\param[out] quantity Returned number of supplies.
\return Returns the attribute Handle used to lookup the supplies. Returns zero if no
supplies were found.

*/
