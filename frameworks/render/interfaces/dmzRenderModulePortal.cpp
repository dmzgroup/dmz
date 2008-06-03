/*!

\class dmz::RenderModulePortal
\ingroup Render
\brief Render portal interface.
\details The render portal module interface provides an interface for manipulating
the view point into the scene. There may be multiple portals for any given scene.
The primary view into the scene is called the master portal.

\fn dmz::RenderModulePortal::RenderModulePortal (const PluginInfo &Info)
\brief Constructor.

\fn dmz::RenderModulePortal::~RenderModulePortal ()
\brief Destructor.

\fn dmz::RenderModulePortal *dmz::RenderModulePortal::cast (const Plugin *PluginPtr)
\brief Cast Plugin pointer to a RenderModulePortal.
\param[in] PluginPtr Pointer to the Plugin to cast.
\return Returns pointer to the RenderModulePortal. Returns NULL if the PluginPtr does
not implement the RenderModulePortal interface.

\fn dmz::String dmz::RenderModulePortal::get_render_portal_name ()
\brief Gets render portal name.

\fn dmz::UInt32 dmz::RenderModulePortal::get_render_portal_handle ()
\brief Gets render portal Handle.

\fn dmz::Boolean dmz::RenderModulePortal::is_master_portal ()
\brief Tests if portal is the master render portal.
\return Returns dmz::True if the portal is the master portal. There should only ever
be one master portal.

\fn void dmz::RenderModulePortal::set_view (const Vector &Pos, const Matrix &Ori)
\brief Sets portal view.
\param[in] Pos Vector containing the translational transform of the portal.
\param[in] Ori Matrix containing the rotation transform of the portal.

\fn void dmz::RenderModulePortal::get_view (Vector &pos, Matrix &ori) const
\brief Gets portal view.
\param[out] pos Vector containing the translational transform of the portal.
\param[out] ori Matrix containing the rotation transform of the portal.

\fn void dmz::RenderModulePortal::set_fov (const Float32 Value)
\brief Sets portal field of view (FOV).
\param[in] Value Float32 containing the field of view of the portal in radians.

\fn dmz::Float32 dmz::RenderModulePortal::get_fov () const
\brief Gets portal field of view (FOV).
\return Returns the field of view of the portal in radians.

\fn void dmz::RenderModulePortal::set_near_clip_plane (const Float32 Value)
\brief Sets near clip plane.
\param[in] Value Float32 containing the near clip plane of the portal in meters.

\fn dmz::Float32 dmz::RenderModulePortal::get_near_clip_plane () const
\brief Gets near clip plane.
\return Returns the near clip plane of the portal in meters.

\fn void dmz::RenderModulePortal::set_far_clip_plane (const Float32 Value)
\brief Sets far clip plane.
\param[in] Value Float32 containing the far clip plane of the portal in meters.

\fn dmz::Float32 dmz::RenderModulePortal::get_far_clip_plane () const
\brief Gets far clip plane.
\return Returns the far clip plane of the portal in meters.

*/
