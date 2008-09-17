/*!

\class dmz::EventModuleCommon
\ingroup Event
\brief Provides an interface for creating common events.

\fn dmz::EventModuleCommon *dmz::EventModuleCommon::cast (
const Plugin *PluginPtr,
const String &PluginName)
\brief Casts Plugin pointer to an EventModuleCommon.
\details If the Plugin object implements the EventModuleCommon interface, a pointer to
the EventModuleCommon interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired EventModuleCommon.
\return Returns pointer to the EventModuleCommon. Returns NULL if the PluginPtr does not
implement the EventModuleCommon interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::EventModuleCommon::EventModuleCommon (const PluginInfo &Info)
\brief Constructor.
\param[in] Info PluginInfo containing initialization data.

\fn dmz::EventModuleCommon::~EventModuleCommon ()
\brief Destructor.

\fn dmz::String dmz::EventModuleCommon::get_event_module_common_name () const
\brief Gets common event module's name.

\fn dmz::Handle dmz::EventModuleCommon::get_event_module_common_handle () const
\brief Gets common event module's handle.

\fn dmz::Handle dmz::EventModuleCommon::create_launch_event (
const Handle MunitionsHandle,
const Handle TargetHandle)
\brief Creates and closes a launch event.
\note The event attributes may not be changed after this call.
\param[in] MunitionsHandle Handle of the munitions being launched.
\param[in] TargetHandle Handle of the target of the munitions.
\return Returns the handle of the event created.

\fn dmz::Handle dmz::EventModuleCommon::create_open_launch_event (
const Handle MunitionsHandle,
const Handle TargetHandle)
\brief Creates a launch event.
\note The event need to be closed after creation. Event attribute may be changed
until the event is closed.
\param[in] MunitionsHandle Handle of the munitions being launched.
\param[in] TargetHandle Handle of the target of the munitions.
\return Returns the handle of the event created.
\sa dmz::EventModule::close_event()

\fn dmz::Handle dmz::EventModuleCommon::create_detonation_event (
const Handle MunitionsHandle,
const Handle TargetHandle)
\brief Creates and closes a detonation event.
\note The event attributes may not be changed after this call.
\param[in] MunitionsHandle Handle of the munitions being detonated.
\param[in] TargetHandle Handle of the target of the munitions.
\return Returns the handle of the event created.

\fn dmz::Handle dmz::EventModuleCommon::create_open_detonation_event (
const Handle MunitionsHandle,
const Handle TargetHandle)
\brief Creates a detonation event.
\note The event need to be closed after creation. Event attribute may be changed
until the event is closed.
\param[in] MunitionsHandle Handle of the munitions being detonated.
\param[in] TargetHandle Handle of the target of the munitions.
\return Returns the handle of the event created.
\sa dmz::EventModule::close_event()

\fn dmz::Handle dmz::EventModuleCommon::create_collision_event (
const Handle SourceHandle,
const Handle TargetHandle)
\brief Creates and closes a collision event.
\note The event attributes may not be changed after this call.
\param[in] SourceHandle Handle of the source of the collision.
\param[in] TargetHandle Handle of the target of the collision.
\return Returns the handle of the event created.

\fn dmz::Handle dmz::EventModuleCommon::create_open_collision_event (
const Handle SourceHandle,
const Handle TargetHandle)
\brief Creates a collision event.
\note The event need to be closed after creation. Event attribute may be changed
until the event is closed.
\param[in] SourceHandle Handle of the source of the collision.
\param[in] TargetHandle Handle of the target of the collision.
\return Returns the handle of the event created.
\sa dmz::EventModule::close_event()

*/
