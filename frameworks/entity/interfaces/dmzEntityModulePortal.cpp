/*!

\class dmz::EntityModulePortal
\ingroup Entity
\brief Provides a unified portal interface for Entity plugins.

\fn dmz::EntityModulePortal *dmz::EntityModulePortal::cast (const Plugin *PluginPtr, const String &PluginName)
\brief Casts Plugin pointer to an EntityModulePortal.
\details If the Plugin object implements the EntityModulePortal interface, a pointer to
the EntityModulePortal interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired EntityModulePortal.
\return Returns pointer to the EntityModulePortal. Returns NULL if the PluginPtr does not
implement the EntityModulePortal interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::EntityModulePortal::EntityModulePortal (const PluginInfo &Info)
\brief Constructor.
\param[in] Info PluginInfo containing initialization data.

\fn dmz::EntityModulePortal::~EntityModulePortal ()
\brief Destructor.

\fn dmz::String dmz::EntityModulePortal::get_entity_module_portal_name () const
\brief Gets object module's name.
\return Returns String containing module's name.

\fn dmz::Handle dmz::EntityModulePortal::get_entity_module_portal_handle () const
\brief Gets object module's runtime Handle.
\return Returns Handle containing module's runtime Handle.

\fn void dmz::EntityModulePortal::set_view (const Vector &Position, const Matrix &Orientation, const Vector &Velocity)
\brief Sets portal view.
\param[in] Position Vector containing the translational transform of the portal.
\param[in] Orientation Matrix containing the rotation transform of the portal.
\param[in] Velocity Vector containing the velocity of the portal.


\fn void dmz::EntityModulePortal::get_view (Vector &position, Matrix &orientation, Vector &velocity)
\brief Gets portal view.
\param[out] position Vector to store the translational transform of the portal.
\param[out] orientation Matrix to store the rotation transform of the portal.
\param[out] velocity Vector to store the velocity of the portal.

*/
