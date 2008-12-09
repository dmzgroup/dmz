/*!

\class dmz::ObjectObserverGrid
\ingroup Object
\brief Observer objects stored in the ObjectModuleGrid.

\fn dmz::ObjectObserverGrid *dmz::ObjectObserverGrid::cast (
const Plugin *PluginPtr,
const String &PluginName)
\brief Casts Plugin pointer to an ObjectObserverGrid pointer.
\details If the Plugin object implements the ObjectObserverGrid interface,
a pointer to the ObjectObserverGrid interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired ObjectObserverGrid.
\return Returns pointer to the ObjectObserverGrid. Return NULL if the PluginPtr does not
implement the ObjectObserverGrid interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::ObjectObserverGrid::ObjectObserverGrid (const PluginInfo &Info)
\brief Constructor.
\param[in] Info PluginInfo containing initialization data.

\fn dmz::ObjectObserverGrid::~ObjectObserverGrid ()
\brief Destructor.

\fn dmz::String dmz::ObjectObserverGrid::get_object_observer_grid_name () const
\brief Gets object grid observer's name.
\return Returns String containing object grid observer's name.

\fn dmz::Handle dmz::ObjectObserverGrid::get_object_observer_grid_handle () const
\brief Gets object grid observer's unique runtime handle.
\return Returns unique runtime handle.

\fn const dmz::Volume &dmz::ObjectObserverGrid::get_observer_volume ()
\brief Gets the observer's bounding volume.
\return Returns a reference to the object grid observer's Volume.

\fn void dmz::ObjectObserverGrid::update_object_grid_state (
const ObjectGridStateEnum State,
const Handle ObjectHandle,
const ObjectType &Type,
const Vector &Position)
\brief Updates the state of an object relative to the observers bounding Volume.
\details This call back is made when ever an object enters or leaves the observer's
bounding Volume.
\param[in] State ObjectGridStateEnum indicating if the object is entering or leaving
the observer bounding volume.
\param[in] ObjectHandle Handle of the object.
\param[in] Type ObjectType of the object.
\param[in] Position Vector containing the current position of the object.

*/
