/*!

\class dmz::ObjectModuleGrid
\ingroup Object
\brief Object grid module interface.

\fn dmz::ObjectModuleGrid *dmz::ObjectModuleGrid::cast (
const Plugin *PluginPtr,
const String &PluginName)
\brief Casts Plugin pointer to an ObjectModuleGrid.
\details If the Plugin object implements the ObjectModuleGrid interface, a pointer to
the ObjectModuleGrid interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired ObjectModuleGrid.
\return Returns pointer to the ObjectModuleGrid. Returns NULL if the PluginPtr does not
implement the ObjectModuleGrid interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::ObjectModuleGrid::ObjectModuleGrid (const PluginInfo &Info)
\brief Constructor.
\param[in] Info PluginInfo containing initialization data.

\fn dmz::ObjectModuleGrid::~ObjectModuleGrid ()
\brief Destructor.

\fn dmz::String dmz::ObjectModuleGrid::get_object_module_grid_name () const
\brief Gets object grid module's name.
\return Returns String containing object grid module's name.

\fn dmz::Handle dmz::ObjectModuleGrid::get_object_module_grid_handle () const
\brief Gets object grid module's runtime Handle.
\return Returns Handle containing object grid module's runtime Handle.

\fn dmz::Boolean dmz::ObjectModuleGrid::register_object_observer_grid (
ObjectObserverGrid &observer)
\brief Registers object grid observer.
\details Allows an object grid observer to register with the object grid module.
The registered observer wil receive callbacks when ever an object enters or leaves
its defined volume.
\param[in] observer ObjectObserverGrid to register.
\return Returns dmz::True if the object grid observer was successfully registered.

\fn dmz::Boolean dmz::ObjectModuleGrid::update_object_observer_grid (
ObjectObserverGrid &observer)
\brief Updates the registered object grid observer.
\details This function should be called anytime the observer's volume has changed
size, shape, or position.
\param[in] observer ObjectObserverGrid to update.
\return Returns dmz::True if the object grid observer was successfully updated.


\fn dmz::Boolean dmz::ObjectModuleGrid::release_object_observer_grid (
ObjectObserverGrid &observer)
\brief Release an object grid observer.

\fn void dmz::ObjectModuleGrid::find_objects (
const Volume &SearchSpace,
HandleContainer &objects,
const ObjectTypeSet *IncludeTypes,
const ObjectTypeSet *ExcludeTypes)
\brief Finds all objects contained in the Volume \a SearchSpace.
\param[in] SearchSpace Volume to use when searching for objects.
\param[out] objects HandleContainer used to return the Handle of all found objects.
\param[in] IncludeTypes Pointer to an ObjectTypeSet used to filter objects.
If a valid ObjectTypeSet pointer is passed in, a found object must be of an ObjectType
contained in the ObjectTypeSet to be returned in the \a objects HandleContainer.
\param[in] ExcludeTypes Pointer to an ObjectTypeSet used to filter objects.
If a valid ObjectTypeSet pointer is passed in, a found object must \b not be of an
ObjectType contained in the ObjectTypeSet to be returned in the \a objects
HandleContainer.

*/
